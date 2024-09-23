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

/* method used to transfer the data */
/* frame format = {PCI_L, SID(0x36), block_sequence_counter, transfer_request_parameter_record}*/
void TransferData::transferData(canid_t can_id, std::vector<uint8_t>& transfer_request)
{
    /* Declaration of data as static in order to retain its value between calls */
    static std::vector<uint8_t> data;
    /* Total size of data */
    static size_t total_size = 0;
    /* Keep track of how many bytes have been tracked */
    static size_t bytes_sent = 0;
    MCU::mcu->setDidValue(OTA_UPDATE_STATUS_DID, {PROCESSING});
    NegativeResponse nrc(socket, transfer_data_logger);
    /* Define chunk_size as 1MB chunk */
    size_t chunk_size = 0x100000;
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
        return;
    }
    else if (expected_block_sequence_number != block_sequence_counter)
    {
        /* Wrong block sequence counter - prepare a negative response */
        nrc.sendNRC(can_id, TD_SID, NegativeResponse::WBSC);
        MCU::mcu->setDidValue(OTA_UPDATE_STATUS_DID, {PROCESSING_TRANSFER_FAILED});
        return;
    }
    else
    {
        /* Load data if this is the first transfer */
        static bool is_first_transfer = true;
        if (is_first_transfer)
        {
            /* use memory manager class to transfer the data */
            std::string path_to_main;
            if (access((std::string(PROJECT_PATH) + "/main_mcu_new").c_str(), F_OK) == 0 && target_id == 0x10) {
                path_to_main = std::string(PROJECT_PATH) + "/main_mcu_new";
            }
            else if (access((std::string(PROJECT_PATH) + "/main_battery_new").c_str(), F_OK) == 0 && target_id == 0x11) {
                path_to_main = std::string(PROJECT_PATH) + "/main_battery_new";
            }
            else if (access((std::string(PROJECT_PATH) + "/main_doors_new").c_str(), F_OK) == 0 && target_id == 0x12) {
                path_to_main = std::string(PROJECT_PATH) + "/main_doors_new";
            }
            else if (access((std::string(PROJECT_PATH) + "/main_engine_new").c_str(), F_OK) == 0 && target_id == 0x13) {
                path_to_main = std::string(PROJECT_PATH) + "/main_engine_new";
            }
            else if (access((std::string(PROJECT_PATH) + "/main_hvac_new").c_str(), F_OK) == 0 && target_id == 0x14) {
                path_to_main = std::string(PROJECT_PATH) + "/main_hvac_new";
            }
            else
            {
                nrc.sendNRC(can_id, TD_SID, NegativeResponse::TDS);
                MCU::mcu->setDidValue(OTA_UPDATE_STATUS_DID, {PROCESSING_TRANSFER_FAILED});
                return;
            }

            /* Read data from the extracted binary */
            data = MemoryManager::readBinary(path_to_main, transfer_data_logger);
            
            /* Set the total size of the binary data */
            total_size = data.size();
            std::cout << "totalllllllllllll" << total_size << "\n";
            /* Initialize the bytes sent for the first transfer */
            bytes_sent = 0;
            /* Set the flag as false after the first transfer */
            is_first_transfer = false;                  
        }

        /* Check whether there is still data to transfer */
        if (bytes_sent < total_size)
        {        
            /* Check the remaining bytes */
            size_t remaining_bytes = total_size - bytes_sent;
            /* Compute the current chunk size */
            size_t current_chunk_size = std::min(chunk_size, remaining_bytes);
            std::cout << "remaining bytes are " << remaining_bytes << " currentchunk size " << current_chunk_size << " chunk siez " << chunk_size << "\n"; 
            /* Extract the current chunk from the binary data */
            std::vector<uint8_t> chunk_data(data.begin() + bytes_sent, data.begin() + bytes_sent + current_chunk_size);                

            MemoryManager* memory_manager = MemoryManager::getInstance(transfer_data_logger);   
            /* Write the read data to the partition created, at a specifig address */
            bool write_success = memory_manager->writeToAddress(chunk_data);

            if(write_success == false)
            {
                nrc.sendNRC(can_id, TD_SID, NegativeResponse::TDS);
                MCU::mcu->setDidValue(OTA_UPDATE_STATUS_DID, {PROCESSING_TRANSFER_FAILED});
                return;
            }
            else
            {
                /* Update bytes sent */
                bytes_sent += current_chunk_size;
                std::cout << "bytes sent    " << bytes_sent << "\n";

                /* clear vector after writing to adress */
                response.clear();
                /* prepare positive response */
                response.push_back(0x02); /* PCI */
                response.push_back(0x76); /* Service ID */
                response.push_back(block_sequence_counter); /* block_sequence_counter */

                /* Send the postive response frame */
                generate_frames.sendFrame(can_id, response);

                /* Increment block sequence counter, wrapping to 0x01 after 0xFF */
                block_sequence_counter = (block_sequence_counter + 1) & 0xFF;
                /* Increment expected_block_sequence_number only if it matches the current block_sequence_counter */
                expected_block_sequence_number++;
            }
        }

        /* Check if all data has been sent */
        if (bytes_sent >= total_size)
        {
            MCU::mcu->setDidValue(OTA_UPDATE_STATUS_DID, {PROCESSING_TRANSFER_COMPLETE});
            /* Reset the flag for the next transfer */
            is_first_transfer = true;
        }

        /* reset it to 0x01 after it reaches 0xFF and resets to 0*/
        if (expected_block_sequence_number == 0x00)
        {
            expected_block_sequence_number = 0x01;
        }
    }
}
