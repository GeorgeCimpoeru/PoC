#include "../include/TransferData.h"

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
        return;
    }
    else if (expected_block_sequence_number != block_sequence_counter)
    {
        /* Wrong block sequence counter - prepare a negative response */
        nrc.sendNRC(can_id, TD_SID, NegativeResponse::WBSC);
        return;
    }
    else
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
            return;
        }

        /* Read data from the extracted binary */
        std::vector<uint8_t> data = MemoryManager::readBinary(path_to_main, transfer_data_logger);
        MemoryManager* memory_manager = MemoryManager::getInstance(transfer_data_logger);
        /* Write the read data to the partition created, at a specifig address */
        bool write_success = memory_manager->writeToAddress(data);
        if(write_success == false)
        {
            nrc.sendNRC(can_id, TD_SID, NegativeResponse::TDS);
            return;
        }
        else
        {
            /* clear vector after writing to adress */
            response.clear();
            /* prepare positive response */
            response.push_back(0x02); /* PCI */
            response.push_back(0x76); /* Service ID */
            response.push_back(block_sequence_counter); /* block_sequence_counter */
            /* Send the postive response frame */ 
            generate_frames.sendFrame(can_id, response);
            /* send request for TransferExit */
            /* generate_frames.requestTransferExit(); */

            /* Increment expected_block_sequence_number only if it matches the current block_sequence_counter */
            expected_block_sequence_number++;
            
            /* reset it to 0x01 after it reaches 0xFF and resets to 0*/
            if (expected_block_sequence_number == 0x00)
            {
                expected_block_sequence_number = 0x01;
            }
        }
    }
}