#include "../include/WriteDataByIdentifier.h"
#include "../../../ecu_simulation/BatteryModule/include/BatteryModule.h"
#include "../../../ecu_simulation/EngineModule/include/EngineModule.h"
#include "../../../ecu_simulation/DoorsModule/include/DoorsModule.h"
#include "../../../ecu_simulation/HVACModule/include/HVACModule.h"
#include "../../../mcu/include/MCUModule.h"

/* Helper function to load data from a file into a map */
std::unordered_map<uint16_t, std::vector<uint8_t>> loadDataFromFile(const std::string& file_name)
{
    std::unordered_map<uint16_t, std::vector<uint8_t>> data_map;
    std::ifstream infile(file_name);
    if (!infile.is_open())
    {
        throw std::runtime_error("Failed to open file: " + file_name);
    }

    std::string line;
    while (std::getline(infile, line))
    {
        std::istringstream iss(line);
        uint16_t data_identifier;
        iss >> std::hex >> data_identifier;

        std::vector<uint8_t> data;
        int byte;
        while (iss >> std::hex >> byte)
        {
            data.push_back(static_cast<uint8_t>(byte));
        }
        data_map[data_identifier] = data;
    }

    infile.close();
    return data_map;
}

/* Helper function to save data from a map to a file */
void saveDataToFile(const std::string& file_name, const std::unordered_map<uint16_t, std::vector<uint8_t>>& data_map)
{
    /* Open in truncate mode to overwrite the file */
    std::ofstream outfile(file_name, std::ios::trunc);

    if (!outfile.is_open())
    {
        throw std::runtime_error("Failed to open file: " + file_name);
    }

    for (const auto& [data_identifier, data] : data_map)
    {
        outfile << std::hex << std::setw(4) << std::setfill('0') << std::uppercase << data_identifier << " ";
        for (uint8_t byte : data)
        {
            outfile << std::hex << std::setw(2) << std::setfill('0') << std::uppercase << static_cast<int>(byte) << " ";
        }
        outfile << "\n";
    }

    outfile.close();
}

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
        if (receiver_id == 0x10)
        {
            MCU::mcu->stop_flags[0x2E] = false;
        } else if (receiver_id == 0x11)
        {
            battery->stop_flags[0x2E] = false;
        } else if (receiver_id == 0x12)
        {
            engine->stop_flags[0x2E] = false;
        }
    }
    else if (receiver_id == 0x10 && !SecurityAccess::getMcuState(wdbi_logger))
    {
        nrc.sendNRC(id, WDBI_SID, NegativeResponse::SAD);
        MCU::mcu->stop_flags[0x2E] = false;
    }
    else if (receiver_id == 0x11 && !ReceiveFrames::getBatteryState())
    {
        nrc.sendNRC(id, WDBI_SID, NegativeResponse::SAD);
        battery->stop_flags[0x2E] = false;
    }
    else if (receiver_id == 0x12)
    {
            engine->stop_flags[0x2E] = false;
    }
    else
    {
        typedef uint16_t DID;
        typedef std::vector<uint8_t> DataParameter;

        /* Extract Data Identifier (DID) */
        DID did = (frame_data[2] << 8) | frame_data[3];
        
        // Uncomment after release
        // switch (receiver_id)
        // {
        // case 0x10:
        //     if((std::find(MCU::mcu->VALID_DID_MCU.begin(), MCU::mcu->VALID_DID_MCU.end(), did)) != MCU::mcu->VALID_DID_MCU.end())
        //     {
        //         LOG_ERROR(wdbi_logger.GET_LOGGER(), "Request Out Of Range: Identifier not found in memory");
        //         nrc.sendNRC(id,WDBI_SID,NegativeResponse::ROOR);
        //         MCU::mcu->stop_flags[0x2E] = false;
        //         return;
        //     }
        //     break;
        // case 0x11:
        //     if(battery->default_DID_battery.find(did) != battery->default_DID_battery.end())
        //     {
        //         LOG_ERROR(wdbi_logger.GET_LOGGER(), "Request Out Of Range: Identifier not found in memory");
        //         nrc.sendNRC(id,WDBI_SID,NegativeResponse::ROOR);
        //         battery->stop_flags[0x2E] = false;
        //         return;
        //     }
        //     break;
        // case 0x12:
        //     if(engine->default_DID_engine.find(did) != engine->default_DID_engine.end())
        //     {
        //         LOG_ERROR(wdbi_logger.GET_LOGGER(), "Request Out Of Range: Identifier not found in memory");
        //         nrc.sendNRC(id,WDBI_SID,NegativeResponse::ROOR);
        //         engine->stop_flags[0x2E] = false;
        //         return;
        //     }
        //     break;
        // case 0x13:
        //     if(doors->default_DID_doors.find(did) != doors->default_DID_doors.end())
        //     {
        //         LOG_ERROR(wdbi_logger.GET_LOGGER(), "Request Out Of Range: Identifier not found in memory");
        //         nrc.sendNRC(id,WDBI_SID,NegativeResponse::ROOR);
        //         doors->stop_flags[0x2E] = false;
        //         return;
        //     }
        //     break;
        // case 0x14:
        //     if(hvac->default_DID_hvac.find(did) != hvac->default_DID_hvac.end())
        //     {
        //         LOG_ERROR(wdbi_logger.GET_LOGGER(), "Request Out Of Range: Identifier not found in memory");
        //         nrc.sendNRC(id,WDBI_SID,NegativeResponse::ROOR);
        //         // Uncomment when its added the flags in HVACModule
        //         // hvac->stop_flags[0x2E] = false;
        //         return;
        //     }
        //     break;
        // default:
        //     LOG_ERROR(wdbi_logger.GET_LOGGER(), "Invalid module.");
        //     break;
        // }


        /* List of valid DIDs */
        std::unordered_set<uint16_t> valid_dids = {
            0xF190, 0xF17F, 0xF18C, 0xF1A0, 0xF187, 0xF1A2, 0xF1A1, 0xF1A4, 
            0xF1A5, 0xF1A8, 0xF1A9, 0xF1AA, 0xF1AB, 0xF1AC, 0xF1AD, 0x0100, 
            0x010C, 0x0110, 0x0114, 0x011C, 0x0120, 0x0124, 0x012C, 0x0130, 
            0x0134, 0x0140, 0x01A0, 0x01B0, 0x01C0, 0x00D0, 0x01D0, 0x02D0,
            0x03D0, 0x04D0, 0x05D0, 0x06D0, 0x01E0, 0x01F0
        };

        if (valid_dids.find(did) == valid_dids.end())
        {
            LOG_ERROR(wdbi_logger.GET_LOGGER(), "Request Out Of Range: Identifier not found in memory");
            nrc.sendNRC(id,WDBI_SID,NegativeResponse::ROOR);
            uint8_t receiver_id = frame_id & 0xFF;
            if (receiver_id == 0x10)
            {
                MCU::mcu->stop_flags[0x2E] = false;
            } else if (receiver_id == 0x11)
            {
                battery->stop_flags[0x2E] = false;
            } else if (receiver_id == 0x12)
            {
                engine->stop_flags[0x2E] = false;
            }
            return;
        }

        /* Extract Data Parameter */
        DataParameter data_parameter(frame_data.begin() + 4, frame_data.end());
        uint8_t receiver_id = frame_id & 0xFF;

        std::string file_name;
        if (receiver_id == 0x10)
        {
            file_name = "mcu_data.txt";
        } else if (receiver_id == 0x11)
        {
            file_name = "battery_data.txt";
        } else if (receiver_id == 0x12)
        {
            file_name = "engine_data.txt";
        }
        else
        {
            if (receiver_id == 0x10)
            {
                MCU::mcu->stop_flags[0x2E] = false;
            } else if (receiver_id == 0x11)
            {
                battery->stop_flags[0x2E] = false;
            } else if (receiver_id == 0x12)
            {
                engine->stop_flags[0x2E] = false;
            }
            LOG_ERROR(wdbi_logger.GET_LOGGER(), "Module with id {:x} not supported.", receiver_id);
            nrc.sendNRC(id, WDBI_SID, NegativeResponse::ROOR);
            return;
        }

        try
        {
            /* Load current data from the file */
            auto data_map = loadDataFromFile(file_name);

            // Update or add the new data for the given DID
            data_map[did] = data_parameter;

            /* Save the updated data back to the file */
            saveDataToFile(file_name, data_map);

            LOG_INFO(wdbi_logger.GET_LOGGER(), "Data written to DID 0x{:x} in {}.", did, (receiver_id == 0x10 ? "MCUModule" : "BatteryModule"));
        } catch (const std::exception& e) 
        {
            if (receiver_id == 0x10)
            {
                MCU::mcu->stop_flags[0x2E] = false;
            } else if (receiver_id == 0x11)
            {
                battery->stop_flags[0x2E] = false;
            } else if (receiver_id == 0x12)
            {
                engine->stop_flags[0x2E] = false;
            }
            LOG_ERROR(wdbi_logger.GET_LOGGER(), "Error processing file: {}", e.what());
            nrc.sendNRC(id, WDBI_SID, NegativeResponse::ROOR);
            return;
        }

        /* Send response frame */
        generate_frames.writeDataByIdentifier(id, did, {});
        LOG_INFO(wdbi_logger.GET_LOGGER(), "Service with SID {:x} successfully sent the response frame.", 0x2E);
        if (receiver_id == 0x10)
        {
            MCU::mcu->stop_flags[0x2e] = false;
        } else if (receiver_id == 0x11)
        {
            battery->stop_flags[0x2e] = false;
        } else if (receiver_id == 0x12)
        {
            engine->stop_flags[0x2E] = false;
        }
    }
};