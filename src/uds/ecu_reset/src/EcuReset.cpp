#include "../include/EcuReset.h"
#include "../../../mcu/include/MCUModule.h"
#include "../../../ecu_simulation/BatteryModule/include/BatteryModule.h"
#include "../../../ecu_simulation/EngineModule/include/EngineModule.h"

EcuReset::EcuReset(uint32_t can_id, uint8_t sub_function, int socket, Logger &logger)
    : can_id(can_id), sub_function(sub_function), socket(socket), ECUResetLog(logger)
{
}

EcuReset::~EcuReset()
{
}

void EcuReset::ecuResetRequest()
{
    uint8_t lowerbits = can_id & 0xFF;
    uint8_t upperbits = can_id >> 8 & 0xFF;
    uint32_t new_id = (upperbits << 8) | lowerbits;

    NegativeResponse nrc(socket, ECUResetLog);

    if (sub_function != 0x01 && sub_function != 0x02)
    {
        nrc.sendNRC(new_id,0x11,NegativeResponse::SFNS);
        if (lowerbits == 0x10)
        {
            MCU::mcu->stop_flags[0x11] = false;
        } else if (lowerbits == 0x11)
        {
            battery->stop_flags[0x11] = false;
        } else if (lowerbits == 0x12)
        {
            engine->stop_flags[0x11] = false;
        }
    }
    /* Hard Reset case */
    else if (sub_function == 0x01) {
        LOG_INFO(ECUResetLog.GET_LOGGER(), "Reset Mode: Hard Reset");
        this->hardReset();
    /* Keys off Reset case */
    } else if (sub_function == 0x02) {
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
            system("./../autoscripts/mcu_reset_hard.sh");            
            break;
        case 0x11:
            system("./../autoscripts/battery_reset_hard.sh");            
            break;
        case 0x12:
            system("./../autoscripts/engine_reset_hard.sh");            
            break;
        case 0x13:
            system("./../autoscripts/doors_reset_hard.sh");            
            break;
        case 0x14:
            system("./../autoscripts/hvac_reset_hard.sh");            
            break;
        default:
            LOG_ERROR(ECUResetLog.GET_LOGGER(), "ECU doesn't exist in hardReset");
    }
}

void EcuReset::keyOffReset()
{
    uint8_t lowerbits = can_id & 0xFF;
    /* Send response */
    this->ecuResetResponse();

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
        LOG_ERROR(ECUResetLog.GET_LOGGER(), "ECU doesn't exist in keyOffReset");
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

    LOG_INFO(ECUResetLog.GET_LOGGER(), "Contents of the file:\n {}", original_file_contents);
    
    /* Reset the program */
    hardReset();
    std::ofstream outfile(file_path, std::ios::out | std::ios::trunc);
    if (!outfile.is_open()) {
        LOG_ERROR(ECUResetLog.GET_LOGGER(), "Failed to open the file for writing: {}", file_path);
        return;
    }
    /* Write back the original content to the file */
    outfile << original_file_contents;
    outfile.close();
}

void EcuReset::ecuResetResponse()
{
    /* Generate the response frame and send it */
    GenerateFrames generate_frames(socket, ECUResetLog);

    uint8_t lowerbits = can_id & 0xFF;
    uint8_t upperbits = can_id >> 8 & 0xFF;
    
    uint32_t new_id = (upperbits << 8) | lowerbits;

    switch(lowerbits)
    {
        case 0x10:
            /* Send response frame */
            generate_frames.ecuReset(new_id, sub_function, socket, true);
            LOG_INFO(ECUResetLog.GET_LOGGER(), "Service with SID {:x} successfully sent the response frame.", 0x11);
            MCU::mcu->stop_flags[0x11] = false;
            break;
        case 0x11:
            /* Send response frame */
            generate_frames.ecuReset(new_id, sub_function, socket, true);
            LOG_INFO(ECUResetLog.GET_LOGGER(), "Service with SID {:x} successfully sent the response frame.", 0x11);
            battery->stop_flags[0x11] = false;
            break;
        case 0x12:
            /* Send response frame */
            generate_frames.ecuReset(new_id, sub_function, socket, true);
            LOG_INFO(ECUResetLog.GET_LOGGER(), "Service with SID {:x} successfully sent the response frame.", 0x11);
            engine->stop_flags[0x11] = false;
            break;
        default:
            LOG_ERROR(ECUResetLog.GET_LOGGER(), "Module with id {:x} not supported.", lowerbits);
    }
}
