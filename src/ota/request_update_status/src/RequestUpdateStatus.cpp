/**
 * @file RequestUpdateStatus.cpp
 * @author Iancu Daniel
 * @brief 
 * @version 0.1
 * @date 2024-06-27
 * 
 * @copyright Copyright (c) 2024
 * 
 */

#include "../include/RequestUpdateStatus.h"

RequestUpdateStatus::RequestUpdateStatus(int socket) : socket(socket)
{}

std::vector<uint8_t> RequestUpdateStatus::requestUpdateStatus(canid_t request_id, std::vector<uint8_t> request)
{
    /* Request validation similar to ReadDataByIdentifier, no need for making the same checks. */

    /* Create the response_id (swap sender with receiver)*/
    canid_t response_id = request_id;
    uint8_t receiver_byte = (response_id & 0xff); /* Get the first byte - receiver */
    uint8_t sender_byte = ((response_id & 0xff00) >> 8);     /* Get second byte - sender */

    if(receiver_byte != MCU_ID || sender_byte != API_ID)
    {
        LOG_WARN(MCULogger->GET_LOGGER(), "Request update status must be made from API to MCU. Request redirected from API to MCU.");
        receiver_byte = MCU_ID;
        sender_byte = API_ID;
        request_id = (API_ID << 8) | MCU_ID;
    }

    response_id = (receiver_byte << 8) | sender_byte; /* Replace sender with receiver for response id */

    /* Create the request for ReadDataByIdentifier */
    std::vector<uint8_t> readDataRequest;
    readDataRequest.emplace_back(request[0]);   /* PCI_l*/
    readDataRequest.emplace_back(READ_DATA_BY_IDENTIFIER_SID);   /* ReadDataByIdentifier SID */
    readDataRequest.emplace_back(OTA_UPDATE_STATUS_DID_MSB);    /* OTA_UPDATE_STATUS_MSB_DID */
    readDataRequest.emplace_back(OTA_UPDATE_STATUS_DID_LSB);    /* OTA_UPDATE_STATUS_LSB_DID */

    ReadDataByIdentifier RIDB(socket, MCULogger);
    std::vector<uint8_t> RIDB_response = RIDB.readDataByIdentifier(request_id, readDataRequest, false);
    std::vector<uint8_t> response;

    /* If a negative response is sent from readDataByIdentifier service, send the same response from requestUpdateStatus, but with changed SID. */
    if(RIDB_response[1] == NEGATIVE_RESPONSE)
    {
        response.push_back(RIDB_response[0]);           /* PCI_l*/
        response.push_back(RIDB_response[1]);           /* Negative response */
        response.push_back(REQUEST_UPDATE_STATUS_SID);  /* SID */
        response.push_back(RIDB_response[3]);           /* Negative response code */
    }
    else
    {   /* Check if the received status value is a valid one */
        uint8_t status = RIDB_response[0];
        if (isValidStatus(RIDB_response[0]) == 0)
        {
            LOG_WARN(MCULogger->GET_LOGGER(), "Status value {} read from readDataByIdentifier is invalid.", status);
            response.push_back(PCI_L);           /* PCI_l*/
            response.push_back(NEGATIVE_RESPONSE);           /* Negative response */
            response.push_back(REQUEST_UPDATE_STATUS_SID);  /* SID */
            response.push_back(REQUEST_OUT_OF_RANGE);           /* Negative response code */
        }
        else
        {
            /* Everything ok, Ota Update Status received*/
            response.push_back(PCI_L);                              /* PCI_l */
            response.push_back(REQUEST_UPDATE_STATUS_SID_SUCCESS);  /* SERVICE SUCCESs = SID + 0X40 */
            response.push_back(status);                   /* OTA UPDATE STATUS */
        }
    }

    GenerateFrames generate_frames(socket, *MCULogger);
    generate_frames.requestUpdateStatusResponse(response_id, response);
    
    return response;
}

bool RequestUpdateStatus::isValidStatus(uint8_t status)
{
    std::vector<OtaUpdateStatesEnum> valid_states{IDLE,INIT,READY,PROCESSING,PROCESSING_TRANSFER_COMPLETE,PROCESSING_TRANSFER_FAILED,WAIT,WAIT_DOWNLOAD_COMPLETED,WAIT_DOWNLOAD_FAILED,VERIFY,ACTIVATE,ACTIVATE_INSTALL_COMPLETE,ACTIVATE_INSTALL_FAILED,ERROR};
    /* Check if provided status from readDataByIdentifier is a valid one. */
    for(auto &state : valid_states)
    {
        if(status == state)
        {
            return 1;
        }
    }
    return 0;
}

RequestUpdateStatus::~RequestUpdateStatus()
{

}