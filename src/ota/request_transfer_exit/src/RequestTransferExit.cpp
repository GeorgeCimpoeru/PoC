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
        uint8_t nrc = 0x13; 
        /* Send the negative response frame */        
        generate_frames.negativeResponse(can_id, RTES_SERVICE_ID, nrc);
        return;
    }
    else    
    {
        std::ifstream file("mcu_data.txt");
        std::string line;
        uint8_t value;
        bool did_found = false;
        
        while (std::getline(file, line))
        {
            std::istringstream iss(line);
            std::string key_str;
            uint16_t key;
            
            if (iss >> std::hex >> key && key == 0x01E0)
            {
                /* Read the first byte associated with the DID */
                iss >> std::hex >> value;
                did_found = true;
                break;
            }
        }
        /* Retrieve transfer_status based on the OTA_UPDATE_STATUS_DID if it exists */
        if (did_found != 01)
        {
            /* Check if the transfer data has been completed */
            if (value == 0x31)        
                {
                /* prepare positive response */
                response.push_back(0x02); /* PCI */
                response.push_back(0x77); /* Service ID */
                response.push_back(request_transfer_exit_data[2]);
                /* Send the postive response frame */ 
                generate_frames.sendFrame(can_id, response);	
                }
                else
                {                                
                /* Request sequence error - prepare a negative response */
                uint8_t nrc = 0x24;
                /* Send the negative response frame */ 
                generate_frames.negativeResponse(can_id, RTES_SERVICE_ID, nrc);
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
