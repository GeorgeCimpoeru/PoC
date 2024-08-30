#include "../include/RoutineControl.h"
#include "../../../ecu_simulation/BatteryModule/include/BatteryModule.h"
#include "../../../mcu/include/MCUModule.h"

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
    /* reverse ids */
    can_id = lowerbits << 8 | upperbits;
    if (request.size() < 6)
    {
        /* Incorrect message length or invalid format - prepare a negative response */
        nrc.sendNRC(can_id,ROUTINE_CONTROL_SID,NegativeResponse::IMLOIF);
        return;
    }
    else if (request[2] < 0x01 || request [2] > 0x03)
    {
        /* Sub Function not supported - prepare a negative response */
        nrc.sendNRC(can_id,ROUTINE_CONTROL_SID,NegativeResponse::SFNS);
        return;
    }
    else if (!SecurityAccess::getMcuState())
    {
        nrc.sendNRC(can_id,ROUTINE_CONTROL_SID,NegativeResponse::SAD);
    }
    /* when our identifiers will be defined, this range should be smaller */
    else if (routine_identifier < 0x0100 || routine_identifier > 0xEFFF)
    {
        /* Request Out of Range - prepare a negative response */
        nrc.sendNRC(can_id,ROUTINE_CONTROL_SID,NegativeResponse::ROOR);
        return;
    }
    else
    {   
        std::string command;
        std::string path_to_main;
        std::cout << "status is: " << std::string(PROJECT_PATH) << std::endl;
        if (access((std::string(PROJECT_PATH) + "/main_mcu_new").c_str(), F_OK) == 0 && lowerbits == 0x10) {
            path_to_main = std::string(PROJECT_PATH) + "/main_mcu_new";
            command = "./../../config/installUpdates.sh";
        }
        else if (access((std::string(PROJECT_PATH) + "/main_battery_new").c_str(), F_OK) == 0 && lowerbits == 0x11) {
            path_to_main = std::string(PROJECT_PATH) + "/main_battery_new";
            command = "./../../../config/installUpdates.sh";
            MemoryManager* managerInstance = MemoryManager::getInstance(0x0801, "/dev/loop25", rc_logger);
            managerInstance->getAddress();
        }
        else if (access((std::string(PROJECT_PATH) + "/main_doors_new").c_str(), F_OK) == 0 && lowerbits == 0x12) {
            path_to_main = std::string(PROJECT_PATH) + "/main_doors_new";
            command = "./../../../config/installUpdates.sh";
        }
        else if (access((std::string(PROJECT_PATH) + "/main_engine_new").c_str(), F_OK) == 0 && lowerbits == 0x13) {
            path_to_main = std::string(PROJECT_PATH) + "/main_engine_new";
            command = "./../../../config/installUpdates.sh";
        }
        else if (access((std::string(PROJECT_PATH) + "/main_hvac_new").c_str(), F_OK) == 0 && lowerbits == 0x14) {
            path_to_main = std::string(PROJECT_PATH) + "/main_hvac_new";
            command = "./../../../config/installUpdates.sh";
        }
        else
        {
            LOG_ERROR(rc_logger.GET_LOGGER(), "No valid main_xxx_new file found in PROJECT_PATH.");
            return;
        }
        std::vector<uint8_t> binary_data;
        std::vector<uint8_t> adress_data;
        MemoryManager* memory_manager;
        switch(routine_identifier)
        {
            case 0x0101:
                /* Erase memory or specific data */
                /* call eraseMemory routine */
                LOG_INFO(rc_logger.GET_LOGGER(), "eraseMemory routine called.");
                routineControlResponse(can_id, request, routine_identifier);
                break;
            case 0x0201:
                /* Install updates */
                /* call installUpdates routine*/
                LOG_INFO(rc_logger.GET_LOGGER(), "installUpdates routine called.");
                system(command.c_str());
                routineControlResponse(can_id, request, routine_identifier);
                break;
            case 0x0301:
                /* call writeToFile routine*/
                LOG_INFO(rc_logger.GET_LOGGER(), "writeToFile routine called.");
                binary_data = MemoryManager::readBinary(path_to_main, rc_logger);
                memory_manager = MemoryManager::getInstance(rc_logger); 
                adress_data = MemoryManager::readFromAddress("/dev/loop25", memory_manager->getAddress(), binary_data.size(), rc_logger);
                MemoryManager::writeToFile(adress_data, selectEcuPath(can_id), rc_logger);
                routineControlResponse(can_id, request, routine_identifier);
                break;
            default:
                LOG_INFO(rc_logger.GET_LOGGER(), "Unknown routine.");
                break;
        }
    }
}

