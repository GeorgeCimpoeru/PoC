#include "../include/TransferData.h"
#include "../../../src/mcu/include/MCUModule.h"

TransferData::TransferData(int socket, Logger transfer_data_logger)
                : transfer_data_logger(transfer_data_logger), 
                  generate_frames(socket, transfer_data_logger)

{
    this->socket = socket;
}

/* Definition and initialization of the static member variable */
MemoryManager* TransferData::memory_manager = nullptr;
uint8_t TransferData::expected_block_sequence_number = 0x01;  /* Start from 1 */
size_t TransferData::chunk_size = 0;
/* Static vector to store the checksums */
std::vector<uint8_t>TransferData::checksums;

/* Method to compute a simple XOR checksum for a block of data */
uint8_t TransferData::computeChecksum(const uint8_t* data, size_t block_size)
{
    uint8_t checksum = 0;
    for (size_t i = 0; i < block_size; ++i)
    {
        checksum ^= *(data + i);
    }
    /* Return the checksum */
    return checksum;
}

/* Retrieve the checksum vector */
const std::vector<uint8_t>&TransferData::getChecksums()
{
    return checksums;
}

void TransferData::processDataForTransfer(uint8_t receiver_id, std::vector<uint8_t>& current_data, Logger& logger)
{
    static std::vector<uint8_t> data = {};  
    /* Total size of data */
    static size_t total_size = 0;
    /* Total bytes sent */
    static size_t bytes_sent = 0;
    /* Data size of 1 transfer data */
    static uint8_t chunk_size = 0;

    OtaUpdateStatesEnum ota_state = static_cast<OtaUpdateStatesEnum>(MCU::mcu->getDidValue(OTA_UPDATE_STATUS_DID)[0]);

    if(ota_state == WAIT_DOWNLOAD_COMPLETED)
    {
        /* Get chunk_size from request download */
        chunk_size = 0x05;
        /* Initialize the bytes sent */
        bytes_sent = 0;

        std::string path_to_main;
        if(FileManager::getEcuPath(receiver_id, path_to_main, 3, logger) == 0)
        {
            MCU::mcu->setDidValue(OTA_UPDATE_STATUS_DID, {PROCESSING_TRANSFER_FAILED});
            return;
        }

        /* Read data from the extracted binary */
        TransferData::checksums.clear();
        data.clear();
        data = MemoryManager::readBinary(path_to_main, logger);
        total_size = data.size();

        /* Determine how many bytes are needed to represent the size */
        std::vector<uint8_t>binary_data_size_bytes;                
        uint8_t byte;
        bool first_byte_found = false;

        for(int i = sizeof(total_size) - 1; i >=0; --i)
        {
            byte = (total_size >> (i * 8)) & 0xFF;
            if(byte != 0 || first_byte_found == true)
            {
                binary_data_size_bytes.emplace_back(byte);
                first_byte_found = true;
            }
        }

        /* Write the size of the size field as the first byte(number of bytes to represent the size) */
        current_data.emplace_back(binary_data_size_bytes.size());

        /* Add the actual size bytes
            PCI_1 + SID_1 + BL_IDX_1 + SIZE_FORMAT_1 + SIZE_1 + SIZE_2 + SIZE_3 + how many bytes are needed for the size
         */
        current_data.insert(current_data.end(), binary_data_size_bytes.begin(), binary_data_size_bytes.end());

        LOG_INFO(logger.GET_LOGGER(), "Data processing for transfer started.");
        return;
    }
    
    size_t current_chunk_size = std::min(static_cast<size_t>(chunk_size), total_size - bytes_sent);
    if (bytes_sent >= total_size)
    {   
        current_data.emplace_back(TransferData::computeChecksum(TransferData::checksums.data(), TransferData::checksums.size()));
        MCU::mcu->setDidValue(OTA_UPDATE_STATUS_DID, {PROCESSING_TRANSFER_COMPLETE});
    }
    else
    {
        current_data.insert(current_data.end(), data.begin() + bytes_sent, data.begin() + bytes_sent + current_chunk_size);
        TransferData::checksums.emplace_back(TransferData::computeChecksum(data.data() + bytes_sent, current_chunk_size));
        bytes_sent += current_chunk_size;
    }
    current_data[0] = static_cast<uint8_t>(current_data.size() - 1);
    /* Display progress */
    std::cout << "\rProgress: " << static_cast<int>((static_cast<double>(bytes_sent) / total_size) * 100) << "% "
                << "Sent: " << bytes_sent << " / " << total_size
                << std::flush;
}

