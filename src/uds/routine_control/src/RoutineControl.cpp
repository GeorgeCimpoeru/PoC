#include "../include/RoutineControl.h"
#include "../../../ecu_simulation/BatteryModule/include/BatteryModule.h"
#include "../../../ecu_simulation/EngineModule/include/EngineModule.h"
#include "../../../ecu_simulation/DoorsModule/include/DoorsModule.h"
#include "../../../ecu_simulation/HVACModule/include/HVACModule.h"
#include "../../../mcu/include/MCUModule.h"
#include <limits.h>

RoutineControl::RoutineControl(int socket, Logger& rc_logger) 
            : generate_frames(socket, rc_logger), rc_logger(rc_logger)
{
    this->socket = socket;
}

/* Function to handle the RoutineControl request */
void RoutineControl::routineControl(canid_t can_id, const std::vector<uint8_t>& request)
{
    uint16_t routine_identifier = request[3] << 8 | request[4];
    std::vector<uint8_t> response;
    NegativeResponse nrc(socket, rc_logger);
    uint8_t lowerbits = can_id & 0xFF;
    uint8_t upperbits = can_id >> 8 & 0xFF;
    uint8_t target_id = can_id >> 16 & 0xFF; 
    uint8_t sub_function = request[2];
    std::vector<uint8_t> routine_result = {0x00};
    /* reverse ids */
    can_id = lowerbits << 8 | upperbits;
    OtaUpdateStatesEnum ota_state = static_cast<OtaUpdateStatesEnum>(MCU::mcu->getDidValue(OTA_UPDATE_STATUS_DID)[0]);

    if (request.size() < 6)
    {
        /* Incorrect message length or invalid format - prepare a negative response */
        nrc.sendNRC(can_id,ROUTINE_CONTROL_SID,NegativeResponse::IMLOIF);
        AccessTimingParameter::stopTimingFlag(lowerbits, 0x31);
        return;
    }
    else if (sub_function < 0x01 || sub_function > 0x03)
    {
        /* Sub Function not supported - prepare a negative response */
        nrc.sendNRC(can_id,ROUTINE_CONTROL_SID,NegativeResponse::SFNS);
        AccessTimingParameter::stopTimingFlag(lowerbits, 0x31);
        return;
    }
    else if (lowerbits == 0x10 && !SecurityAccess::getMcuState(rc_logger))
    {
        nrc.sendNRC(can_id,ROUTINE_CONTROL_SID,NegativeResponse::SAD);
        AccessTimingParameter::stopTimingFlag(lowerbits, 0x31);
    }
    else if ((lowerbits == 0x11 || lowerbits == 0x12 ||
              lowerbits == 0x13 || lowerbits == 0x14) &&
              !ReceiveFrames::getEcuState())
    {
        nrc.sendNRC(can_id,ROUTINE_CONTROL_SID,NegativeResponse::SAD);
        AccessTimingParameter::stopTimingFlag(lowerbits, 0x31);
    }
    /* when our identifiers will be defined, this range should be smaller */
    else if (routine_identifier < 0x0100 || routine_identifier > 0xEFFF)
    {
        /* Request Out of Range - prepare a negative response */
        nrc.sendNRC(can_id,ROUTINE_CONTROL_SID,NegativeResponse::ROOR);
        AccessTimingParameter::stopTimingFlag(lowerbits, 0x31);
        return;
    }
    else
    {   
        std::vector<uint8_t> binary_data;
        std::vector<uint8_t> adress_data;
        MemoryManager* memory_manager = MemoryManager::getInstance(0x0801, DEV_LOOP, rc_logger);
        switch(routine_identifier)
        {
            case 0x0101:
            {
                /* Erase memory or specific data */
                /* call eraseMemory routine */
                LOG_INFO(rc_logger.GET_LOGGER(), "eraseMemory routine called.");
                routineControlResponse(can_id, sub_function, routine_identifier, routine_result);
                break;
            }
            case 0x0201:
            {
                /* Initialise OTA Update */
                if(DiagnosticSessionControl::getCurrentSessionToString() != "EXTENDED_DIAGNOSTIC_SESSION")
                {
                    LOG_WARN(rc_logger.GET_LOGGER(), "OTA update can be initialised only in EXTENDED_DIAGNOSTIC_SESSION");
                    nrc.sendNRC(can_id, ROUTINE_CONTROL_SID, NegativeResponse::SFNSIAS);
                    return;
                }
                if(ota_state != IDLE && ota_state != ERROR)
                {
                    LOG_INFO(rc_logger.GET_LOGGER(), "OTA update can be initialised only from an IDLE or ERROR state, current state is {:x}", ota_state);
                    nrc.sendNRC(can_id, ROUTINE_CONTROL_SID, NegativeResponse::CNC);
                    return;
                }

                LOG_INFO(rc_logger.GET_LOGGER(), "Initialise OTA update routine called.");
                if(initialiseOta(target_id, request, routine_result) == false)
                {
                    MCU::mcu->setDidValue(OTA_UPDATE_STATUS_DID, {ERROR});
                    nrc.sendNRC(can_id, ROUTINE_CONTROL_SID, NegativeResponse::IMLOIF);
                }
                else
                {
                    MCU::mcu->setDidValue(OTA_UPDATE_STATUS_DID, {INIT});
                    routineControlResponse(can_id, sub_function, routine_identifier, routine_result);
                }
                break;
            }
            case 0x0301:
            {
                /* call writeToFile routine*/
                LOG_INFO(rc_logger.GET_LOGGER(), "writeToFile routine called.");
                std::string ecu_path;
                if(FileManager::getEcuPath(lowerbits, ecu_path, 1, rc_logger) == 0)
                {
                    LOG_INFO(rc_logger.GET_LOGGER(), "Invalid ecu path for reading binary:\n{}", ecu_path);
                    nrc.sendNRC(can_id, ROUTINE_CONTROL_SID, NegativeResponse::SFNSIAS);
                    return;
                }
                binary_data = MemoryManager::readBinary(ecu_path, rc_logger);
                memory_manager = MemoryManager::getInstance(rc_logger); 
                adress_data = MemoryManager::readFromAddress(DEV_LOOP, memory_manager->getAddress(), binary_data.size(), rc_logger);

                if(FileManager::getEcuPath(lowerbits, ecu_path, 2, rc_logger) == 0)
                {
                    LOG_INFO(rc_logger.GET_LOGGER(), "Invalid ecu path for writting to file:\n{}", ecu_path);
                    nrc.sendNRC(can_id, ROUTINE_CONTROL_SID, NegativeResponse::SFNSIAS);
                    return;
                }
                MemoryManager::writeToFile(adress_data, ecu_path, rc_logger);
                routineControlResponse(can_id, sub_function, routine_identifier, routine_result);
                break;
            }
            case 0x0401:
            {
                LOG_INFO(rc_logger.GET_LOGGER(), "Verify installation routine called.");
                if(verifySoftware() == false)
                {
                    MCU::mcu->setDidValue(OTA_UPDATE_STATUS_DID, {VERIFY_FAILED});
                    nrc.sendNRC(can_id, ROUTINE_CONTROL_SID, NegativeResponse::IMLOIF);
                }
                else
                {
                    MCU::mcu->setDidValue(OTA_UPDATE_STATUS_DID, {VERIFY_COMPLETE});
                }
                break;
            }
            case 0x0501:
            {
                LOG_INFO(rc_logger.GET_LOGGER(), "Rollback routine called.");
                break;
            }
            case 0x0601:
            {
                LOG_INFO(rc_logger.GET_LOGGER(), "Activate software routine called. Saving the current software..");
                if(saveCurrentSoftware() == false)
                {
                    LOG_INFO(rc_logger.GET_LOGGER(), "Current software saving failed.");
                    MCU::mcu->setDidValue(OTA_UPDATE_STATUS_DID, {ACTIVATE_INSTALL_FAILED});
                    nrc.sendNRC(can_id, ROUTINE_CONTROL_SID, NegativeResponse::IMLOIF);
                    return;
                }
                LOG_INFO(rc_logger.GET_LOGGER(), "Current software saved. Activating the new software..");
                
                if(activateSoftware() == false)
                {
                    MCU::mcu->setDidValue(OTA_UPDATE_STATUS_DID, {ACTIVATE_INSTALL_FAILED});
                }
                else
                {
                    MCU::mcu->setDidValue(OTA_UPDATE_STATUS_DID, {ACTIVATE_INSTALL_COMPLETE});
                    routineControlResponse(can_id, sub_function, routine_identifier, routine_result);
                }
                break;
            }
            default:
                LOG_INFO(rc_logger.GET_LOGGER(), "Unknown routine.");
                break;
        }
    }
}

