#include "../include/ReadDataByIdentifier.h"
#include "../../../ecu_simulation/BatteryModule/include/BatteryModule.h"
#include "../../../ecu_simulation/EngineModule/include/EngineModule.h"
#include "../../../ecu_simulation/DoorsModule/include/DoorsModule.h"
#include "../../../ecu_simulation/HVACModule/include/HVACModule.h"
#include "../../../mcu/include/MCUModule.h"

ReadDataByIdentifier::ReadDataByIdentifier(int socket, Logger& rdbi_logger) 
            : generate_frames(socket, rdbi_logger), rdbi_logger(rdbi_logger)
{
    this->socket = socket;
}

/* Function to handle the Read Data By Identifier request */
std::vector<uint8_t> ReadDataByIdentifier::readDataByIdentifier(canid_t frame_id, const std::vector<uint8_t>& request, bool use_send_frame)
{
    std::vector<uint8_t> response;
    NegativeResponse nrc(socket, rdbi_logger);

    /* Extract the first 8 bits of frame_id */
    uint8_t lowerbits = frame_id & 0xFF;
    uint8_t upperbits = frame_id >> 8 & 0xFF;

    /* Reverse IDs */
    canid_t can_id = ((lowerbits << 8) | upperbits);

    /* Check if the request size is less than 4 */
    if (request.size() < 4)
    {
        /* Invalid request length - prepare a negative response */
        response.push_back(0x03); /* PCI */
        response.push_back(0x7F); /* Negative response */
        response.push_back(RDBI_SERVICE_ID); /* Service ID */
        response.push_back(NegativeResponse::IMLOIF); /* Incorrect message length or invalid format */
        if (use_send_frame)
        {
            /* Send the negative response frame */ 
            nrc.sendNRC(can_id, RDBI_SERVICE_ID, NegativeResponse::IMLOIF);
            stopTimingFlag(lowerbits);
        }

        /* Return early as the request is invalid */
        return response;
    }
    if (lowerbits == 0x10 && !SecurityAccess::getMcuState(rdbi_logger))
    {
        response.push_back(0x03); /* PCI */
        response.push_back(0x7F); /* Negative response */
        response.push_back(RDBI_SERVICE_ID); /* Service ID */
        response.push_back(NegativeResponse::SAD); /* Security Access Denied */
        if (use_send_frame)
        {
            nrc.sendNRC(can_id, RDBI_SERVICE_ID, NegativeResponse::SAD);
        }
        MCU::mcu->stop_flags[0x22] = false;
        return response;
    }
    if (lowerbits == 0x11 && !ReceiveFrames::getBatteryState())
    {
        response.push_back(0x03); /* PCI */
        response.push_back(0x7F); /* Negative response */
        response.push_back(RDBI_SERVICE_ID); /* Service ID */
        response.push_back(NegativeResponse::SAD); /* Security Access Denied */
        if (use_send_frame)
        {
            nrc.sendNRC(can_id, RDBI_SERVICE_ID, NegativeResponse::SAD);
        }
        battery->stop_flags[0x22] = false;
        return response;
    }
    if (lowerbits == 0x12 && !ReceiveFrames::getEngineState())
    {
        response.push_back(0x03); /* PCI */
        response.push_back(0x7F); /* Negative response */
        response.push_back(RDBI_SERVICE_ID); /* Service ID */
        response.push_back(NegativeResponse::SAD); /* Security Access Denied */
        if (use_send_frame)
        {
            nrc.sendNRC(can_id, RDBI_SERVICE_ID, NegativeResponse::SAD);
        }
        engine->stop_flags[0x22] = false;
        return response;
    }
    if (lowerbits == 0x13 && !ReceiveFrames::getDoorsState())
    {
        response.push_back(0x03); /* PCI */
        response.push_back(0x7F); /* Negative response */
        response.push_back(RDBI_SERVICE_ID); /* Service ID */
        response.push_back(NegativeResponse::SAD); /* Security Access Denied */
        if (use_send_frame)
        {
            nrc.sendNRC(can_id, RDBI_SERVICE_ID, NegativeResponse::SAD);
        }
        doors->stop_flags[0x22] = false;
        return response;
    }
    if (lowerbits == 0x14 && !ReceiveFrames::getHvacState())
    {
        response.push_back(0x03); /* PCI */
        response.push_back(0x7F); /* Negative response */
        response.push_back(RDBI_SERVICE_ID); /* Service ID */
        response.push_back(NegativeResponse::SAD); /* Security Access Denied */
        if (use_send_frame)
        {
            nrc.sendNRC(can_id, RDBI_SERVICE_ID, NegativeResponse::SAD);
        }
        hvac->_ecu->stop_flags[0x22] = false;
        return response;
    }

    /* Extract the data identifier from the request */
    uint16_t data_identifier = (request[2] << 8) | request[3];
    std::string file_name;

    if (lowerbits == 0x10)
    {
        file_name = "mcu_data.txt";
    } else if (lowerbits == 0x11)
    {
        file_name = "battery_data.txt";
    }
    else if (lowerbits == 0x12)
    {
        file_name = "engine_data.txt";
    }
    else if (lowerbits == 0x13)
    {
        file_name = "doors_data.txt";
    }
    else if (lowerbits == 0x14)
    {
        file_name = "hvac_data.txt";
    }
    else
    {
        response.push_back(0x03); /* PCI */
        response.push_back(0x7F); /* Negative response */
        response.push_back(RDBI_SERVICE_ID); /* Service ID */
        response.push_back(NegativeResponse::ROOR); /* Request out of range */
        if (use_send_frame)
        {
            nrc.sendNRC(can_id, RDBI_SERVICE_ID, NegativeResponse::ROOR);
            stopTimingFlag(lowerbits);
        }
        return response;
    }

    try
    {
        auto data_map = FileManager::readMapFromFile(file_name);
        response = data_map[data_identifier];
    } catch (const std::exception& e)
    {
        LOG_ERROR(rdbi_logger.GET_LOGGER(), "Error reading from file: {}", e.what());
        response.push_back(0x03); /* PCI */
        response.push_back(0x7F); /* Negative response */
        response.push_back(RDBI_SERVICE_ID); /* Service ID */
        response.push_back(NegativeResponse::ROOR); /* Request out of range */
        if (use_send_frame)
        {
            nrc.sendNRC(can_id, RDBI_SERVICE_ID, NegativeResponse::ROOR);
            stopTimingFlag(lowerbits);
        }
        return response;
    }

    if (response.empty())
    {
        /* Data identifier not found */
        response.push_back(0x03); /* PCI */
        response.push_back(0x7F); /* Negative response */
        response.push_back(RDBI_SERVICE_ID); /* Service ID */
        response.push_back(NegativeResponse::ROOR); /* Request out of range */
        LOG_ERROR(rdbi_logger.GET_LOGGER(), "Error response empty");
        if (use_send_frame) {
            nrc.sendNRC(can_id, RDBI_SERVICE_ID, NegativeResponse::ROOR);
            stopTimingFlag(lowerbits);
        }
        return response;
    }

    // Convert the response vector to a string for logging
    std::ostringstream oss;
    for (const auto& byte : response)
    {
        oss << std::hex << std::setw(2) << std::setfill('0') << static_cast<int>(byte) << " ";
    }

    // Log the data
    LOG_INFO(rdbi_logger.GET_LOGGER(), "Data for DID 0x{:04X} from {}: {}", data_identifier, file_name, oss.str());


    if (use_send_frame)
    {
        if (response.size() > 7)
        {
            /* Send response frame */
            generate_frames.readDataByIdentifierLongResponse(can_id, data_identifier, response, true);
            LOG_INFO(rdbi_logger.GET_LOGGER(), "Service with SID {:x} successfully sent the response frame.", 0x22);
            stopTimingFlag(lowerbits);
        } 
        else
        {
            /* Send response frame */
            generate_frames.readDataByIdentifier(can_id, data_identifier, response);
            LOG_INFO(rdbi_logger.GET_LOGGER(), "Service with SID {:x} successfully sent the response frame.", 0x22);
            stopTimingFlag(lowerbits);
        }
    }
    return response;
}

void ReadDataByIdentifier::stopTimingFlag(uint8_t receiver_id)
{
        switch(receiver_id)
        {
            case 0x10:
                MCU::mcu->stop_flags[0x22] = false;
                break;
            case 0x11:
                battery->stop_flags[0x22] = false;
                break;
            case 0x12:
                engine->stop_flags[0x22] = false;
                break;
            case 0x13:
                doors->stop_flags[0x22] = false;
                break;
            case 0x14:
                hvac->_ecu->stop_flags[0x22] = false;
                break;
            default:
                LOG_ERROR(rdbi_logger.GET_LOGGER(), "Module with id {:x} not supported.", receiver_id);
                break; 
        }
}