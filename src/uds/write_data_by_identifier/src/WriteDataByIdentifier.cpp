#include "../include/WriteDataByIdentifier.h"
#include "../../../ecu_simulation/BatteryModule/include/BatteryModule.h"
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
    std::vector<uint8_t> data_parameter = {};
    NegativeResponse nrc(socket, wdbi_logger);

    /* Form the new id */
    int id = ((frame_id & 0xFF) << 8) | ((frame_id >> 8) & 0xFF);

    /* Checks if frame_data has the required minimum length */
    if (frame_data.size() < 3)
    {
        nrc.sendNRC(id,WDBI_SID,NegativeResponse::IMLOIF);
    }
    else if (!SecurityAccess::getMcuState())
    {
        nrc.sendNRC(id,WDBI_SID,NegativeResponse::SAD);
    }
    else
    {
        typedef uint16_t DID;
        typedef std::vector<uint8_t> DataParameter;

        /* Extract Data Identifier (DID) */
        DID did = (frame_data[2] << 8) | frame_data[3];

        /* Extract Data Parameter */
        DataParameter data_parameter(frame_data.begin() + 4, frame_data.end());
        uint8_t receiver_id = frame_id & 0xFF;

        /* List of valid DIDs */
        std::unordered_set<uint16_t> valid_dids = {
            0xF190, 0xF17F, 0xF18C, 0xF1A0, 0xF187, 0xF1A2, 0xF1A1, 0xF1A4, 
            0xF1A5, 0xF1A8, 0xF1A9, 0xF1AA, 0xF1AB, 0xF1AC, 0xF1AD, 0x0100, 
            0x010C, 0x0110, 0x0114, 0x011C, 0x0120, 0x0124, 0x012C, 0x0130, 
            0x0134, 0x0140, 0x01A0, 0x01B0, 0x01C0, 0x00D0, 0x01D0, 0x02D0,
            0x03D0, 0x04D0, 0x05D0, 0x06D0, 0x01E0, 0x01F0
        };
        auto logCollection = [&](const std::unordered_map<uint16_t, std::vector<uint8_t>>& collection, const std::string& collectionName) {
            std::ostringstream oss;
            oss << collectionName << " contents:\n";
            for (const auto& [key, value] : collection)
            {
                oss << "DID 0x" << std::hex << std::setw(4) << std::setfill('0') << key << ": ";
                for (uint8_t byte : value)
                {
                    oss << std::hex << std::setw(2) << std::setfill('0') << static_cast<int>(byte) << " ";
                }
                oss << "\n";
            }
            LOG_INFO(wdbi_logger.GET_LOGGER(), oss.str());
        };

        /* Find where to write the dataParameter */
        if(receiver_id == 0x10 && MCU::mcu->mcu_data.find(did) != MCU::mcu->mcu_data.end()) 
        {
            /* Stores the data in the memory location associated with the DID  */
            MCU::mcu->mcu_data[did] = data_parameter;
            LOG_INFO(wdbi_logger.GET_LOGGER(), "Data written to DID 0x{:x} in MCUModule.", did);
            logCollection(MCU::mcu->mcu_data, "MCUModule");
        } 
        else if (receiver_id == 0x11 && battery->ecu_data.find(did) != battery->ecu_data.end()) 
        {
            /* Stores the data in the memory location associated with the DID  */
            battery->ecu_data[did] = data_parameter;
            LOG_INFO(wdbi_logger.GET_LOGGER(), "Data written to DID 0x{:x} in BatteryModule.", did);
            logCollection(battery->ecu_data, "BatteryModule");
        } 
        else if (valid_dids.find(did) != valid_dids.end())
        {
            /* Determine the correct storage based on receiver_id */
            if (receiver_id == 0x10)
            {
                MCU::mcu->mcu_data[did] = data_parameter;
                LOG_INFO(wdbi_logger.GET_LOGGER(), "Data written to new DID 0x{:x} in MCUModule.", did);
                logCollection(MCU::mcu->mcu_data, "MCUModule");
            }
            else if (receiver_id == 0x11)
            {
                battery->ecu_data[did] = data_parameter;
                LOG_INFO(wdbi_logger.GET_LOGGER(), "Data written to new DID 0x{:x} in BatteryModule.", did);
                logCollection(battery->ecu_data, "BatteryModule");
            }
        }
        else 
        {
            LOG_ERROR(wdbi_logger.GET_LOGGER(), "Request Out Of Range: Identifier not found in memory");
            nrc.sendNRC(id,WDBI_SID,NegativeResponse::ROOR);
        }

        /* Check on which socket to send the frame */
        generate_frames.writeDataByIdentifier(id, did, {});
    }
};