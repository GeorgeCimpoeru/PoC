#include "../include/WriteDataByIdentifier.h"
#include "../../../ecu_simulation/BatteryModule/include/BatteryModule.h"
#include "../../../ecu_simulation/EngineModule/include/EngineModule.h"
#include "../../../ecu_simulation/DoorsModule/include/DoorsModule.h"
#include "../../../ecu_simulation/HVACModule/include/HVACModule.h"
#include "../../../ecu_simulation/DoorsModule/include/DoorsModule.h"
#include "../../../ecu_simulation/HVACModule/include/HVACModule.h"
#include "../../../mcu/include/MCUModule.h"

WriteDataByIdentifier::WriteDataByIdentifier(Logger& wdbi_logger, int socket)
            : generate_frames(socket, wdbi_logger), wdbi_logger(wdbi_logger)
{   
    this->socket = socket;
}

WriteDataByIdentifier::~WriteDataByIdentifier()
{
};

void WriteDataByIdentifier::WriteDataByIdentifierService(canid_t frame_id, std::vector<uint8_t> frame_data)
{
    LOG_INFO(wdbi_logger.GET_LOGGER(), "Write Data By Identifier Service invoked.");

    std::vector<uint8_t> response;
    NegativeResponse nrc(socket, wdbi_logger);

    /* Form the new id */
    int id = ((frame_id & 0xFF) << 8) | ((frame_id >> 8) & 0xFF);
    uint8_t receiver_id = frame_id & 0xFF;

    /* Checks if frame_data has the required minimum length */
    if (frame_data.size() < 3)
    {
        nrc.sendNRC(id, WDBI_SID, NegativeResponse::IMLOIF);
        uint8_t receiver_id = frame_id & 0xFF;
        AccessTimingParameter::stopTimingFlag(receiver_id, 0x2E);
    }
    else if (receiver_id == 0x10 && !SecurityAccess::getMcuState(wdbi_logger))
    {
        nrc.sendNRC(id, WDBI_SID, NegativeResponse::SAD);
        MCU::mcu->stop_flags[0x2E] = false;
    }
    else if (receiver_id == 0x11 && !ReceiveFrames::getBatteryState())
    {
        nrc.sendNRC(id, WDBI_SID, NegativeResponse::SAD);
        battery->_ecu->stop_flags[0x2E] = false;
    }
    else if (receiver_id == 0x12 && !ReceiveFrames::getEngineState())
    {
        nrc.sendNRC(id, WDBI_SID, NegativeResponse::SAD);
        engine->_ecu->stop_flags[0x2E] = false;
    }
    else if (receiver_id == 0x13 && !ReceiveFrames::getDoorsState())
    {
        nrc.sendNRC(id, WDBI_SID, NegativeResponse::SAD);
        doors->_ecu->stop_flags[0x2E] = false;
    }
    else if (receiver_id == 0x14 && !ReceiveFrames::getHvacState())
    {
        nrc.sendNRC(id, WDBI_SID, NegativeResponse::SAD);
        hvac->_ecu->stop_flags[0x2E] = false;
    }
    else
    {
        typedef uint16_t DID;
        typedef std::vector<uint8_t> DataParameter;

        /* Extract Data Identifier (DID) */
        DID did = (frame_data[2] << 8) | frame_data[3];
        switch (receiver_id)
        {
            case 0x10:
                if((std::find(MCU::mcu->VALID_DID_MCU.begin(), MCU::mcu->VALID_DID_MCU.end(), did)) == MCU::mcu->VALID_DID_MCU.end())
                {
                    LOG_ERROR(wdbi_logger.GET_LOGGER(), "Request Out Of Range: Identifier not found in memory");
                    nrc.sendNRC(id,WDBI_SID,NegativeResponse::ROOR);
                    MCU::mcu->stop_flags[0x2E] = false;
                    return;
                }
                break;
            case 0x11:
                if(battery->default_DID_battery.find(did) == battery->default_DID_battery.end())
                {
                    LOG_ERROR(wdbi_logger.GET_LOGGER(), "Request Out Of Range: Identifier not found in memory");
                    nrc.sendNRC(id,WDBI_SID,NegativeResponse::ROOR);
                    battery->_ecu->stop_flags[0x2E] = false;
                    return;
                }
                break;
            case 0x12:
                if(engine->default_DID_engine.find(did) == engine->default_DID_engine.end())
                {
                    LOG_ERROR(wdbi_logger.GET_LOGGER(), "Request Out Of Range: Identifier not found in memory");
                    nrc.sendNRC(id,WDBI_SID,NegativeResponse::ROOR);
                    engine->_ecu->stop_flags[0x2E] = false;
                    return;
                }
                break;
            case 0x13:
                if(doors->default_DID_doors.find(did) == doors->default_DID_doors.end())
                {
                    LOG_ERROR(wdbi_logger.GET_LOGGER(), "Request Out Of Range: Identifier not found in memory");
                    nrc.sendNRC(id,WDBI_SID,NegativeResponse::ROOR);
                    doors->_ecu->stop_flags[0x2E] = false;
                    return;
                }
                break;
            case 0x14:
                if(hvac->default_DID_hvac.find(did) == hvac->default_DID_hvac.end())
                {
                    LOG_ERROR(wdbi_logger.GET_LOGGER(), "Request Out Of Range: Identifier not found in memory");
                    nrc.sendNRC(id,WDBI_SID,NegativeResponse::ROOR);
                    hvac->_ecu->stop_flags[0x2E] = false;
                    return;
                }
                break;
            default:
                LOG_ERROR(wdbi_logger.GET_LOGGER(), "Invalid module.");
                break;
        }

        /* Extract Data Parameter */
        DataParameter data_parameter(frame_data.begin() + 4, frame_data.end());
        uint8_t receiver_id = frame_id & 0xFF;

        std::string file_name;
        if (receiver_id == 0x10)
        {
            file_name = "mcu_data.txt";
        }
        else if (receiver_id == 0x11)
        {
            file_name = "battery_data.txt";
        }
        else if (receiver_id == 0x12)
        {
            file_name = "engine_data.txt";
        }
        else if (receiver_id == 0x13)
        {
            file_name = "doors_data.txt";
        }
        else if (receiver_id == 0x14)
        {
            file_name = "hvac_data.txt";
        }
        else
        {
            LOG_ERROR(wdbi_logger.GET_LOGGER(), "Module with id {:x} not supported.", receiver_id);
            nrc.sendNRC(id, WDBI_SID, NegativeResponse::ROOR);
            AccessTimingParameter::stopTimingFlag(receiver_id, 0x2E);
            return;
        }

        try
        {
            /* Load current data from the file */
            auto data_map = FileManager::readMapFromFile(file_name);

            // Update or add the new data for the given DID
            data_map[did] = data_parameter;

            /* Save the updated data back to the file */
            FileManager::writeMapToFile(file_name, data_map);

            LOG_INFO(wdbi_logger.GET_LOGGER(), "Data written to DID 0x{:x} in the module with id {}.", did, receiver_id);
        } catch (const std::exception& e) 
        {
            LOG_ERROR(wdbi_logger.GET_LOGGER(), "Error processing file: {}", e.what());
            nrc.sendNRC(id, WDBI_SID, NegativeResponse::ROOR);
            AccessTimingParameter::stopTimingFlag(receiver_id, 0x2E);
            return;
        }

        /* Send response frame */
        generate_frames.writeDataByIdentifier(id, did, {});
        LOG_INFO(wdbi_logger.GET_LOGGER(), "Service with SID {:x} successfully sent the response frame.", 0x2E);
        AccessTimingParameter::stopTimingFlag(receiver_id, 0x2E);
    }
};