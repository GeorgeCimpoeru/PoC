#include "src/ota/transfer_data/include/TransferData.h"

TransferData::TransferData(int socket, Logger& transfer_data_logger)
                : transfer_data_logger(transfer_data_logger), 
                  generate_frames(socket, transfer_data_logger),
                  memory_manager(MemoryManager::getInstance())
{
    this->socket = socket;
}

/* Definition and initialization of the static member variable */
uint8_t TransferData::expected_block_sequence_number = 0x01;  /* Start from 1 */

/* method used to transfer the data */
/* frame format = {PCI_L, SID(0x36), block_sequence_counter, transfer_request_parameter_record}*/
void TransferData::transferData(canid_t can_id, std::vector<uint8_t>& transfer_request)
{
    uint8_t block_sequence_counter = transfer_request[2];
    std::vector<uint8_t> response;
    if (transfer_request.size() < 4)
    {
        /* Incorrect message length or invalid format - prepare a negative response */
        response.push_back(0x03); /* PCI */
        response.push_back(0x7F); /* Negative response */
        response.push_back(0x76); /* Service ID */
        response.push_back(0x13); /* Incorrect message length or invalid format */
        /* Send the negative response frame */ 
        generate_frames.sendFrame(can_id, response);
        return;
    }
    else if (expected_block_sequence_number != block_sequence_counter)
    {
        /* Wrong block sequence counter - prepare a negative response */
        response.push_back(0x03); /* PCI */
        response.push_back(0x7F); /* Negative response */
        response.push_back(0x76); /* Service ID */
        response.push_back(0x73); /* Wrong block sequence counter */
        /* Send the negative response frame */ 
        generate_frames.sendFrame(can_id, response);
        return;
    }
    else
    {
        response.insert(response.end(),transfer_request.begin() + 4, transfer_request.end());
        /* use memory manager class to transfer the data */
        memory_manager->writeToAddress(response);
        /* clear vector after writing to adress */
        response.clear();
        /* prepare positive response */
        response.push_back(0x03); /* PCI */
        response.push_back(0x36); /* Service ID */
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