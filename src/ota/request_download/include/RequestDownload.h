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
#include <zip.h>
#include "../../../utils/include/CreateInterface.h"
#include "../../../utils/include/GenerateFrames.h"
#include "../../utils/include/Logger.h"
#include "../../uds/diagnostic_session_control/include/DiagnosticSessionControl.h"
#include "../../../utils/include/NegativeResponse.h"
#include "../../uds/authentication/include/SecurityAccess.h"
#include "../../uds/read_data_by_identifier/include/ReadDataByIdentifier.h"
#include "../../utils/include/MemoryManager.h"
#include <pybind11/embed.h>

/* ECU permitted transfer data requests */
#define MAX_TRANSFER_DATA_REQUESTS 5
#define MAXIMUM_ALLOWED_DOWNLOAD_SIZE  50000000

class RequestDownloadService
{
public:
    static constexpr uint8_t RDS_SID = 0x34;
    /**
     * @brief Construct a new Request Download Service object
     * 
     * @param RDSlogger A logger instance used to record information and errors during the execution.
     */
    RequestDownloadService(Logger &RDSlogger);
    /**
     * @brief Construct a new Request Download Service object
     * 
     * @param socket The socket descriptor used for communication over the CAN bus.
     * @param RDSlogger A logger instance used to record information and errors during the execution.
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
     * @param id An unique identifier.
     * @param stored_data 
     */
    void requestDownloadRequest(canid_t id, std::vector<uint8_t> stored_data);
    /**
     * @brief Response method from manual Request Download service to start the download in MCU
     * 
     * @param id An unique identifier.
     * @param memory_address Starting address of the memory where the data needs to be written.
     * @param max_number_block Used while sending a positive response to tell the number of data bytes to consider.
     */
    void requestDownloadResponse(canid_t id, int memory_address, int max_number_block);

    static size_t getMaxNumberBlock();
private:
    int socket = -1;
    Logger& RDSlogger;

    GenerateFrames generate_frames;
    static size_t max_number_block;
    /**
     * @brief Method for validation of the provided memory address and size, ensuring they are within acceptable bounds and logical ranges.
     *
     * @param id An unique identifier.
     * @param memory_address Starting address of the memory where the data needs to be written.
     * @param memory_size Indicates the number of bytes in the block that needs to be downloaded in the server’s flash memory.
     * @return true
     */
    bool isValidMemoryRange(const int &memory_address, const int &memory_size);
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
     * @param stored_data A vector used to store the data received in the service request.
     * @param length_memory_address The length of the memory address field in the stored_data.
     * @param length_memory_size The length of the memory size field in the stored_data.
     * @return std::pair<int,int> 
     */
    std::pair<int,int> extractSizeAndAddress( std::vector<uint8_t> stored_data, uint8_t length_memory_address, uint8_t length_memory_size );
    /**
     * @brief Extract size and address length
     * 
     * @param stored_data A vector used to store the data received in the service request.
     * @return std::pair<int,int> 
     */
    std::pair<int,int> extractSizeAndAddressLength(canid_t id, std::vector<uint8_t> stored_data);
    /**
     * @brief Response to request download
     * 
     * @param receiver_id The receiver identifier.
     * @param memory_address Starting address of the memory where the data needs to be written.
     * @param max_number_block Used while sending a positive response to tell the number of data bytes to consider.
     */
    void requestDownloadAutomatic(canid_t receiver_id, int memory_address, int max_number_block);
    /**
     * @brief Method to calculate max_number_block
     * 
     * @param memory_size Indicates the number of bytes in the block that needs to be downloaded in the server’s flash memory.
     * @return int 
     */
    int calculate_max_number_block(int memory_size);
    /**
     * @brief Method to read frame from can bus
     * 
     * @param id An unique identifier.
     * @param sid Request download service id.
     * @return can_frame* 
     */
    can_frame* read_frame(int id, uint8_t sid);
    /**
     * @brief Method to download data in ECU
     * 
     * @param id An unique identifier.
     * @param memory_address Starting address of the memory where the data needs to be written.
     * @return true 
     */
    void downloadInEcu(int id, int memory_address);

    /**
     * @brief Method for downloading software version from google drive.
     * 
     * @param ecu_id The identifier of ECU.
     * @param sw_version The software version.
     */
    void downloadSoftwareVersion(uint8_t ecu_id, uint8_t sw_version);

    /**
     * @brief Method to extract the zipped file.
     * 
     * @param target_id Targeted ecu for file unzipping.
     * @param zipFilePath Path to zip file.
     * @param outputDir Path for the extracted file.
     */
    bool extractZipFile(uint8_t target_id, const std::string &zipFilePath, const std::string &outputDir);
};

#endif /* REQUEST_DOWNLOAD_SERVICE_H */
