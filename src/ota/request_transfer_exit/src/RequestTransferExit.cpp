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
RequestTransferExit::RequestTransferExit() : callback_(nullptr)
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
bool RequestTransferExit::requestTransferExit(bool transferSucces)
{
    LOG_INFO(MCULogger.GET_LOGGER(), "Exiting transfer with service 0x37 ");

    /* check if the callback is set */
    if (callback_)
    {
        /** Invoke the callback with the result of the transfer 
        *   return true to continue or false to stop it
        */
       callback_(transferSucces);

       /* Return true if the callback indicates the transfer should continue */
       return transferSucces;
    }

    /* If no callback is set, default to continue the transfer */
    return transferSucces;
}
