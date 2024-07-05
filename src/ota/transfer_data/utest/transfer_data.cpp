/**
 * @brief 
 * @file transfer_data.cpp
 * @author Avramescu Anamaria
 * @date 2024-06-26
 * 
 */

#include "../include/transfer_data.h"
#include <iostream>

/* Constructor that initializes the callback_ to nullptr */
TransferDataRequest::TransferDataRequest() : callback_(nullptr)
{
}

/* Destructor */
TransferDataRequest::~TransferDataRequest()
{    
}

/* Method to set the callback function */
void TransferDataRequest::setTransferCompleteCallBack(transferCompleteCallBack callback)
{
    callback_ = callback;
}

/* Method to handle the request transfer exit 0x37 and invoke the callback */
bool TransferDataRequest::transferDataRequest(bool transferSucces)
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