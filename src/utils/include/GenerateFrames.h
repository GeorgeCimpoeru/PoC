/**
 * @file GenerateFrames.h
 * @author Mujdei Ruben & Alexandru Doncea
 * @brief The library facilitates the transmission of Controller Area Network (CAN) 
 * frames through an interface utilizing sockets.
 * The library also gives some methods for the creation of specific 
 * frames for the main services.
 * How to use example:
 *     GenerateFrames g1 = GenerateFrames(socket);
 *     std::vector<uint8_t> x = {0x11, 0x34, 0x56};
 *     g1.SendFrame(0x23, x);
 *     g1.SessionControl(0x34A, 0x1);
 * @version 0.1
 * @date 2024-05-27
 * @copyright Copyright (c) 2024
 */
#ifndef POC_INCLUDE_GENERATEFRAMES_H_
#define POC_INCLUDE_GENERATEFRAMES_H_

#include <iostream>
#include <cstdlib>
#include <vector>

#include <unistd.h>
#include <linux/can.h>
#include "Logger.h"
/* Enumeration for frame types */
enum FrameType {
    DATA_FRAME,
    REMOTE_FRAME,
    ERROR_FRAME,
    OVERLOAD_FRAME
};

class GenerateFrames
{
    private:
        Logger& logger;
        int socket = -1;
    public:
        GenerateFrames(Logger& logger);
  
