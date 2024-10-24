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
    uint8_t receiver_id = can_id & 0xFF;
    uint8_t sender_id = can_id >> 8 & 0xFF;
    uint8_t target_id = can_id >> 16 & 0xFF; 
    uint8_t sub_function = request[2];
    std::vector<uint8_t> routine_result = {0x00};
    /* reverse ids */
    can_id = receiver_id << 8 | sender_id;
    OtaUpdateStatesEnum ota_state = static_cast<OtaUpdateStatesEnum>(MCU::mcu->getDidValue(OTA_UPDATE_STATUS_DID)[0]);

    if (request.size() < 6 || (request.size() - 1 != request[0]))
    {
        /* Incorrect message length or invalid format - prepare a negative response */
        nrc.sendNRC(can_id,ROUTINE_CONTROL_SID,NegativeResponse::IMLOIF);
        AccessTimingParameter::stopTimingFlag(receiver_id, 0x31);
        return;
    }

    if (sub_function < 0x01 || sub_function > 0x03)
    {
        /* Sub Function not supported - prepare a negative response */
        nrc.sendNRC(can_id,ROUTINE_CONTROL_SID,NegativeResponse::SFNS);
        AccessTimingParameter::stopTimingFlag(receiver_id, 0x31);
        return;
    }

    if (receiver_id == 0x10 && !SecurityAccess::getMcuState(rc_logger))
    {
        nrc.sendNRC(can_id,ROUTINE_CONTROL_SID,NegativeResponse::SAD);
        AccessTimingParameter::stopTimingFlag(receiver_id, 0x31);
        return;
    }

    if ((receiver_id == 0x11 || receiver_id == 0x12 ||
              receiver_id == 0x13 || receiver_id == 0x14) &&
              !ReceiveFrames::getEcuState())
    {
        nrc.sendNRC(can_id,ROUTINE_CONTROL_SID,NegativeResponse::SAD);
        AccessTimingParameter::stopTimingFlag(receiver_id, 0x31);
        return;
    }
    /* when our identifiers will be defined, this range should be smaller */
    if (routine_identifier < 0x0100 || routine_identifier > 0x0601)
    {
        /* Request Out of Range - prepare a negative response */
        nrc.sendNRC(can_id,ROUTINE_CONTROL_SID,NegativeResponse::ROOR);
        AccessTimingParameter::stopTimingFlag(receiver_id, 0x31);
        return;
    }
  
    std::vector<uint8_t> binary_data;
    std::vector<uint8_t> adress_data;
    switch(routine_identifier)
    {
        case 0x0101:
        {
            /* Erase memory or specific data */
            /* call eraseMemory routine */
            LOG_INFO(rc_logger.GET_LOGGER(), "Erase memory routine called for address set.");

            size_t starting_address = 0;
            for(uint8_t i = 5; i < request.size(); i++)
            {
                starting_address |= (request[i] << ((request.size() - i - 1) * 8));
            }
            if(eraseMemory(starting_address, routine_identifier) == false)
            {
                LOG_WARN(rc_logger.GET_LOGGER(), "Erase memory routine failed");
                nrc.sendNRC(can_id, ROUTINE_CONTROL_SID, NegativeResponse::CNC);
                AccessTimingParameter::stopTimingFlag(receiver_id, 0x31);
                return;
            }
            routineControlResponse(can_id, sub_function, routine_identifier, routine_result);
            break;
        }
        case 0x0102:
        {
            /* Erase memory or specific data */
            /* call eraseMemory routine */
            LOG_INFO(rc_logger.GET_LOGGER(), "Erase memory routine called for size set and erase.");
            ssize_t size = 0;
            for(uint8_t i = 5; i < request.size(); i++)
            {
                size |= (request[i] << ((request.size() - i - 1) * 8));
            }
            if(eraseMemory(size, routine_identifier) == false)
            {
                LOG_WARN(rc_logger.GET_LOGGER(), "Erase memory routine failed");
                nrc.sendNRC(can_id, ROUTINE_CONTROL_SID, NegativeResponse::CNC);
                AccessTimingParameter::stopTimingFlag(receiver_id, 0x31);
                return;
            }
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
                AccessTimingParameter::stopTimingFlag(receiver_id, 0x31);
                return;
            }
            if(ota_state != IDLE && ota_state != ERROR)
            {
                LOG_WARN(rc_logger.GET_LOGGER(), "OTA update can be initialised only from an IDLE or ERROR state, current state is {:x}", ota_state);
                nrc.sendNRC(can_id, ROUTINE_CONTROL_SID, NegativeResponse::CNC);
                AccessTimingParameter::stopTimingFlag(receiver_id, 0x31);
                return;
            }
            if(receiver_id != MCU_ID)
            {
                LOG_WARN(rc_logger.GET_LOGGER(), "OTA update can be initialised only by MCU");
                nrc.sendNRC(can_id, ROUTINE_CONTROL_SID, NegativeResponse::CNC);
                AccessTimingParameter::stopTimingFlag(receiver_id, 0x31);
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

            AccessTimingParameter::stopTimingFlag(receiver_id, 0x31);
            break;
        }
        case 0x0301:
        {
            // if(ota_state != VERIFY_SUCCESS)
            // {
            //     return 0;
            // }
            /* call writeToFile routine*/
            LOG_INFO(rc_logger.GET_LOGGER(), "writeToFile routine called.");

            if(receiver_id != MCU_ID)
            {
                auto rds_data = RequestDownloadService::getRdsData();
                auto memory_manager = MemoryManager::getInstance(rc_logger);
                /* Address and path should have been initialized from Request Download. If they are not, initialisation is done here.*/            
                if(memory_manager->getAddress() != rds_data.address || memory_manager->getPath() != DEV_LOOP)
                {
                    LOG_WARN(rc_logger.GET_LOGGER(), "Write to file routine called with uninitialized memory manager instance. Initilization done in write to file routine.");
                    memory_manager->setAddress(rds_data.address);
                    memory_manager->setPath(DEV_LOOP);
                }
                uint8_t file_size_format = MemoryManager::readFromAddress(DEV_LOOP, memory_manager->getAddress(), 1, rc_logger)[0];
                auto file_size_bytes = MemoryManager::readFromAddress(DEV_LOOP, memory_manager->getAddress() + 1, file_size_format, rc_logger);   
                uint8_t binary_offset = sizeof(file_size_format) + file_size_format;
        
                size_t file_size = 0;
                for(uint8_t i = 0; i < file_size_format; i++)
                {
                    file_size |= (file_size_bytes[i] << ((file_size_format - i - 1) * 8));
                }   

                /* Read the binary data from memory */            
                auto binary_data = MemoryManager::readFromAddress(DEV_LOOP, memory_manager->getAddress() + binary_offset, file_size, rc_logger);

                std::string ecu_path;
                if(FileManager::getEcuPath(receiver_id, ecu_path, 1, rc_logger) == 0)
                {
                    LOG_ERROR(rc_logger.GET_LOGGER(), "Invalid ecu path for writting to file:\n{}", ecu_path);
                    nrc.sendNRC(can_id, ROUTINE_CONTROL_SID, NegativeResponse::SFNSIAS);
                    AccessTimingParameter::stopTimingFlag(receiver_id, 0x31);
                    return;
                }
                bool status = MemoryManager::writeToFile(binary_data, ecu_path, rc_logger);

                if(status == 0)
                {
                    LOG_ERROR(rc_logger.GET_LOGGER(), "Write to file {} failed.", ecu_path);
                    nrc.sendNRC(can_id, ROUTINE_CONTROL_SID, NegativeResponse::SFNSIAS);
                    AccessTimingParameter::stopTimingFlag(receiver_id, 0x31);
                    return;
                }
            }
            
            bool status = handleDataCompressionEncryption(receiver_id);
            if(status == 0)
            {
                LOG_ERROR(rc_logger.GET_LOGGER(), "Compression encryption for file failed.");
                nrc.sendNRC(can_id, ROUTINE_CONTROL_SID, NegativeResponse::SFNSIAS);
                AccessTimingParameter::stopTimingFlag(receiver_id, 0x31);
                return;
            }

            routineControlResponse(can_id, sub_function, routine_identifier, routine_result);
            break;
        }
        case 0x0401:
        {
            // if(ota_state != READY)
            // {
            //     return 0;
            // }

            LOG_INFO(rc_logger.GET_LOGGER(), "Verify installation routine called.");
            if(verifySoftware(receiver_id) == false)
            {
                MCU::mcu->setDidValue(OTA_UPDATE_STATUS_DID, {VERIFY_FAILED});
                nrc.sendNRC(can_id, ROUTINE_CONTROL_SID, NegativeResponse::IMLOIF);
            }
            else
            {
                MCU::mcu->setDidValue(OTA_UPDATE_STATUS_DID, {VERIFY_COMPLETE});
                routineControlResponse(can_id, sub_function, routine_identifier, routine_result);
            }
            AccessTimingParameter::stopTimingFlag(receiver_id, 0x31);

            break;
        }
        case 0x0501:
        {
            if(ota_state != IDLE && ota_state != ERROR && ota_state != ACTIVATE_INSTALL_FAILED)
            {
                LOG_WARN(rc_logger.GET_LOGGER(), "OTA software rollback can be started only from an IDLE, ERROR or ACTIVATE_INSTALL_FAILED state, current state is {:x}", ota_state);
                nrc.sendNRC(can_id, ROUTINE_CONTROL_SID, NegativeResponse::CNC);
                AccessTimingParameter::stopTimingFlag(receiver_id, 0x31);
                return;
            }

            if(rollbackSoftware() == false)
            {
                LOG_ERROR(rc_logger.GET_LOGGER(), "Rollback failed.");
                nrc.sendNRC(can_id, ROUTINE_CONTROL_SID, NegativeResponse::CNC);
                AccessTimingParameter::stopTimingFlag(receiver_id, 0x31);
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
                AccessTimingParameter::stopTimingFlag(receiver_id, 0x31);
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
            AccessTimingParameter::stopTimingFlag(receiver_id, 0x31);
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
    uint8_t sw_version = request[5];
#if PYTHON_ENABLED == 1

    namespace py = pybind11;
    py::scoped_interpreter guard{}; // start the interpreter and keep it alive

    /* PROJECT_PATH defined in makefile to be the root folder path (POC)*/
    std::string project_path = PROJECT_PATH;
    std::string path_to_drive_api = project_path + "/src/ota/google_drive_api";
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
#elif PYTHON_ENABLED == 0
    routine_result[0] = 0;
#endif
    /* Map 0-15 to 1-16 */
    uint8_t highNibble = ((sw_version >> 4) & 0x0F) + 1;
    /* Map 0-15 to 1-16 */
    uint8_t lowNibble = (sw_version & 0x0F);
    char buffer[5];
    /* Format the string as "X.Y" */
    std::sprintf(buffer, "%x.%x", highNibble, lowNibble);
    std::string zip_path;
    if(FileManager::getEcuPath(target_ecu, zip_path, 0, rc_logger, buffer) == 0)
    {
        LOG_WARN(rc_logger.GET_LOGGER(), "Failed setting path to software version zip.");
        return false;
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
bool RoutineControl::verifySoftware(uint8_t receiver_id)
{
    auto rds_data = RequestDownloadService::getRdsData();
    std::vector<uint8_t> binary_data;
    if(receiver_id != MCU_ID)
    {
        auto memory_manager = MemoryManager::getInstance(rc_logger);
        if(memory_manager->getAddress() != rds_data.address || memory_manager->getPath() != DEV_LOOP)
        {
            LOG_WARN(rc_logger.GET_LOGGER(), "Verify software routine called with uninitialized memory manager instance. Initilization done in verify software routine.");
            memory_manager->setAddress(rds_data.address);
            memory_manager->setPath(DEV_LOOP);
        }
        uint8_t binary_size_format = MemoryManager::readFromAddress(DEV_LOOP, memory_manager->getAddress(), 1, rc_logger)[0];
        auto binary_size_bytes = MemoryManager::readFromAddress(DEV_LOOP, memory_manager->getAddress() + 1, binary_size_format, rc_logger);    
        uint8_t binary_offset = sizeof(binary_size_format) + binary_size_format;
            
        size_t binary_size = 0;
        for(uint8_t i = 0; i < binary_size_format; i++)
        {
            binary_size |= (binary_size_bytes[i] << ((binary_size_format - i - 1) * 8));
        }
        /* Read the binary data from memory */    
        binary_data = MemoryManager::readFromAddress(DEV_LOOP, memory_manager->getAddress() + binary_offset, binary_size, rc_logger);

        uint8_t checksum = MemoryManager::readFromAddress(DEV_LOOP, memory_manager->getAddress() + 1 + binary_size_format + binary_size, 1, rc_logger)[0];
        uint8_t recomputed_checksum = TransferData::computeChecksum(binary_data.data(), binary_data.size());
        if(checksum != recomputed_checksum)
        {
            LOG_ERROR(rc_logger.GET_LOGGER(),"Error in checksum verification. Sent checksum = 0b{:b}, recomputed checksum = 0b{:b}", checksum, recomputed_checksum);
            return 0;
        }
    }
    else
    {
        std::string path_to_zip;
        if(FileManager::getEcuPath(receiver_id, path_to_zip, 3, rc_logger) == 0)
        {
            LOG_ERROR(rc_logger.GET_LOGGER(), "Error in reading file path, may not exist: {}", path_to_zip);
            return 0;
        }

        /* Read data from the extracted binary */
        binary_data = MemoryManager::readBinary(path_to_zip, rc_logger);
    }

    /* Check if the binary data is in ELF format */
    if (binary_data.size() >= 4)
    {
        /* Get compression type of the data */
        uint8_t data_format = rds_data.data_format & 0xF0;
        bool check_signature = 0;
        switch(data_format)
        {
            case 0x00:
            {
                check_signature = FileManager::validateData(binary_data, FileType::ELF_FILE);
                break;
            }
            case 0x10:
            {
                check_signature = FileManager::validateData(binary_data, FileType::ZIP_FILE);
                break;
            }
            default:
            {

            }
        }
        if(check_signature == 0)
        {
            LOG_ERROR(rc_logger.GET_LOGGER(), "Data signature is nod valid.");
            return 0;
        }
    }

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
    uint8_t binary_size_format = MemoryManager::readFromAddress(DEV_LOOP, DEV_LOOP_PARTITION_2_ADDRESS_START, 1, rc_logger)[0];
    auto binary_size_bytes = MemoryManager::readFromAddress(DEV_LOOP, DEV_LOOP_PARTITION_2_ADDRESS_START + 1, binary_size_format, rc_logger);
    uint8_t binary_offset = sizeof(binary_size_format) + binary_size_format;

    size_t binary_size = 0;
    /* Recreate the number that represents the size of the binary. */
    for(uint8_t i = 0; i < binary_size_format; i++)
    {
        binary_size |= (binary_size_bytes[i] << ((binary_size_format - i - 1) * 8));
    }

    auto binary_data = MemoryManager::readFromAddress(DEV_LOOP, DEV_LOOP_PARTITION_2_ADDRESS_START + binary_offset, binary_size, rc_logger);

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
    memory_manager->setAddress(DEV_LOOP_PARTITION_2_ADDRESS_START);
    bool write_success = memory_manager->writeToAddress(binary_data_info);
    if(write_success == false)
    {
        return 0;
    }
    uint8_t offset = binary_data_size_bytes.size() + 1;

    /* Set address to point to partition 2 */
    memory_manager->setAddress(DEV_LOOP_PARTITION_2_ADDRESS_START + offset);
    /* Write to the second partition (used for rollback) */
    write_success = memory_manager->writeToAddress(binary_data);
    if(write_success == false)
    {
        return 0;
    }
    return 1;
}

bool RoutineControl::handleDataCompressionEncryption(uint8_t receiver_id)
{
    auto rds_data = RequestDownloadService::getRdsData();
    uint8_t data_format = rds_data.data_format;
    uint8_t compression = (data_format & 0xF0) >> 4;
    uint8_t encryption = (data_format & 0x0F);

    if(compression == 0)
    {
        LOG_INFO(rc_logger.GET_LOGGER(), "No compression used.");
    }
    else
    {
        std::string zipFilePath;

        if(receiver_id != MCU_ID)
        {
            if(FileManager::getEcuPath(receiver_id, zipFilePath, 1, rc_logger) == 0)
            {
                LOG_ERROR(rc_logger.GET_LOGGER(), "No valid zip file file found in PROJECT_PATH.");
                // nrc.sendNRC(id, RDS_SID, NegativeResponse::UDNA);
                // AccessTimingParameter::stopTimingFlag(receiver_id, 0x34);
                return 0;
            }            
        }
        else
        {
            if(FileManager::getEcuPath(receiver_id, zipFilePath, 3, rc_logger) == 0)
            {
                LOG_ERROR(rc_logger.GET_LOGGER(), "No valid zip file file found in PROJECT_PATH.");
                // nrc.sendNRC(id, RDS_SID, NegativeResponse::UDNA);
                // AccessTimingParameter::stopTimingFlag(receiver_id, 0x34);
                return 0;
            }
        }

        std::string extractedZipOutputPath;
        if(FileManager::getEcuPath(receiver_id, extractedZipOutputPath, 2, rc_logger) == 0)
        {
            LOG_ERROR(rc_logger.GET_LOGGER(), "No valid ecu path for extracted zip.");
            return 0;
        }

        if (FileManager::extractZipFile(receiver_id, zipFilePath, extractedZipOutputPath, rc_logger))
        {
            LOG_INFO(rc_logger.GET_LOGGER(), "Files extracted successfully");
        } 
        else
        {
            LOG_ERROR(rc_logger.GET_LOGGER(), "Failed to extract files from ZIP archive.");
            MCU::mcu->setDidValue(OTA_UPDATE_STATUS_DID, {WAIT_DOWNLOAD_FAILED});
            return 0;
        }
    }

    if(encryption == 0)
    {
        LOG_INFO(rc_logger.GET_LOGGER(), "No encryption used.");
    }
    else
    {
        /* TODO */
    }

    return 1;
}

bool RoutineControl::eraseMemory(size_t address_or_size_parameter, uint16_t routine_id)
{
    static auto memory_manager = MemoryManager::getInstance(rc_logger);
    switch(routine_id)
    {
        case 0x0101:
        {
            if(address_or_size_parameter < DEV_LOOP_PARTITION_1_ADDRESS_START || address_or_size_parameter > DEV_LOOP_PARTITION_2_ADDRESS_END)
            {
                LOG_INFO(rc_logger.GET_LOGGER(), "Erase memory routine 0x0101 failed setting the addres. Address not valid");
                return 0;
            }
            memory_manager->setAddress(address_or_size_parameter);
            memory_manager->setPath(DEV_LOOP);
            break;
        }
        case 0x0102:
        {
            /* TODO: Work on the upper limit of the size param */
            if((address_or_size_parameter <= 0)) /* || ((memory_manager->getAddress() * SECTOR_SIZE) + address_or_size_parameter) > (DEV_LOOP_PARTITION_2_ADDRESS_END))*/
            {
                LOG_INFO(rc_logger.GET_LOGGER(), "Erase memory routine 0x0102 failed setting the size. Size not valid");
                return 0;
            }
            std::vector<uint8_t> zero_vector(address_or_size_parameter, 0);
            memory_manager->writeToAddress(zero_vector);
            break;
        }
        default:
        {
            return 0;
        }
    }
    return 1;
}