void RoutineControl::routineControlResponse(canid_t can_id, const uint8_t sub_function, const uint16_t& routine_identifier, std::vector<uint8_t>& routine_result)
{
    uint8_t receiver_id = can_id >> 8 & 0xFF;

    generate_frames.routineControl(can_id, sub_function, routine_identifier, routine_result, true);
    LOG_INFO(rc_logger.GET_LOGGER(), "Service with SID {:x} successfully sent the response frame for routine: {:2x}", 0x31, routine_identifier);
                
    AccessTimingParameter::stopTimingFlag(receiver_id, 0x31);
}

bool RoutineControl::initialiseOta(uint8_t target_ecu, const std::vector<uint8_t>& request, std::vector<uint8_t>& routine_result)
{
    /* More checks here */

    namespace py = pybind11;
    py::scoped_interpreter guard{}; // start the interpreter and keep it alive

    /* PROJECT_PATH defined in makefile to be the root folder path (POC)*/
    std::string project_path = PROJECT_PATH;
    std::string path_to_drive_api = project_path + "/src/ota/google_drive_api";
    uint8_t sw_version = request[5];
    short version_size = -1;
    try
    {
        auto sys = py::module::import("sys");
        sys.attr("path").attr("append")(path_to_drive_api);

        /* Get the created Python module */
        py::module python_module = py::module::import("GoogleDriveApi");
        /* From the module, get the needed functionality (gDrive object) */
        py::object gGdrive_object = python_module.attr("gDrive");

        /* Call the searchVersion method from GoogleDriveApi.py that returns the size of the version, or 0 if not found*/
        version_size = (gGdrive_object.attr("downloadFile")(target_ecu, sw_version)).cast<short>();
    }
    catch(const py::error_already_set& e)
    {
        LOG_ERROR(rc_logger.GET_LOGGER(), "Python error: {}", e.what());
        MCU::mcu->setDidValue(OTA_UPDATE_STATUS_DID, {ERROR});
        return false;
    }
    if(version_size == -1)
    {
        /* NRC*/
        return false;
    }
    else
    {
        /* Send response */
        routine_result[0] = version_size;
    }
    return true;
    
}

