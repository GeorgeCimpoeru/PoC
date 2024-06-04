/**
 * @file GenerateFrame.h
 * @brief For creating a CAN frame
 * This class is used to create a CAN frame and send it to the CAN bus.
 * The class has a constructor that takes the frame type, CAN ID, data, and
 * data length as arguments.
 * The private method GenerateFrame creates the CAN frame with the given
 * arguments.
 * The SendFrame method sends the created frame to the CAN bus.
 * It uses the CAN socket interface to send the frame to the CAN bus.
 */

#ifndef CREATE_FRAME_H
#define CREATE_FRAME_H

#include <linux/can.h>
#include <string>
#include <cstring>
#include <stdexcept>
#include <iostream>
#include <sys/ioctl.h>
#include <net/if.h>
#include <sys/socket.h>
#include <unistd.h>
#include <vector>

enum FrameType
{
    DATA_FRAME,
    REMOTE_FRAME,
    ERROR_FRAME,
    OVERLOAD_FRAME
};

class GenerateFrame
{
public:
    struct can_frame frame;

    /**
     * @brief Constructor for GenerateFrame class.
     * @param socket The CAN socket file descriptor.
     */
    GenerateFrame(int socket);

    /**
     * @brief Default constructor for GenerateFrame class.
     */
    GenerateFrame();

    /**
     * @brief Sends the created frame to the CAN bus.
     * @param can_id The CAN ID of the frame.
     * @param data The data to be sent in the frame.
     * @param frameType The type of the frame (default: DATA_FRAME).
     * @return Returns 0 on success, -1 on failure.
     */
    int SendFrame(uint32_t can_id, std::vector<uint8_t> data, FrameType frameType = DATA_FRAME);

    /**
     * @brief Gets the CAN frame.
     * @return Returns the CAN frame.
     */
    can_frame getFrame();

    /**
     * @brief Gets the CAN socket file descriptor.
     * @return Returns the CAN socket file descriptor.
     */
    int getSocket();

    /**
     * @brief Creates a CAN frame with the given arguments.
     * @param can_id The CAN ID of the frame.
     * @param data The data to be sent in the frame.
     * @param frameType The type of the frame (default: DATA_FRAME).
     * @return Returns the created CAN frame.
     */
    can_frame CreateFrame(uint32_t can_id, std::vector<uint8_t> data, FrameType frameType = DATA_FRAME);

    /**
     * @brief Creates a CAN frame with multiple segments with the given arguments.
     * @param can_id The CAN ID of the frame.
     * @param sid The service identifier.
     * @param dataIdentifier The data identifier.
     * @param response The response data.
     * @param firstFrame Indicates if it is the first frame (default: true).
     */
    void CreateFrameLong(uint32_t can_id, uint8_t sid, uint8_t dataIdentifier, std::vector<uint8_t> response, bool firstFrame);

    /**
     * @brief Performs session control.
     * @param can_id The CAN ID of the frame.
     * @param subfunction The subfunction.
     * @param response Indicates if it is a response frame (default: false).
     */
    void SessionControl(uint32_t can_id, uint8_t subfunction, bool response = false);

    /**
     * @brief Performs ECU reset.
     * @param can_id The CAN ID of the frame.
     * @param response Indicates if it is a response frame (default: false).
     */
    void EcuReset(uint32_t can_id, bool response = false);

    /**
     * @brief Reads data by identifier.
     * @param can_id The CAN ID of the frame.
     * @param dataIdentifier The data identifier.
     * @param response The response data (default: empty vector).
     */
    void ReadDataByIdentifier(uint32_t can_id, uint8_t dataIdentifier, std::vector<uint8_t> response = {});

    /**
     * @brief Reads data by identifier (long format).
     * @param can_id The CAN ID of the frame.
     * @param dataIdentifier The data identifier.
     * @param response The response data (default: empty vector).
     * @param firstFrame Indicates if it is the first frame (default: true).
     */
    void ReadDataByIdentifierLong(uint32_t can_id, uint8_t dataIdentifier, std::vector<uint8_t> response = {}, bool firstFrame = true);

    /**
     * @brief Sends flow control frame.
     * @param can_id The CAN ID of the frame.
     */
    void FlowControlFrame(uint32_t can_id);

    /**
     * @brief Sends authentication request seed frame.
     * @param can_id The CAN ID of the frame.
     * @param seed The seed data (default: empty vector).
     */
    void AuthenticationRequestSeed(uint32_t can_id, const std::vector<uint8_t> &seed = {});

    /**
     * @brief Sends authentication send key frame.
     * @param can_id The CAN ID of the frame.
     * @param key The key data (default: empty vector).
     */
    void AuthenticationSendKey(uint32_t can_id, const std::vector<uint8_t> &key = {});

    /**
     * @brief Performs routine control.
     * @param can_id The CAN ID of the frame.
     * @param subfunction The subfunction.
     * @param routine_identifier The routine identifier.
     * @param response Indicates if it is a response frame (default: false).
     */
    void RoutineControl(uint32_t can_id, uint8_t subfunction, uint8_t routine_identifier, bool response = false);

    /**
     * @brief Sends tester present frame.
     * @param can_id The CAN ID of the frame.
     * @param response Indicates if it is a response frame (default: false).
     */
    void TesterPresent(uint32_t can_id, bool response = false);