void RoutineControl::routineControlResponse(canid_t can_id, const std::vector<uint8_t>& request, const uint16_t& routine_identifier)
{
    uint8_t receiver_id = can_id >> 8 & 0xFF;
    switch(receiver_id)
        {
            case 0x10:
                /* Send response frame to MCU */
                generate_frames.routineControl(can_id, request[2], routine_identifier, true);
                LOG_INFO(rc_logger.GET_LOGGER(), "Service with SID {:x} successfully sent the response frame for routine: {:2x}", 0x31, routine_identifier);
                MCU::mcu->stop_flags[0x31] = false;
                break;
            case 0x11:
                /* Send response frame to Battery */
                generate_frames.routineControl(can_id, request[2], routine_identifier, true);
                LOG_INFO(rc_logger.GET_LOGGER(), "Service with SID {:x} successfully sent the response frame for routine: {:2x}", 0x31, routine_identifier);
                battery->stop_flags[0x31] = false;
                break;
            case 0x12:
                /* Send response frame to Doors */
                generate_frames.routineControl(can_id, request[2], routine_identifier, true);
                LOG_INFO(rc_logger.GET_LOGGER(), "Service with SID {:x} successfully sent the response frame for routine: {:2x}", 0x31, routine_identifier);
                /* doors->stop_flags[0x31] = false; */
                break;
            case 0x13:
                /* Send response frame to Engine */
                generate_frames.routineControl(can_id, request[2], routine_identifier, true);
                LOG_INFO(rc_logger.GET_LOGGER(), "Service with SID {:x} successfully sent the response frame for routine: {:2x}", 0x31, routine_identifier);
                /* engine->stop_flags[0x31] = false; */
                break;
            case 0x14:
                /* Send response frame to HVAC*/
                generate_frames.routineControl(can_id, request[2], routine_identifier, true);
                LOG_INFO(rc_logger.GET_LOGGER(), "Service with SID {:x} successfully sent the response frame for routine: {:2x}", 0x31, routine_identifier);
                /* hvac->stop_flags[0x31] = false; */
                break;
            default:
                LOG_ERROR(rc_logger.GET_LOGGER(), "Module with id {:x} not supported.", receiver_id);
                break;
        }
}

std::string RoutineControl::selectEcuPath(canid_t can_id)
{
    uint8_t receiver_id = can_id >> 8 & 0xFF;

        switch(receiver_id)
        {
            case 0x10:
                return std::string(PROJECT_PATH) + "/src/mcu/main_mcu_new";
            break;
            case 0x11:
                return std::string(PROJECT_PATH) + "/src/ecu_simulation/BatteryModule/main_battery_new";
            break;
            case 0x12:
                return std::string(PROJECT_PATH) + "/src/ecu_simulation/DoorsModule/main_doors_new";
            break;
            case 0x13:
                return std::string(PROJECT_PATH) + "/src/ecu_simulation/EngineModule/main_engine_new";
            break;
            case 0x14:
                return std::string(PROJECT_PATH) + "/src/ecu_simulation/HvacModule/main_hvac_new";
            break;
            default:
                LOG_ERROR(rc_logger.GET_LOGGER(), "No valid path.");
                return "no valid path";
            break;
        }
    return "no path found";
}