        /**
         * @brief Construct a new Generate Frames object
         * 
         * @param socket The socket descriptor used for communication over the CAN bus.
         * @param logger A logger instance used to record information and errors during the execution.
         */
        GenerateFrames(int socket, Logger& logger);
        /**
         * @brief Method to get the socket
         * 
         * @return Returns an integer socket
         */
        int getSocket();
        /**
         * @brief method to send response back to API
         * 
         * @param api_id identifier for the API that will send the frame.
         * @param sid service id.
         * @param battery_id id of the battery component.
         * @param doors_id id of the doors component.
         * @param engine_id id of the engine component.
        */ 
        void apiResponse(uint32_t api_id, uint8_t sid, uint8_t battery_id, uint8_t doors_id, uint8_t engine_id);
        /**
         * @brief Method for creation of custom frames
         * 
         * @param id id of the frame
         * @param data data to be sent through CAN
         * @param s socket needed for sendFrame
         * @param frameType default value: DATA_FRAME. More values: REMOTE_FRAME, ERROR_FRAME
         */
        int sendFrame(int can_id, std::vector<uint8_t> data, int s, FrameType frameType = DATA_FRAME);
        /**
         * @brief Method for creation of custom frames
         * 
         * @param id id of the frame
         * @param data data to be sent through CAN
         * @param frameType default value: DATA_FRAME. More values: REMOTE_FRAME, ERROR_FRAME
         * @return Returns a boolean if the CAN frame was send or not
         */
        bool sendFrame(int id, std::vector<uint8_t > data, FrameType frameType = DATA_FRAME);
        /**
         * !!! IMPORTANT: FOR ALL METHODS: !!!
         * Most of the methods can be used to send a request or a response frame 
         * (a commentary 'Response&Request' before each method specifies if it 
         * can be used as a response and request method) by setting:
         * - The response variable as true or false. Implicitly false if the variable 
         *   is not defined.
         * OR
         * - Adding some specific data. If not added, the frame is not a response/request.
         *   Ex: readDataByIdentifier(id, identifier, response);  <- Is a response because 
         *                                                            the response variable has values
         *       readDataByIdentifier(id, identifier);  <- Is a request because the response 
         *                                                 variable is empty
         * OR
         * void transferData(id, block_sequence_counter, transfer_request);  <- Is a response 
         *                                                                     because the transfer_request 
         *                                                                     variable has values
         * void transferData(id, block_sequence_counter);  <- Is a request because the transfer_request 
         *                                                    variable is empty
         * OBS: Not all the methods have this variable
         * PS: For more details of how the frames are formed and the data needed to create them
         * please check the ISO file for the Services.
         */
        /**
         * @brief Frame for Session Control Service
         * 
         * @param id id of the frame(sender id and receiver id)
         * @param sub_function subfunction of the service
         * @param response variable for request or response frame
         * Response&Request
        */
        void sessionControl(int id, uint8_t sub_function, bool response=false);
        /**
         * @brief Frame for ECU reset Service
         * 
         * @param id id of the frame(sender id and receiver id)
         * @param sub_function the sub_function of the ECU Reset
         * @param response variable for request or response frame
         * Response&Request
         */
        void ecuReset(int id, uint8_t sub_function, bool response=false);
        /**
         * @brief Frame for ECU reset Service
         * 
         * @param id id of the frame(sender id and receiver id)
         * @param sub_function the sub_function of the ECU Reset
         * @param socket socket used to send the frame through the bus
         * @param response variable for request or response frame
         */
        void ecuReset(int id, uint8_t sub_function, int socket, bool response=false);
        /**
         * @brief Frame for Read data by Identifier Service
         * Consider using the method readDataByIdentifierLongResponse(), if the response
         * is longer than 5 bytes, to split the response into multiple frames.
         * Example:
         *     if (response.size() > 5)
         *     {
         *         gf.readDataByIdentifierLongResponse(0x0, 0x0, response, true); // First frame
         *         // !!!! IMPORTANT: Wait for the Flow control frame from the Client !!!!
         *         gf.readDataByIdentifierLongResponse(0x0, 0x0, response, false); // Remaining frames
         *     }
         *     else
         *     {
         *         gf.readDataByIdentifier(0x0, 0x0, response);
         *     }
         * PS: Same for writeDataByIdentifier(), readMemoryByAddress() and transferDataLong()
         * but different size bytes.
         * 
         * @param id id of the frame(sender id and receiver id)
         * @param identifier identifier of the data to be read
         * @param response the data that is sent (if not set, the frame is a request)
         * Response&Request
         */
        void readDataByIdentifier(int id, int identifier, std::vector<uint8_t> response = {});
        /**
         * @brief Frame for Read data by Identifier Service. Use this method when the response need to be send through more frames
         * 
         * @param id id of the frame(sender id and receiver id)
         * @param identifier identifier of the data to be read
         * @param response the data that is sent 
         * @param first_frame set as true if it is the first frame (default) or false for the rest of the frames.
         */
        void readDataByIdentifierLongResponse(int id, uint16_t identifier, std::vector<uint8_t> response, bool first_frame = true);
        /**
         * @brief This frame is sent as a response to a FirstFrame
         * 
         * @param id id of the frame(sender id and receiver id)
         */
        void flowControlFrame(int id);
        /**
         * @brief Frame for Authentication Service, sub-function 0x01(Request seed)
         * 
         * @param id id of the frame(sender id and receiver id)
         * @param seed the requested seed(if not set the frame is a request)
        Response&Request */
        void securityAccessRequestSeed(int id, const std::vector<uint8_t>& seed = {});
        /**
         * @brief Frame for Authentication Service, sub-function 0x01(Request seed)
         * 
         * @param id id of the frame(sender id and receiver id)
         * @param key if not set, the frame is a response
        Response&Request */
        void securityAccessSendKey(int id, const std::vector<uint8_t>& key = {});
        /**
         * @brief Frame for Routine Control Service
         * 
         * @param id id of the frame(sender id and receiver id)
         * @param sub_function 0x01,0x02,0x03(START, STOP, RESET)
         * @param routine_identifier Identifier of the routine to be run
         * @param response variable for request or response frame
        Response&Request */
        void routineControl(int id, uint8_t sub_function, uint16_t routine_identifier, bool response=false);
        /**
         * @brief Frame for Tester Present Service
         * 
         * @param id id of the frame(sender id and receiver id)
         * @param response variable for request or response frame
         * Response&Request
         */
        void testerPresent(int id, bool response=false);
        /**
         * @brief Frame for Read data by Address. Check the commentary from the readDataByIdentifier() method
         * 
         * @param id id of the frame(sender id and receiver id)
         * @param memory_size Memory size variable
         * @param memory_address Memory address variable
         * @param response variable for request or response frame
         * Response&Request
         */
        void readMemoryByAddress(int id, int memory_address, int memory_size, std::vector<uint8_t> response = {});
        /**
         * @brief Frame for Read data by Address. Check the commentary from the readDataByIdentifier() method
         * 
         * @param id id of the frame(sender id and receiver id)
         * @param memory_size Memory size variable
         * @param memory_address Memory address variable
         * @param response variable for request or response frame
         * @param first_frame set as true if it is the first frame (default) or false for the rest of the frames.
         */
        void readMemoryByAddressLongResponse(int id, int memory_address, int memory_size, std::vector<uint8_t> response, bool first_frame = true);
        /**
         * @brief Frame for Write data by Identifier. Check the commentary from the readDataByIdentifier() method
         * 
         * @param id id of the frame(sender id and receiver id)
         * @param identifier identifier of the data to be read
         * @param data_parameter data to be write in the identifier
         * Response&Request
         */
        void writeDataByIdentifier(int id, uint16_t identifier, std::vector<uint8_t> data_parameter = {});
        /**
         * @brief Frame for Write data by Identifier. Check the commentary from the readDataByIdentifier() method
         * 
         * @param id id of the frame(sender id and receiver id)
         * @param identifier identifier of the data to be read
         * @param data_parameter data to be write in the identifier
         * @param first_frame set as true if it is the first frame (default) or false for the rest of the frames.
         */
        void writeDataByIdentifierLongData(int id, uint16_t identifier, std::vector<uint8_t> data_parameter, bool first_frame = true);
        /**
         * @brief Frame for Read DTC Information Request
         * 
         * @param id id of the frame(sender id and receiver id)
         * @param sub_function can be use only for subfunction 'Number of DTCs'(17/6/2024)
         * @param dtc_status_mask DTC status Mask variable
         */
        void readDtcInformation(int id, uint8_t sub_function, uint8_t dtc_status_mask);
        /**
         * @brief Response for readDTCInformation service, sub_function 0x01
         * 
         * @param id id of the frame(sender id and receiver id)
         * @param status_availability_mask Status availability mask variable
         * @param dtc_format_identifier DTC format identifier variable
         * @param dtc_count Number of dtc's found
         */
        void readDtcInformationResponse01(int id, uint8_t status_availability_mask, uint8_t dtc_format_identifier, uint16_t dtc_count);
        /**
         * @brief Response for readDTCInformation service, sub_function 0x02
         * 
         * @param id id of the frame(sender id and receiver id)
         * @param status_availability_mask Status availability mask variable
         * @param dtc Specific Data trouble code(DTC)
         * @param status_mask Status mask of the dtc
         */
        void readDtcInformationResponse02(int id, uint8_t status_availability_mask, std::vector<std::pair<int,int>> dtc_and_status_list);
        /**
         * @brief Response for readDTCInformation service, sub_function 0x02
         * 
         * @param id id of the frame(sender id and receiver id)
         * @param status_availability_mask Status availability mask variable
         * @param dtc_and_status_list list of pairs of dtc and its status mask
         * @param first_frame set as true if it is the first frame (default) or false for the rest of the frames.
         */
        void readDtcInformationResponse02Long(int id, uint8_t status_availability_mask, std::vector<std::pair<int,int>> dtc_and_status_list, bool first_frame = true);
        /**
         * @brief Frame for Clear Diagnostic Information Service
         * 
         * @param id id of the frame(sender id and receiver id)
         * @param group_of_dtc if not set, the default value(FFFFFF) clear all DTCs
         * @param response variable for request or response frame
         Response&Request
         */
        void clearDiagnosticInformation(int id, std::vector<uint8_t> group_of_dtc = {0xFF, 0xFF, 0xFF}, bool response=false);
        /**
         * @brief Frame for Access timing parameters
         * 
         * @param id id of the frame(sender id and receiver id)
         * @param sub_function Value of the sub function for the service
         * @param data_parameter data to be write in the identifier
         * @param response variable for request or response frame
         * Response&Request
         */
        void accessTimingParameters(int id, uint8_t sub_function,std::vector<uint8_t> data_parameter, bool response=false);
        /**
         * @brief Method to manage the negative response
         * 
         * @param id id of the frame(sender id and receiver id)
         * @param sid sid of the service id for which the Negative response is send
         * @param nrc Negative response code
         */
        void negativeResponse(int id, uint8_t sid, uint8_t nrc);
        /*
         * OTA Services
         * The next methods create and send frames for the specific services
        */
        /**
         * @brief Frame for Request Download
         * 
         * @param id id of the frame(sender id and receiver id)
         * @param data_format_identifier Data format identifier variable
         * @param memory_address Memory address variable
         * @param memory_size Memory size variable
         * @param download_type The type of download: manual or automatic
         */
        void requestDownload(int id, uint8_t data_format_identifier, int memory_address, int memory_size, uint8_t download_type);
        /**
         * @brief Frame for Request Download long
         * 
         * @param id id of the frame(sender id and receiver id)
         * @param data_format_identifier Data format identifier variable
         * @param memory_address Memory address variable
         * @param memory_size Memory size variable
         * @param download_type The type of download: manual or automatic
         */
        void requestDownloadLong(int id, uint8_t data_format_identifier, int memory_address, int memory_size, uint8_t download_type, bool first_frame = true);
        /**
         * @brief Response to requestDownload
         * 
         * @param id id of the frame(sender id and receiver id)
         * @param max_number_block Max number of blocks variable
         */        
        void requestDownloadResponse(int id, int max_number_block);
        /**
         * @brief Frame for Transfer Data service. Check the commentary from the readDataByIdentifier() method
         * 
         * @param id id of the frame(sender id and receiver id)
         * @param block_sequence_counter Block sequence counter variable
         * @param transfer_request Data to be transfer
         * Response&Request
         */
        void transferData(int id, uint8_t block_sequence_counter, std::vector<uint8_t> transfer_request = {});
        /**
         * @brief Frame for Transfer Data service. Check the commentary from the readDataByIdentifier() method
         * 
         * @param id id of the frame(sender id and receiver id)
         * @param block_sequence_counter Block sequence counter variable
         * @param transfer_request Data to be transfer
         * @param first_frame set as true if it is the first frame (default) or false for the rest of the frames.
         */
        void transferDataLong(int id, uint8_t block_sequence_counter, std::vector<uint8_t> transfer_request, bool first_frame = true);
        /**
         * @brief Frame for Request transfer exit service
         * 
         * @param id id of the frame(sender id and receiver id)
         * @param response variable for request or response frame
         * Response&Request
         */
        void requestTransferExit(int id, bool response=false);
        /**
         * @brief NOT IMPLEMENTED!!
         */
        bool requestUpdateStatusResponse(int id, std::vector<uint8_t> response);
    private:
        /**
         * @brief Set the socket parameter
         * 
         * @param socket The socket descriptor used for communication over the CAN bus.
         */
        void addSocket(int socket);
        /**
         * @brief Create a Frame object
         * 
         * @param id if of the frame
         * @param data data to be put in the frame
         * @param frameType type of frame
         * @return struct can_frame 
         */
        struct can_frame createFrame(int& id, std::vector<uint8_t>& data, FrameType frameType = DATA_FRAME);
        /**
         * @brief Count the number of digits in a number
         * 
         * @param number The integer whose digits are to be counted.
         * @return The number of digits for a given integer.
         */
        int countDigits(int number);
        /**
         * @brief Insert in a std::vector a number but separated in bytes
         * 
         * @param[out] byteVector Vector in which the data will be inserted
         * @param num number to be inserted
         * @param numBytes the number of bytes to extract from the integer num and inserted into bytevector.
         */
        void insertBytes(std::vector<uint8_t>& byteVector, unsigned int num, int numBytes);
        /**
         * @brief Universal class used by long-frame-methods to generate consecutive frames
         * 
         * @param id id of the frame(sender id and receiver id)
         * @param data Data to be put in to the frame
         * @param first_frame set as true if it is the first frame (default) or false for the rest of the frames.
         */
        void GenerateConsecutiveFrames(int id, std::vector<uint8_t> data, bool first_frame);
};

#endif