    /**
     * @brief Reads memory by address.
     * @param can_id The CAN ID of the frame.
     * @param memory_size The memory size.
     * @param memory_address The memory address.
     * @param response The response data (default: empty vector).
     */
    void ReadMemoryByAddress(uint32_t can_id, uint8_t memory_size, uint8_t memory_address, std::vector<uint8_t> response = {});

    /**
     * @brief Reads memory by address (long format).
     * @param can_id The CAN ID of the frame.
     * @param memory_size The memory size.
     * @param memory_address The memory address.
     * @param response The response data.
     * @param first_frame Indicates if it is the first frame (default: true).
     */
    void ReadMemoryByAddressLong(uint32_t can_id, uint8_t memory_size, uint8_t memory_address, std::vector<uint8_t> response, bool first_frame = true);

    /**
     * @brief Writes data by identifier.
     * @param can_id The CAN ID of the frame.
     * @param identifier The identifier.
     * @param data_parameter The data parameter (default: empty vector).
     */
    void WriteDataByIdentifier(uint32_t can_id, uint8_t identifier, std::vector<uint8_t> data_parameter = {});

    /**
     * @brief Writes data by identifier (long format).
     * @param can_id The CAN ID of the frame.
     * @param identifier The identifier.
     * @param data_parameter The data parameter.
     * @param first_frame Indicates if it is the first frame (default: true).
     */
    void WriteDataByIdentifierLong(uint32_t can_id, uint8_t identifier, std::vector<uint8_t> data_parameter, bool first_frame = true);

    /**
     * @brief Reads DTC information.
     * @param can_id The CAN ID of the frame.
     * @param subfunction The subfunction.
     * @param dtc_status_mask The DTC status mask.
     */
    void ReadDtcInformation(uint32_t can_id, uint8_t subfunction, uint8_t dtc_status_mask);

    /**
     * @brief Sends DTC information response frame (format 01).
     * @param can_id The CAN ID of the frame.
     * @param status_availability_mask The status availability mask.
     * @param dtc_format_identifier The DTC format identifier.
     * @param dtc_count The DTC count.
     */
    void ReadDtcInformationResponse01(uint32_t can_id, uint8_t status_availability_mask, uint8_t dtc_format_identifier, uint8_t dtc_count);

    /**
     * @brief Clears diagnostic information.
     * @param can_id The CAN ID of the frame.
     * @param group_of_dtc The group of DTC (default: {0xFF, 0xFF, 0xFF}).
     * @param response Indicates if it is a response frame (default: false).
     */
    void ClearDiagnosticInformation(uint32_t can_id, std::vector<uint8_t> group_of_dtc = {0xFF, 0xFF, 0xFF}, bool response = false);

    /**
     * @brief Accesses timing parameters.
     * @param can_id The CAN ID of the frame.
     * @param subfunction The subfunction.
     * @param response Indicates if it is a response frame (default: false).
     */
    void AccessTimingParameters(uint32_t can_id, uint8_t subfunction, bool response = false);

    /**
     * @brief Sends negative response frame.
     * @param can_id The CAN ID of the frame.
     * @param sid The service identifier.
     * @param nrc The negative response code.
     */
    void NegativeResponse(uint32_t can_id, uint8_t sid, uint8_t nrc);

    /**
     * @brief Requests download.
     * @param can_id The CAN ID of the frame.
     * @param data_format_identifier The data format identifier.
     * @param memory_address The memory address.
     * @param memory_size The memory size.
     */
    void RequestDownload(uint32_t can_id, uint8_t data_format_identifier, uint8_t memory_address, uint8_t memory_size);

    /**
     * @brief Sends request download response frame.
     * @param can_id The CAN ID of the frame.
     * @param max_number_block The maximum number of blocks.
     */
    void RequestDownloadResponse(uint32_t can_id, uint8_t max_number_block);

    /**
     * @brief Transfers data.
     * @param can_id The CAN ID of the frame.
     * @param block_sequence_counter The block sequence counter.
     * @param transfer_request The transfer request (default: empty vector).
     */
    void TransferData(uint32_t can_id, uint8_t block_sequence_counter, std::vector<uint8_t> transfer_request = {});

    /**
     * @brief Transfers data (long format).
     * @param can_id The CAN ID of the frame.
     * @param block_sequence_counter The block sequence counter.
     * @param transfer_request The transfer request.
     * @param first_frame Indicates if it is the first frame (default: true).
     */
    void TransferDataLong(uint32_t can_id, uint8_t block_sequence_counter, std::vector<uint8_t> transfer_request, bool first_frame = true);

    /**
     * @brief Requests transfer exit.
     * @param can_id The CAN ID of the frame.
     * @param response Indicates if it is a response frame (default: false).
     */
    void RequestTransferExit(uint32_t can_id, bool response = false);

private:
    int socket = -1;
    struct sockaddr_can addr;
    struct ifreq ifr;

    /**
     * @brief Inserts bytes into the data vector at the specified index.
     * @param data The data vector.
     * @param index The index to insert the bytes.
     * @param num_bytes The number of bytes to insert.
     */
    void InsertBytes(std::vector<uint8_t> &data, uint8_t index, uint8_t num_bytes);

    /**
     * @brief Counts the number of digits in a number.
     * @param number The number.
     * @return Returns the number of digits.
     */
    int CountDigits(int number);
};

#endif
