/**
 * @file TransferData.h
 * @author Mihnea Tanasevici
 * @brief This library represents the TransferData service.
 * It is used to transfer data to a mememory location.
 * It is using the MemoryManager class that prepares the memory in Request Download service before calling TransferData.
 */
#ifndef TRANSFER_DATA_H
#define TRANSFER_DATA_H

#include <linux/can.h>
#include "../../utils/include/Logger.h"
#include "../../../utils/include/GenerateFrames.h"
#include "../../utils/include/MemoryManager.h"
#include "../../../utils/include/NegativeResponse.h"
#include "../../request_transfer_exit/include/RequestTransferExit.h"
class TransferData 
{
    public:
    static constexpr uint8_t TD_SID = 0x36;
    /**
     * @brief Constructor for transfer data object
     * 
     * @param socket The socket descriptor used for communication over the CAN bus.
     * @param transfer_data_logger local reference to the parent logger
     */
    TransferData(int socket, Logger transfer_data_logger);
    /**
     * @brief method used to facilitate the transfer of data
     * 
     * @param can_id frame id that contains the sender and receiver
     * @param transfer_request data to be transferred
     */
    void transferData(canid_t can_id, std::vector<uint8_t>& transfer_request);
    private:
    Logger transfer_data_logger;
    GenerateFrames generate_frames;
    int socket = -1;
    size_t total_size;
    size_t bytes_sent;
    static uint8_t expected_block_sequence_number;
    static bool is_first_transfer;
    /* This represents 1 transfer data size, calculated in Request Download representing the  max_number_block */
    static size_t chunk_size;
    /* Used to check if all transfers are done, this is set in Request Download*/
    static uint8_t expected_transfer_data_requests;

};






#endif