/**
 * @file WriteDataByIdentifier.h
 * @author Dirva Nicolae
 * @brief This library represents the WriteDataByIdentifier UDS service.
 * It writes data based on the Data Identifier (DID) received in the CAN frame.
 * For example, if you want to update the battery voltage, 0x01B0 DID is responsible for that.
 * The request frame received by the service will have the format: frame.data = {PCI_L(1byte), SID(1byte = 0x2E), DID(2bytes), DATA}
 * The positive response frame sent by the service will have the format: frame.data = {PCI_L(1byte), RESPONSE_SID(1byte = 0x6E), DID(2bytes)}
 * The negative response frame sent by the service will have the format: frame.data = {PCI_L(1byte), 0x7F, SID(1byte = 0x2E), NRC(1byte)}
 */

#include <iostream>
#include <vector>
#include <cstdint>
#include <string>
#include <sys/ioctl.h>
#include <sys/socket.h>
#include <net/if.h>
#include <unistd.h>
#include <fcntl.h>
#include<linux/can.h>
#include <unordered_set>
#include<iomanip>

#include "../../../utils/include/GenerateFrames.h"
#include "../../../utils/include/Logger.h"

#ifndef UDS_WDBI_SERVICE
#define UDS_WDBI_SERVICE

class WriteDataByIdentifier
{
private:
    GenerateFrames generate_frames;
    int socket = -1;
    Logger& wdbi_logger;

public:
    /**
     * @brief Construct a new Write Data By Identifier object
     * 
     * @param frame_id 
     * @param frame_data 
     * @param WDBILogger 
     */
    WriteDataByIdentifier(canid_t frame_id, std::vector<uint8_t> frame_data, Logger& wdbi_logger, int socket);
    /**
     * @brief Destroy the Write Data By Identifier object
     * 
     */
    ~WriteDataByIdentifier();
    /**
     * @brief Execute the WriteDataByIdentifier service.
     * 
     * This function performs the WriteDataByIdentifier service, writing the received data to the specified
     * Data Identifier (DID) and sending the appropriate response frame or a negative response if an error occurs.
     * 
     * @param frame_id 
     * @param frame_data 
     */
    void WriteDataByIdentifierService(canid_t frame_id, std::vector<uint8_t> frame_data);
};

#endif