bool RoutineControl::activateSoftware()
{
    MCU::mcu->setDidValue(OTA_UPDATE_STATUS_DID, {ACTIVATE});
    /* Checks for the system exit value to update the status in COMPLETED or FAILED */
    pid_t pid;
    std::string pname, ppath;
    if(getCurrentProcessInfo(pid, pname, ppath) == 0)
    {
        return 0;
    }
    std::string cmd = std::string(PROJECT_PATH) + "/config/installUpdates.sh " + std::to_string(pid) + " " + pname;
    int install_update_status = system(cmd.c_str());
    if(install_update_status != 0)
    {
        return 0;
    }
    
    return 1;
}
bool RoutineControl::verifySoftware()
{
    return true;
}

bool RoutineControl::getCurrentProcessInfo(pid_t& pid, std::string& pname, std::string& ppath) 
{
    /* Get the current process ID */
    pid = getpid();
    /* Open the process file */ 
    std::ifstream file("/proc/" + std::to_string(pid) + "/comm");

    if (file.is_open()) 
    {
        std::getline(file, pname);
        file.close();
    } 
    else
    {
        std::cerr << "Error: Unable to open /proc/" << pid << "/comm" << std::endl;
        return 0;
    }

    /* Get the path of the executable using /proc/self/exe */
    char result[PATH_MAX];
    ssize_t count = readlink(("/proc/" + std::to_string(pid) + "/exe").c_str(), result, PATH_MAX);
    if (count != -1) 
    {
        ppath = std::string(result, count);
    } 
    else 
    {
        std::cerr << "Error: Unable to resolve path for /proc/" << pid << "/exe" << std::endl;
        return false;
    }
    return 1;
}

bool RoutineControl::rollbackSoftware()
{
    return 1;
}

bool RoutineControl::saveCurrentSoftware()
{
    pid_t pid;
    std::string pname, ppath;
    if(getCurrentProcessInfo(pid, pname, ppath) == 0)
    {
        return 0;
    }
    /* Read the current software binary data */
    auto binary_data = MemoryManager::readBinary(ppath, rc_logger);

    MemoryManager* memory_manager = MemoryManager::getInstance(rc_logger);   
    memory_manager->setPath(DEV_LOOP);
    /* Set address to point to partition 2 */
    memory_manager->setAddress(206848);
    /* Write to the second partition (used for rollback) */
    bool write_success = memory_manager->writeToAddress(binary_data);
    if(write_success == false)
    {
        return 0;
    }
    return 1;
}
