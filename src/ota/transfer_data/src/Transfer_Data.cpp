/**
 * @brief 
 * @file transfer_data.cpp
 * @author Avramescu Anamaria
 * @date 2024-06-26
 * 
 */

#include "../include/Transfer_Data.h"
#include "../include/RequestTransferExit.h" 
//#include "../../../utils/include/Logger.h"
 #include "../../../mcu/include/MCULogger.h"
// #include <spdlog/spdlog.h>
#include <iostream>

/* Constructor that initializes the callback_ to nullptr */
TransferDataRequest::TransferDataRequest(Logger &logger) : callback_(nullptr), TransferDataLog(logger)
{
  /* Set up the exit callback*/ 
    exitHandler_.setTransferCompleteCallBack([this](bool transferSuccess) {
        return this->transferDataRequest(transferSuccess);
    });
  
}

/* Destructor */
TransferDataRequest::~TransferDataRequest()
{    
}

/* Method to set the callback function */
void TransferDataRequest::setTransferDataCallBack(transferDataCallBack callback)
{
    callback_ = callback;
}

/* Method to handle the request transfer data 0x36 and invoke the callback */
bool TransferDataRequest::transferDataRequest(bool transferSucces)
{
    LOG_INFO(TransferDataLog.GET_LOGGER(), "Transfer with service 0x36 ");

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