/* method used to transfer the data */
/* frame format = {PCI_L, SID(0x36), block_sequence_counter, transfer_request_parameter_record}*/
void TransferData::transferData(canid_t can_id, std::vector<uint8_t>& transfer_request)
{
    /* Declaration of data as static in order to retain its value between calls */
    static std::vector<uint8_t> data;
    NegativeResponse nrc(socket, transfer_data_logger);
    uint8_t block_sequence_counter = transfer_request[2];
    std::vector<uint8_t> response;
    /* Extract and switch sender and receiver */
    uint8_t receiver_id = can_id  & 0xFF;
    uint8_t sender_id = (can_id >> 8) & 0xFF;
    /* target id represents the ecu needed to be updated, if 0 then target will be mcu */
    /* it is 1 byte and can be found between bit 16 and 23 in canid_t */
    uint8_t target_id = (can_id >> 16) & 0xFF;
    /* Reverse IDs, target id will be in same position but receiver will be switched with sender */
    can_id = (target_id << 16) | (receiver_id << 8) | sender_id;
    if (transfer_request.size() < 3)
    {
        /* Incorrect message length or invalid format - prepare a negative response */
        nrc.sendNRC(can_id, TD_SID, NegativeResponse::IMLOIF);
        MCU::mcu->setDidValue(OTA_UPDATE_STATUS_DID, {PROCESSING_TRANSFER_FAILED});
        AccessTimingParameter::stopTimingFlag(receiver_id, TRANSFER_DATA_SID);
        return;
    }

    OtaUpdateStatesEnum ota_state = static_cast<OtaUpdateStatesEnum>(MCU::mcu->getDidValue(OTA_UPDATE_STATUS_DID)[0]);
    if(ota_state == WAIT_DOWNLOAD_COMPLETED)
    {
        /* Clear old data */
        data.clear();
        /* Request Download informations */
        RDSData rds_data = RequestDownloadService::getRdsData();
        /* Get chunk_size from request download */
        chunk_size = rds_data.max_number_block;
        /* set expected transfer data requests that is defined in request download */
        expected_block_sequence_number = 1;
        TransferData::memory_manager = MemoryManager::getInstance(transfer_data_logger);
        /* Address and path should have been initialized from Request Download. If they are not, initialisation is done here.*/            
        if(memory_manager->getAddress() != rds_data.address || memory_manager->getPath() != DEV_LOOP)
        {
            LOG_WARN(transfer_data_logger.GET_LOGGER(), "Transfer Data initialized without setting address and path in Request Download. Initialization done in Transfer Data.");
            memory_manager->setAddress(rds_data.address);
            memory_manager->setPath(DEV_LOOP);
        }
        memory_write_status = false;

        MCU::mcu->setDidValue(OTA_UPDATE_STATUS_DID, {PROCESSING});
        ota_state = static_cast<OtaUpdateStatesEnum>(MCU::mcu->getDidValue(OTA_UPDATE_STATUS_DID)[0]);

        LOG_INFO(transfer_data_logger.GET_LOGGER(), "Data transfer started.");
    }


    if(ota_state != PROCESSING && ota_state != PROCESSING_TRANSFER_COMPLETE)
    {
        LOG_WARN(transfer_data_logger.GET_LOGGER(), "Data transfer is not initialized. Use Request Download in order to initialize a data transfer. Current OTA state:{}", ota_state);
        nrc.sendNRC(can_id, TD_SID, NegativeResponse::CNC);
        AccessTimingParameter::stopTimingFlag(receiver_id, TRANSFER_DATA_SID);
        return;
    }

    if (expected_block_sequence_number != block_sequence_counter)
    {
        /* Wrong block sequence counter - prepare a negative response */
        nrc.sendNRC(can_id, TD_SID, NegativeResponse::WBSC);
        AccessTimingParameter::stopTimingFlag(receiver_id, TRANSFER_DATA_SID);
        return;
    }

    /* Write the chunk to the vector */
    data.insert(data.end(), transfer_request.begin() + 3, transfer_request.end());
    /* Display bytes received */
    std::cout << "\rBytes received: " << data.size()
                << std::flush;
    if(ota_state == PROCESSING_TRANSFER_COMPLETE)
    {
        if(memory_write_status == false)
        {
            memory_write_status = memory_manager->writeToAddress(data);
            if(memory_write_status == false)
            {
                nrc.sendNRC(can_id, TD_SID, NegativeResponse::TDS);
                MCU::mcu->setDidValue(OTA_UPDATE_STATUS_DID, {PROCESSING_TRANSFER_FAILED});
                AccessTimingParameter::stopTimingFlag(receiver_id, TRANSFER_DATA_SID);
                LOG_INFO(transfer_data_logger.GET_LOGGER(), "Data transfer failed at writting in memory.");
                return;
            }
            /* Status remains PROCESSING_TRANSFER_COMPLETE */
            response.clear();
            /* prepare positive response */
            response.push_back(0x03); /* PCI */
            response.push_back(0x76); /* Service ID */
            response.push_back(block_sequence_counter); /* block_sequence_counter */
            response.emplace_back(static_cast<uint8_t>(ota_state));
            /* Send the postive response frame */
            generate_frames.sendFrame(can_id, response);
            AccessTimingParameter::stopTimingFlag(receiver_id, TRANSFER_DATA_SID);
            LOG_INFO(transfer_data_logger.GET_LOGGER(), "Data transfer complete.");
        }
        return;
    }
    /* Continue */
    response.clear();
    /* prepare positive response */
    response.push_back(0x03); /* PCI */
    response.push_back(0x76); /* Service ID */
    response.push_back(block_sequence_counter); /* block_sequence_counter */
    response.emplace_back(static_cast<uint8_t>(ota_state));
    /* Send the postive response frame */
    generate_frames.sendFrame(can_id, response);
    AccessTimingParameter::stopTimingFlag(receiver_id, TRANSFER_DATA_SID);

    /* Increment expected_block_sequence_number only if it matches the current block_sequence_counter */
    expected_block_sequence_number++;
    /* reset it to 0x01 after it reaches 0xFF and resets to 0*/
    if (expected_block_sequence_number == 0x00)
    {
        expected_block_sequence_number = 0x01;
    }
}