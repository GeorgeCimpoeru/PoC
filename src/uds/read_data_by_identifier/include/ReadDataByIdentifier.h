/**
 * @file ReadDataByIdentifier.h
 * @author your name (you@domain.com)
 * @brief This library represents the ReadDataByIdentifier UDS service.
 * It retrieves data based on the Data Identifier(DID) received in the can_frame.]
 * For example, if you want to know the battery voltage, 0x01B0 DID is responsible for that.
 * The request frame receive by the service will have the format: frame.data = {PCI_L(1byte), SID(1byte = 0x22), DID(2bytes),Padding(4bytes)}
 * The positive response frame sent by the service will have the format: frame.data = {PCI_L(1byte), RESPONSE_SID(1byte = 0x62), DID(2bytes), DATA}
 * The negative response frame sent by the service will have the format: frame.data = {PCI_L(1byte), 0x7F, SID(1byte = 0x22), NRC(1byte)}
 */


#include <linux/can.h>
#include <vector>
#include <unordered_map>
#include <bitset>

#include "../../../utils/include/CreateInterface.h"
#include "../../../utils/include/GenerateFrames.h"
#include "../../utils/include/Logger.h"

#ifndef UDS_READ_DATA_BY_IDENTIFIER_H
#define UDS_READ_DATA_BY_IDENTIFIER_H

class ReadDataByIdentifier
{
    public:
    /**
    * @brief Default constructor
    */
    ReadDataByIdentifier();
    /**
    * @brief Method that retrieves some data based on a DID.
    * @param can_id The frame id.
    * @param request Data from a can frame that contains PCI, SID and DID
    * @param RDBILogger Logger reference to log in a specific file.
    */
    std::vector<uint8_t> readDataByIdentifier(canid_t can_id, const std::vector<uint8_t>& request, Logger& RDBILogger);
    
    private:
    GenerateFrames* generate_frames;
    CreateInterface* create_interface;

};

#endif