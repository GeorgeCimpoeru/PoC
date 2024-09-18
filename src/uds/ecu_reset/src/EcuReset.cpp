#include "../include/EcuReset.h"
#include "../../../mcu/include/MCUModule.h"
#include "../../../ecu_simulation/BatteryModule/include/BatteryModule.h"
#include "../../../ecu_simulation/EngineModule/include/EngineModule.h"
#include "../../../ecu_simulation/DoorsModule/include/DoorsModule.h"
#include "../../../ecu_simulation/HVACModule/include/HVACModule.h"

EcuReset::EcuReset(uint32_t can_id, uint8_t sub_function, int socket, Logger &logger)
    : can_id(can_id), sub_function(sub_function), socket(socket), ECUResetLog(logger)
{
}

EcuReset::~EcuReset()
{
}

void EcuReset::ecuResetRequest(const std::vector<uint8_t>& request)
{
    uint8_t lowerbits = can_id & 0xFF;
    uint32_t new_id = ((can_id & 0xFF) << 8) | ((can_id >> 8) & 0xFF);

    NegativeResponse nrc(socket, ECUResetLog);
    if (request.size() < 3 || (request.size() != static_cast<size_t>(request[0] + 1)))
    {
        nrc.sendNRC(new_id,0x11,NegativeResponse::IMLOIF);
        stopTimingFlag(lowerbits);
        return;
    }

    if (sub_function != 0x01 && sub_function != 0x02)
    {
        nrc.sendNRC(new_id,0x11,NegativeResponse::SFNS);
        stopTimingFlag(lowerbits);
        return;
    }
    if (lowerbits == 0x10 && !SecurityAccess::getMcuState(ECUResetLog))
    {
        nrc.sendNRC(new_id, 0x11, NegativeResponse::SAD);
        MCU::mcu->stop_flags[0x11] = false;
        return;
    }
    else
    if (lowerbits == 0x11 && !ReceiveFrames::getBatteryState())
    {
        nrc.sendNRC(new_id, 0x11, NegativeResponse::SAD);
        battery->stop_flags[0x11] = false;
        return;
    }
    else
    if (lowerbits == 0x12 && !ReceiveFrames::getEngineState())
    {
        nrc.sendNRC(new_id, 0x11, NegativeResponse::SAD);
        engine->stop_flags[0x11] = false;
        return;
    }
    else
    if (lowerbits == 0x13 && !ReceiveFrames::getDoorsState())
    {
        nrc.sendNRC(new_id, 0x11, NegativeResponse::SAD);
        doors->stop_flags[0x11] = false;
        return;
    }
    else
    if (lowerbits == 0x14 && !ReceiveFrames::getHvacState())
    {
        nrc.sendNRC(new_id, 0x11, NegativeResponse::SAD);
        hvac->_ecu->stop_flags[0x11] = false;
        return;
    }
    /* Hard Reset case */
    if (sub_function == 0x01)
    {
        LOG_INFO(ECUResetLog.GET_LOGGER(), "Reset Mode: Hard Reset");
        this->hardReset();
    /* Keys off Reset case */
    }
    else if (sub_function == 0x02)
    {
        LOG_INFO(ECUResetLog.GET_LOGGER(), "Reset Mode: Key Off Reset");
        this->keyOffReset();
    }
}
void EcuReset::hardReset()
{  
    uint8_t lowerbits = can_id & 0xFF;
    /* Send response */
    this->ecuResetResponse();
    switch(lowerbits)
    {
        case 0x10:
        {    
            const char *args[] = {"/bin/bash", "../autoscripts/reset_hard.sh", "main_mcu", NULL};
            execvp(args[0], const_cast<char *const *>(args));

            perror("execvp failed");
            exit(EXIT_FAILURE);
            break;
        }
        case 0x11:
        {
            const char *args[] = {"/bin/bash", "../../autoscripts/reset_hard.sh", "main_battery", NULL};
            execvp(args[0], const_cast<char *const *>(args));

            perror("execvp failed");
            exit(EXIT_FAILURE);
            break;
        }
        case 0x12:
        {
            const char *args[] = {"/bin/bash", "../../autoscripts/reset_hard.sh", "main_engine", NULL};
            execvp(args[0], const_cast<char *const *>(args));

            perror("execvp failed");
            exit(EXIT_FAILURE);
            break;
        }
        case 0x13:
        {
            const char *args[] = {"/bin/bash", "../../autoscripts/reset_hard.sh", "main_doors", NULL};
            execvp(args[0], const_cast<char *const *>(args));

            perror("execvp failed");
            exit(EXIT_FAILURE);
            break;
        }
        case 0x14:
        {
            const char *args[] = {"/bin/bash", "../../autoscripts/reset_hard.sh", "main_hvac", NULL};
            execvp(args[0], const_cast<char *const *>(args));

            perror("execvp failed");
            exit(EXIT_FAILURE);
            break;
        }
        default:
            LOG_ERROR(ECUResetLog.GET_LOGGER(), "ECU doesn't exist in hardReset");
    }
}

