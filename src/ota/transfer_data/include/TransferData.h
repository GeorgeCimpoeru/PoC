#ifndef TRANSFER_DATA_H
#define TRANSFER_DATA_H

#include <linux/can.h>
#include "../../utils/include/Logger.h"
#include "../../../utils/include/GenerateFrames.h"
#include "../../../utils/include/MemoryManager.h"
#include "../../request_transfer_exit/include/RequestTransferExit.h"

class TransferData 
{
    public:
    /**
     * @brief Constructor for transfer data object
     * @param transfer_data_logger local reference to the parent logger
     */
    TransferData(int socket, Logger& transfer_data_logger);
    /**
     * @brief method used to facilitate the transfer of data
     * @param can_id frame id that contains the sender and receiver
     * @param transfer_request data to be transferred
     */
    void transferData(canid_t can_id, std::vector<uint8_t>& transfer_request);
    private:
    Logger& transfer_data_logger;
    GenerateFrames generate_frames;
    MemoryManager* memory_manager;
    int socket = -1;
    static uint8_t expected_block_sequence_number;

};






#endif