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

    if (request.size() < 6 || (request.size() - 1 != request[0]))
    {
        /* Incorrect message length or invalid format - prepare a negative response */
        nrc.sendNRC(can_id,ROUTINE_CONTROL_SID,NegativeResponse::IMLOIF);
        AccessTimingParameter::stopTimingFlag(lowerbits, 0x31);
        return;
    }

    if (sub_function < 0x01 || sub_function > 0x03)
    {
        /* Sub Function not supported - prepare a negative response */
        nrc.sendNRC(can_id,ROUTINE_CONTROL_SID,NegativeResponse::SFNS);
        AccessTimingParameter::stopTimingFlag(lowerbits, 0x31);
        return;
    }

    if (lowerbits == 0x10 && !SecurityAccess::getMcuState(rc_logger))
    {
        nrc.sendNRC(can_id,ROUTINE_CONTROL_SID,NegativeResponse::SAD);
        AccessTimingParameter::stopTimingFlag(lowerbits, 0x31);
        return;
    }

    if ((lowerbits == 0x11 || lowerbits == 0x12 ||
              lowerbits == 0x13 || lowerbits == 0x14) &&
              !ReceiveFrames::getEcuState())
    {
        nrc.sendNRC(can_id,ROUTINE_CONTROL_SID,NegativeResponse::SAD);
        AccessTimingParameter::stopTimingFlag(lowerbits, 0x31);
        return;
    }
    /* when our identifiers will be defined, this range should be smaller */
    if (routine_identifier < 0x0100 || routine_identifier > 0x0601)
    {
        /* Request Out of Range - prepare a negative response */
        nrc.sendNRC(can_id,ROUTINE_CONTROL_SID,NegativeResponse::ROOR);
        AccessTimingParameter::stopTimingFlag(lowerbits, 0x31);
        return;
    }
  
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
                LOG_WARN(rc_logger.GET_LOGGER(), "OTA update can be initialised only from an IDLE or ERROR state, current state is {:x}", ota_state);
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
                LOG_ERROR(rc_logger.GET_LOGGER(), "Invalid ecu path for reading binary:\n{}", ecu_path);
                nrc.sendNRC(can_id, ROUTINE_CONTROL_SID, NegativeResponse::SFNSIAS);
                return;
            }
            binary_data = MemoryManager::readBinary(ecu_path, rc_logger);
            memory_manager = MemoryManager::getInstance(rc_logger); 
            adress_data = MemoryManager::readFromAddress(DEV_LOOP, memory_manager->getAddress(), binary_data.size(), rc_logger);

            if(FileManager::getEcuPath(lowerbits, ecu_path, 2, rc_logger) == 0)
            {
                LOG_ERROR(rc_logger.GET_LOGGER(), "Invalid ecu path for writting to file:\n{}", ecu_path);
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
                routineControlResponse(can_id, sub_function, routine_identifier, routine_result);
            }
            break;
        }
        case 0x0501:
        {
            if(ota_state != IDLE && ota_state != ERROR && ota_state != ACTIVATE_INSTALL_FAILED)
            {
                LOG_WARN(rc_logger.GET_LOGGER(), "OTA software rollback can be started only from an IDLE, ERROR or ACTIVATE_INSTALL_FAILED state, current state is {:x}", ota_state);
                nrc.sendNRC(can_id, ROUTINE_CONTROL_SID, NegativeResponse::CNC);
                return;
            }

            if(rollbackSoftware() == false)
            {
                LOG_ERROR(rc_logger.GET_LOGGER(), "Rollback failed.");
                nrc.sendNRC(can_id, ROUTINE_CONTROL_SID, NegativeResponse::CNC);
                return;
            }
            else
            {
                LOG_INFO(rc_logger.GET_LOGGER(), "Rollback succesfull.");
            }
            break;
        }
        case 0x0601:
        {
            LOG_INFO(rc_logger.GET_LOGGER(), "Activate software routine called. Saving the current software..");
            if(saveCurrentSoftware() == false)
            {
                LOG_ERROR(rc_logger.GET_LOGGER(), "Current software saving failed.");
                MCU::mcu->setDidValue(OTA_UPDATE_STATUS_DID, {ACTIVATE_INSTALL_FAILED});
                nrc.sendNRC(can_id, ROUTINE_CONTROL_SID, NegativeResponse::IMLOIF);
                return;
            }
            LOG_INFO(rc_logger.GET_LOGGER(), "Current software saved. Activating the new software..");
            
            if(activateSoftware() == false)
            {
                MCU::mcu->setDidValue(OTA_UPDATE_STATUS_DID, {ACTIVATE_INSTALL_FAILED});
                LOG_ERROR(rc_logger.GET_LOGGER(), "Software activation failed, restoring previous software version..");
                rollbackSoftware();
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
    std::string cmd = std::string(PROJECT_PATH) + "/config/installUpdates.sh " + std::to_string(pid) + " " + pname + " " + "activate";
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
        LOG_ERROR(rc_logger.GET_LOGGER(), "Error: Unable to open /proc/{}/comm", pid);
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
        LOG_ERROR(rc_logger.GET_LOGGER(), "Error: Unable to resolve path for /proc/{}/exe", pid);
        return false;
    }
    return 1;
}

