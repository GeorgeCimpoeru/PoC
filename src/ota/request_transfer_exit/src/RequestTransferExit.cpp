/**
 * @brief 
 * @file RequestTransferExit.cpp
 * @author Loredana Dugulean
 * @date 2024-06-26
 * 
 */

#include "../include/RequestTransferExit.h"
#include <iostream>

/* Constructor that initializes the callback_ to nullptr */
RequestTransferExit::RequestTransferExit(Logger& RTESLogger) : callback_(nullptr)
{
}

/* Destructor */
RequestTransferExit::~RequestTransferExit()
{    
}

/* Method to set the callback function */
void RequestTransferExit::setTransferCompleteCallBack(transferCompleteCallBack callback)
{
    callback_ = callback;
}

/* Method to handle the request transfer exit 0x37 and invoke the callback */
bool RequestTransferExit::requestTransferExit(int id, std::vector<uint8_t>stored_data, Logger& RTESlogger, bool transferSucces)
{
    LOG_INFO(RTESlogger.GET_LOGGER(), "Exiting transfer with service 0x37 ");

    /* check if the callback is set */
    if (callback_)
    {
        /** Invoke the callback with the result of the transfer data
        *   return true to continue or false to stop it
        */
       callback_(transferSucces);

       /* Return true if the callback indicates the transfer should continue */
       return transferSucces;
    }

    /* If no callback is set, default to continue the transfer */
    return transferSucces;

    requestTransferExitResponse(id, RTESlogger);
}

void RequestTransferExit::requestTransferExitResponse(int id, Logger& RTESlogger)
{
    /* Generate the response frame and send it */
    LOG_INFO(RTESlogger.GET_LOGGER(), "RequestTransferExit: Response sent");


    uint8_t frame_dest_id = (id >> 8) & 0xFF;
    LOG_INFO(RTESlogger.GET_LOGGER(), "frame_dest_id = 0x{0:x}", frame_dest_id);

    uint8_t frame_sender_id = id & 0xFF;
    LOG_INFO(RTESlogger.GET_LOGGER(), "frame_sender_id = 0x{0:x}", frame_sender_id);
    
    id = (frame_sender_id << 8) | frame_dest_id;
    LOG_INFO(RTESlogger.GET_LOGGER(), "can_id = 0x{0:x}", id);
    generate_frames.requestTransferExit(id, true);
}
