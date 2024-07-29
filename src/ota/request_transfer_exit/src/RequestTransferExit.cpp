/**
 * @brief 
 * @file RequestTransferExit.cpp
 * @author Loredana Dugulean
 * @date 2024-06-26
 * 
 */

#include "../include/RequestTransferExit.h"
#include <iostream>
#include "../../../mcu/include/MCUModule.h"

/* Constructor that initializes the callback_ to nullptr */
RequestTransferExit::RequestTransferExit(int socket, Logger& RTESLogger) : callback_(nullptr), RTESLogger(RTESLogger), generate_frames(socket, RTESLogger)
{
    this->socket = socket;
}

/* Destructor */
RequestTransferExit::~RequestTransferExit()
{    
}

/* Define the service identifier for Read Data By Identifier */
const uint8_t RTES_SERVICE_ID = 0x37;

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

/* Method to handle the request transfer exit 0x37 */
/* frame format = {PCI_L, SID(0x37), transfer_request_parameter_record}*/
void RequestTransferExit::requestTRansferExitRequest(canid_t can_id, const std::vector<uint8_t>& request_transfer_exit_data)
{
    std::vector<uint8_t> response;
    /* Check the frame data */
    if (request_transfer_exit_data.size() < 3)
    {
        /* Incorrect message length or invalid format - prepare a negative response */
        response.push_back(0x03); /* PCI */
        response.push_back(0x7F); /* Negative response */
        response.push_back(0x37); /* Service ID */
        response.push_back(0x13); /* Incorrect message length or invalid format */
        /* Send the negative response frame */ 
        generate_frames.sendFrame(can_id, response);
        return;
    }
    else    
    {       
        /* Retrieve transfer_status based on the OTA_UPDATE_STATUS_DID if it exists */
        if ( MCU::mcu.mcu_data.find(0x01E0) != MCU::mcu.mcu_data.end())
        {
            uint8_t transfer_status = MCU::mcu.mcu_data.at(0x01E0)[0];
            /* Check if the transfer data has been completed */
            if (transfer_status == 0x31)
        
                {
                /* prepare positive response */
                response.push_back(0x02); /* PCI */
                response.push_back(0x77); /* Service ID */
                response.push_back(request_transfer_exit_data[2]); /* contains info about */
                /* Send the postive response frame */ 
                generate_frames.sendFrame(can_id, response);	
                }
                else
                {                                
                /* Request sequence error - prepare a negative response */
                response.push_back(0x03); /* PCI */
                response.push_back(0x7F); /* Negative response */
                response.push_back(0x37); /* Service ID */
                response.push_back(0x24); /* Request sequence error */
                /* Send the negative response frame */ 
                generate_frames.sendFrame(can_id, response);
                return;
                }       
        }
        else
        {
           /*  OTA_UPDATE_STATUS_DID not found */
            LOG_ERROR(RTESLogger.GET_LOGGER(),"OTA_UPDATE_STATUS_DID 01E0 not found "); 
        }
    }
}
