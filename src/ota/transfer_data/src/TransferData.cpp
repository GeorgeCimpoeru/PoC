#include "../include/TransferData.h"
#include "../../../src/mcu/include/MCUModule.h"

TransferData::TransferData(int socket, Logger transfer_data_logger)
                : transfer_data_logger(transfer_data_logger), 
                  generate_frames(socket, transfer_data_logger)

{
    this->socket = socket;
}

/* Definition and initialization of the static member variable */
uint8_t TransferData::expected_block_sequence_number = 0x01;  /* Start from 1 */
bool TransferData::is_first_transfer = false;
size_t TransferData::chunk_size = 0;
uint8_t TransferData::expected_transfer_data_requests = 0;
/* Static vector to store the checksums */
std::vector<uint8_t>TransferData::checksums;

MemoryManager* TransferData::memory_manager = nullptr;
/* Method to compute a simple XOR checksum for a block of data */
uint8_t TransferData::computeChecksum(const uint8_t* data, size_t block_size)
{
    uint8_t checksum = 0;
    for (size_t i = 0; i < block_size; ++i)
    {
        checksum ^= data[i];
    }
    /* Return the checksum */
    return checksum;
}

/* Retrieve the checksum vector */
const std::vector<uint8_t>&TransferData::getChecksums()
{
    return checksums;
}

void TransferData::processBinaryDataForTransfer(uint8_t receiver_id, std::vector<uint8_t>& current_data, Logger& logger)
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
        std::string path_to_main;
        if(FileManager::getEcuPath(receiver_id, path_to_main, 3, logger) == 0)
        {
            // nrc.sendNRC(can_id, TD_SID, NegativeResponse::TDS);
            MCU::mcu->setDidValue(OTA_UPDATE_STATUS_DID, {PROCESSING_TRANSFER_FAILED});
            return;
        }

        /* Read data from the extracted binary */
        data = MemoryManager::readBinary(path_to_main, logger);
        
        /* Get chunk_size from request download */
        chunk_size = 0x05; //static_cast<size_t>(RequestDownloadService::getMaxNumberBlock());
        /* Set the total size of the binary data */
        total_size = data.size();
        /* Initialize the bytes sent for the first transfer */
        bytes_sent = 0;
    }

    size_t current_chunk_size = std::min(static_cast<size_t>(chunk_size), total_size - bytes_sent);
    current_data.insert(current_data.end(), data.begin() + bytes_sent, data.begin() + bytes_sent + current_chunk_size);
    current_data[0] = static_cast<uint8_t>(current_data.size() - 1);
    bytes_sent += current_chunk_size;

    if (bytes_sent >= total_size)
    {
        MCU::mcu->setDidValue(OTA_UPDATE_STATUS_DID, {PROCESSING_TRANSFER_COMPLETE});
    }
    /* Display progress, speed, and remaining time */
    std::cout << "\rProgress: " << static_cast<int>((static_cast<double>(bytes_sent) / total_size) * 100) << "% "
                << "Sent: " << bytes_sent << " / " << total_size << " "
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

    if (expected_block_sequence_number != block_sequence_counter)
    {
        /* Wrong block sequence counter - prepare a negative response */
        nrc.sendNRC(can_id, TD_SID, NegativeResponse::WBSC);
        AccessTimingParameter::stopTimingFlag(receiver_id, TRANSFER_DATA_SID);
        return;
    }
    OtaUpdateStatesEnum ota_state = static_cast<OtaUpdateStatesEnum>(MCU::mcu->getDidValue(OTA_UPDATE_STATUS_DID)[0]);
    if(ota_state == WAIT_DOWNLOAD_COMPLETED)
    {
        is_first_transfer = true;
        /* Get chunk_size from request download */
        chunk_size = 5; //static_cast<size_t>(RequestDownloadService::getMaxNumberBlock());
        /* set expected transfer data requests that is defined in request download */
        expected_transfer_data_requests = MAX_TRANSFER_DATA_REQUESTS;
        expected_block_sequence_number = 1;
        TransferData::memory_manager = MemoryManager::getInstance(DEV_LOOP_PARTITION_1_ADDRESS, DEV_LOOP, transfer_data_logger);                

        MCU::mcu->setDidValue(OTA_UPDATE_STATUS_DID, {PROCESSING});
        ota_state = static_cast<OtaUpdateStatesEnum>(MCU::mcu->getDidValue(OTA_UPDATE_STATUS_DID)[0]);
    }

    if(ota_state != PROCESSING && ota_state != PROCESSING_TRANSFER_COMPLETE)
    {
        LOG_WARN(transfer_data_logger.GET_LOGGER(), "Data transfer is not initialized. Use Request Download in order to initialize a data transfer. Current OTA state:{}", ota_state);
        nrc.sendNRC(can_id, TD_SID, NegativeResponse::CNC);
        AccessTimingParameter::stopTimingFlag(receiver_id, TRANSFER_DATA_SID);
        return;
    }

    /* Write the chunk to the vector */
    data.insert(data.end(), transfer_request.begin() + 3, transfer_request.end());
    
    if(ota_state == PROCESSING_TRANSFER_COMPLETE)
    {
        bool write_success = memory_manager->writeToAddress(data);
        if(write_success == false)
        {
            nrc.sendNRC(can_id, TD_SID, NegativeResponse::TDS);
            MCU::mcu->setDidValue(OTA_UPDATE_STATUS_DID, {PROCESSING_TRANSFER_FAILED});
            AccessTimingParameter::stopTimingFlag(receiver_id, TRANSFER_DATA_SID);
        }
        else
        {   
            /* Status remains PROCESSING_TRANSFER_COMPLETE */
            response.clear();
            /* prepare positive response */
            response.push_back(0x02); /* PCI */
            response.push_back(0x76); /* Service ID */
            response.push_back(block_sequence_counter); /* block_sequence_counter */
            /* TODO : this can be replaced with meaniningfull informations about the overall transfer*/
            response.insert(response.end(), transfer_request.begin() + 3, transfer_request.end()); /* transfer parameter record */

            /* Send the postive response frame */
            generate_frames.sendFrame(can_id, response);
            AccessTimingParameter::stopTimingFlag(receiver_id, TRANSFER_DATA_SID);
        }
        return;
    }
    /* Increment expected_block_sequence_number only if it matches the current block_sequence_counter */
    expected_block_sequence_number++;

    /* reset it to 0x01 after it reaches 0xFF and resets to 0*/
    if (expected_block_sequence_number == 0x00)
    {
        expected_block_sequence_number = 0x01;
    }
}