bool RoutineControl::rollbackSoftware()
{
    LOG_INFO(rc_logger.GET_LOGGER(), "Rollback routine called.");
    /* Get the size of the stored binary. 
        First byte represents the size in bytes of the binary size. 03 means the following 3 bytes are used for representing the size 
        Following n bytes are used to represent the size.
        The following bytes after this are the binary data.
    */
    uint8_t binary_size_format = MemoryManager::readFromAddress(DEV_LOOP, DEV_LOOP_PARTITION_2_ADDRESS, 1, rc_logger)[0];
    auto binary_size_bytes = MemoryManager::readFromAddress(DEV_LOOP, DEV_LOOP_PARTITION_2_ADDRESS + 1, binary_size_format, rc_logger);
    uint8_t binary_offset = sizeof(binary_size_format) + binary_size_format;

    size_t binary_size = 0;
    /* Recreate the number that represents the size of the binary. */
    for(uint8_t i = 0; i < binary_size_format; i++)
    {
        binary_size |= (binary_size_bytes[i] << ((binary_size_format - i - 1) * 8));
    }

    auto binary_data = MemoryManager::readFromAddress(DEV_LOOP, DEV_LOOP_PARTITION_2_ADDRESS + binary_offset, binary_size, rc_logger);

    /* Check is software is saved in the memory by checking the .elf extension */
    if( (binary_data[1] != 'E') || 
        (binary_data[2] != 'L') || 
        (binary_data[3] != 'F'))
    {
        LOG_WARN(rc_logger.GET_LOGGER(), "Current software is not saved in the memory before doing the rollback. Aborting rollback..");
        return 0;        
    }
    pid_t pid;
    std::string pname, ppath;
    if(getCurrentProcessInfo(pid, pname, ppath) == false)
    {
        return 0;
    }
    ppath += "_restored";
    bool write_status = MemoryManager::writeToFile(binary_data, ppath, rc_logger);

    if(write_status == 0)
    {
        return 0;
    }

    std::string cmd = std::string(PROJECT_PATH) + "/config/installUpdates.sh " + std::to_string(pid) + " " + pname + " " + "restore";
    int install_update_status = system(cmd.c_str());
    if(install_update_status != 0)
    {
        return 0;
    }
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

    MemoryManager* memory_manager = MemoryManager::getInstance(rc_logger);   
    memory_manager->setPath(DEV_LOOP);
    auto binary_data = MemoryManager::readBinary(ppath, rc_logger);
    size_t binary_data_size = binary_data.size();

    std::vector<uint8_t> binary_data_size_bytes;
    bool first_byte_found = false;
    uint8_t byte;
    for(char i = sizeof(binary_data_size) - 1; i >=0; --i)
    {
        byte = (binary_data_size >> (i * 8)) & 0xFF;
        if(byte != 0 || first_byte_found == true)
        {
            binary_data_size_bytes.push_back(byte);
            first_byte_found = true;
        }
    }
    /* Vector containing the binary_data_size format and the size in bytes */
    std::vector<uint8_t> binary_data_info;
    binary_data_info.push_back(binary_data_size_bytes.size());
    binary_data_info.insert(binary_data_info.end(), binary_data_size_bytes.begin(), binary_data_size_bytes.end());
    /* Write at the start of partition 2 the informations about the binary */
    memory_manager->setAddress(DEV_LOOP_PARTITION_2_ADDRESS);
    bool write_success = memory_manager->writeToAddress(binary_data_info);
    if(write_success == false)
    {
        return 0;
    }
    uint8_t offset = binary_data_size_bytes.size() + 1;

    /* Set address to point to partition 2 */
    memory_manager->setAddress(DEV_LOOP_PARTITION_2_ADDRESS + offset);
    /* Write to the second partition (used for rollback) */
    write_success = memory_manager->writeToAddress(binary_data);
    if(write_success == false)
    {
        return 0;
    }
    return 1;
}