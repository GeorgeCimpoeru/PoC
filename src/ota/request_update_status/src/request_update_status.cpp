/**
 * @file request_update_status.cpp
 * @author Iancu Daniel
 * @brief 
 * @version 0.1
 * @date 2024-06-27
 * 
 * @copyright Copyright (c) 2024
 * 
 */

#include "../include/request_update_status.h"

RequestUpdateStatus::RequestUpdateStatus(Logger logger) : _logger{logger}
{
    create_interface = create_interface->getInstance(0x00, _logger);
}

void RequestUpdateStatus::requestUpdateStatus(canid_t request_id, std::vector<uint8_t> request)
{
    /* Request validation similar to ReadDataByIdentifier, no need for making the same checks. */

    /* Create the response_id (swap sender with receiver)*/
    canid_t response_id = request_id;
    uint8_t receiver_byte = GET_BYTE(response_id, 0);   /* Get the first byte - receiver */
    uint8_t sender_byte = GET_BYTE(response_id, 1);     /* Get second byte - sender */

    SET_BYTE(response_id, 0, sender_byte);              /* Replace sender with receiver */
    SET_BYTE(response_id, 1, receiver_byte);            /* Replace receiver with sender */

    /* Create the request for ReadDataByIdentifier */
    std::vector<uint8_t> readDataRequest;
    readDataRequest.emplace_back(request[0]);   /* PCI_l*/
    readDataRequest.emplace_back(READ_DATA_BY_IDENTIFIER_SID);   /* ReadDataByIdentifier SID */
    readDataRequest.emplace_back(OTA_STATUS_UPDATE_DID_MSB);    /* OTA_UPDATE_STATUS_MSB_DID */
    readDataRequest.emplace_back(OTA_STATUS_UPDATE_DID_LSB);    /* OTA_UPDATE_STATUS_LSB_DID */

    SET_BYTE(request_id, 1, 0x00); /* Set sender to 0x00, this will tell the readData service to not send a frame, but return the response */
    /* ReadDataByIdentifier RIDB; */ 
    /* std::vector<uint8_t> RIDB_response = RIDB.readDataByIdentifier(request_id, readDataRequest, _logger); */

    /* Dummy data, will be replaced by data from service */
    std::vector<uint8_t> RIDB_response = {0x03, 0x7F, 0x01, 0xE0, 0x41};
    std::vector<uint8_t> response;

    if(RIDB_response[1] == 0x7F)
    {
        response.push_back(RIDB_response[0]);   /* PCI_l*/
        response.push_back(RIDB_response[1]);   /* Negative response */
        response.push_back(REQUEST_UPDATE_STATUS_SERVICE);  /* SID */
        response.push_back(RIDB_response[3]); /* Negative response code */
    }
    else
    {   
        response.push_back(0x03);   /* PCI_l */
        response.push_back(REQUEST_UPDATE_STATUS_SERVICE_SUCCESS); /* SERVICE SUCCESs = SID + 0X40 */
        response.push_back(RIDB_response[4]);   /* OTA UPDATE STATUS */
    }

    GenerateFrames generate_frames(create_interface->getSocketApiWrite(), _logger);
    generate_frames.sendFrame(response_id, response);
}


RequestUpdateStatus::~RequestUpdateStatus()
{

}