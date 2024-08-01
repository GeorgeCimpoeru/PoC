/**
 * @file RequestDownload.h
 * @author Stoisor Miruna & Ruben Mujdei
 * @brief  This class is used in scenarios where firmware or data needs to be downloaded to 
 * an MCU or ECU. It ensures that requests are authenticated, sessions are valid, and the 
 * data is correctly formatted and handled. It provides methods for dealing with both 
 * compressed and uncompressed data, handles memory management, and manages manual or automatic OTA updates.
 * Data format:
 * The request frame format: stored.data = {PCI_L(1byte), SID(1byte = 0x34), DFI(1byte),AML(1byte), MA+MA.size(1byte or more), MS+MS.size(1byte or more))}
 * The positive response format: frame.data = {LMNB(1byte), RESPONSE_SID(1byte = 0x74), LMNB_format(1byte), MNB(1byte or more)}
 * The negative response format: frame.data = {PCI_L(1byte), 0x7F, SID(1byte = 0x34), NRC(1byte)}
 */

#ifndef REQUEST_DOWNLOAD_SERVICE_H
#define REQUEST_DOWNLOAD_SERVICE_H

#include <iostream>
#include <vector>
#include <sstream>
#include <cstring>
#include <string>
#include <utility>
#include <chrono>
#include <sys/ioctl.h>
#include <linux/can.h>
#include <net/if.h>
#include "../../../utils/include/CreateInterface.h"
#include "../../../utils/include/GenerateFrames.h"
#include "../../utils/include/Logger.h"
#include "../../uds/diagnostic_session_control/include/DiagnosticSessionControl.h"
#include "../../uds/authentication/include/SecurityAccess.h"
#include "../../uds/read_data_by_identifier/include/ReadDataByIdentifier.h"
#include "../../utils/include/MemoryManager.h"
#include <pybind11/embed.h>
class RequestDownloadService
{
public:
    Logger RDSlogger;
    /**
     * @brief Construct a new Request Download Service object
     * 
     * @param RDSlogger 
     */
    RequestDownloadService(Logger &RDSlogger);
    /**
     * @brief Construct a new Request Download Service object
     * 
     * @param socket 
     * @param RDSlogger 
     */
    RequestDownloadService(int socket, Logger &RDSlogger);
    /**
     * @brief Destroy the Request Download Service object
     *
     */
    ~RequestDownloadService();
    /**
     * @brief Request download method to validate and prepare the system to receive data
     * 
     * @param id 
     * @param stored_data 
     */
    void requestDownloadRequest(int id, std::vector<uint8_t> stored_data);
    /**
     * @brief Response method from manual Request Download service to start the download in MCU
     * 
     * @param id 
     * @param max_number_block 
     */
    void requestDownloadResp(int id, int memory_address, int max_number_block);

private:
    int socket = -1;
    GenerateFrames generateFrames;
    /**
     * @brief Method for checking if the MCU is in the programming session
     * 
     * @return true 
     */
    bool isInProgrammingSession();
    /**
     * @brief Method for validation of the provided memory address and size, ensuring they are within acceptable bounds and logical ranges.
     *
     * @param memory_address
     * @param memory_size
     * @return true
     */
    bool isValidMemoryRange(const int &memory_address, const int &memory_size);
    /**
     * @brief Method to authenticate the download request based on the provided identifiers
     * 
     * @return true 
     */
    bool isRequestAuthenticated();
    /**
     * @brief Method to check last software version
     * 
     * @return true 
     * @return false 
     */
    bool isLatestSoftwareVersion();
    /**
     * @brief Extract size and address
     * 
     * @param stored_data 
     * @param length_memory_address 
     * @param length_memory_size 
     * @return std::pair<int,int> 
     */
    std::pair<int,int> extractSizeAndAddress( std::vector<uint8_t> stored_data, uint8_t length_memory_address, uint8_t length_memory_size );
    /**
     * @brief Extract size and address length
     * 
     * @param stored_data 
     * @return std::pair<int,int> 
     */
    std::pair<int,int> extractSizeAndAddressLength( std::vector<uint8_t> stored_data);
    void download();
    /**
     * @brief Response to request download
     * 
     * @param receiver_id 
     * @param memory_address 
     * @param max_number_block 
     */
    void requestDownloadResp89(int receiver_id, int memory_address, int max_number_block);
    /**
     * @brief Method to calculate max_number_block
     * 
     * @return int 
     */
    int calculate_max_number_block(int memory_size);
    /**
     * @brief Method to read frame from can bus
     * 
     * @param id 
     * @param sid 
     * @return can_frame* 
     */
    can_frame* read_frame(int id, uint8_t sid);
    /**
     * @brief Method to download data in ECU
     * 
     * @param id 
     * @param memory_address 
     * @return true 
     */
    void downloadInEcu(int id, int memory_address);

    void downloadSoftwareVersion(std::string version_file_id);
};

#endif /* REQUEST_DOWNLOAD_SERVICE_H */