void EcuReset::keyOffReset()
{
    uint8_t lowerbits = can_id & 0xFF;

    std::string file_path;
    /* Path to the data file */
    switch (lowerbits)
    {
    case 0x10:
        file_path = "mcu_data.txt";
        break;
    case 0x11:
        file_path = "battery_data.txt";
        break;
    case 0x12:
        file_path = "engine_data.txt";
        break;
    case 0x13:
        file_path = "doors_data.txt";
        break;
    case 0x14:
        file_path = "hvac_data.txt";
        break;
    default:
        LOG_ERROR(ECUResetLog.GET_LOGGER(), "ECU doesn't exist");
        break;
    }

    /* Read the current file contents into memory */
    std::ifstream infile(file_path);
    std::stringstream buffer;
    /* Read the entire file into the buffer */
    buffer << infile.rdbuf();
    infile.close();

    /* Store the original content */
    std::string original_file_contents = buffer.str(); 

    switch (lowerbits)
    {
    case 0x10:
    {
        std::ofstream outfile("old_mcu_data.txt");
        if (!outfile.is_open())
        {
            throw std::runtime_error("Failed to open file: old_mcu_data.txt");
        }
        outfile << original_file_contents;
        outfile.close();
        break;
    }
    case 0x11:
    {
        std::ofstream outfile("old_battery_data.txt");
        if (!outfile.is_open())
        {
            throw std::runtime_error("Failed to open file: old_battery_data.txt");
        }
        outfile << original_file_contents;
        outfile.close();
        break;
    }
    case 0x12:
    {
        std::ofstream outfile("old_engine_data.txt");
        if (!outfile.is_open())
        {
            throw std::runtime_error("Failed to open file: old_engine_data.txt");
        }
        outfile << original_file_contents;
        outfile.close();
        break;
    }
    case 0x13:
    {
        std::ofstream outfile("old_doors_data.txt");
        if (!outfile.is_open())
        {
            throw std::runtime_error("Failed to open file: old_doors_data.txt");
        }
        outfile << original_file_contents;
        outfile.close();
        break;
    }
        break;
    case 0x14:
    {
        std::ofstream outfile("old_hvac_data.txt");
        if (!outfile.is_open())
        {
            throw std::runtime_error("Failed to open file: old_hvac_data.txt");
        }
        outfile << original_file_contents;
        outfile.close();
        break;
    }
        break;
    default:
        LOG_ERROR(ECUResetLog.GET_LOGGER(), "ECU doesn't exist");
        break;
    }
    
    /* Reset the program */
    hardReset();
}

void EcuReset::ecuResetResponse()
{
    /* Generate the response frame and send it */
    GenerateFrames generate_frames(socket, ECUResetLog);

    uint8_t lowerbits = can_id & 0xFF;
    
    uint32_t new_id = ((can_id & 0xFF) << 8) | ((can_id >> 8) & 0xFF);

    generate_frames.ecuReset(new_id, sub_function, socket, true);
    LOG_INFO(ECUResetLog.GET_LOGGER(), "Service with SID {:x} successfully sent the response frame.", 0x11);
    
    stopTimingFlag(lowerbits);
}

void EcuReset::stopTimingFlag(uint8_t receiver_id)
{
        switch(receiver_id)
        {
            case 0x10:
                MCU::mcu->stop_flags[0x11] = false;
                break;
            case 0x11:
                battery->stop_flags[0x11] = false;
                break;
            case 0x12:
                engine->stop_flags[0x11] = false;
                break;
            case 0x13:
                doors->stop_flags[0x11] = false;
                break;
            case 0x14:
                hvac->_ecu->stop_flags[0x11] = false;
                break;
            default:
                LOG_ERROR(ECUResetLog.GET_LOGGER(), "Module with id {:x} not supported.", receiver_id);
                break; 
        }
}
