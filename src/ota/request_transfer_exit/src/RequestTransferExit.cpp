/**
 * @brief 
 * @file RequestTransferExit.cpp
 * @author Loredana Dugulean
 * @date 2024-06-26
 * 
 */

#include "src/ota/request_transfer_exit/include/RequestTransferExit.h"

/* Constructor that initializes the callback_ to nullptr */
RequestTransferExit::RequestTransferExit(Logger& RTESLogger) : callback_(nullptr), RTESLogger(RTESLogger)
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
bool RequestTransferExit::requestTransferExit(int id, bool transferSuccess)
{
    LOG_INFO(RTESLogger.GET_LOGGER(), "Exiting transfer with service 0x37 ");

    /* check if the callback is set */
    if (callback_)
    {
        /** Invoke the callback with the result of the transfer data
        *   returns true to continue or false to stop the transfer
        */
       bool continueTransfer = callback_(transferSuccess);

       /* Return the result from the callback indicating if the transfer should continue or stop */
       return continueTransfer;
    }

    /* If no callback is set, default to false to stop the transfer */
    return false;
}
