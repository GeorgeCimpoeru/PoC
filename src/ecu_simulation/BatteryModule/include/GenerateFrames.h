/**
 * @file GenerateFrames.h
 * @author Mujdei Ruben
 * @brief The library facilitates the transmission of Controller Area Network (CAN) 
 * frames through an interface utilizing sockets.
 * The library also gives some methods for the creation of specific 
 * frames for the main services.
 * How to use example:
 *     GenerateFrames g1 = GenerateFrames(socket);
 *     std::vector<int> x = {0x11, 0x34, 0x56};
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

class GenerateFrames
{
    private:
        int socket = -1;
    public:
        /**
         * @brief Construct a new Generate Frames object
         * 
         * @param socket 
         */
        GenerateFrames(int socket);
        int getSocket();
        /**
         * Method creation of custom frames
         * @param id 
         * @param data 
         */
        bool sendFrame(int id, std::vector<int> data);
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
        /*
         * UDS Services
         * The next methods create and send frames for the specific services.
         */
        /* Response&Request */
        void sessionControl(int id, int sub_function, bool response=false);
        /**
         * @param id 
         * @param response 
         * Response&Request
         */
        void ecuReset(int id, bool response=false);
        /**
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
         * @param id
         * @param identifier
         * @param response the data that is sent (if not set, the frame is a request)
         * Response&Request
         */
        void readDataByIdentifier(int id, int identifier, std::vector<int> response = {});
        /**
         * @param id
         * @param identifier
         * @param response the data that is sent 
         * @param first_frame: set as true if it is the first frame (default) or false for the rest of the frames.
         */
        void readDataByIdentifierLongResponse(int id, int identifier, std::vector<int> response, bool first_frame = true);
        /* This frame is sent as a response to a FirstFrame */
        void flowControlFrame(int id);
        /**
         * @param id
         * @param seed the requested seed(if not set the frame is a request)
        Response&Request */
        void authenticationRequestSeed(int id, const std::vector<int>& seed = {});
        /**
         * @param id
         * @param key if not set, the frame is a response
        Response&Request */
        void authenticationSendKey(int id, const std::vector<int>& key = {});
        /**
         * @param id
         * @param sub_function 0x01,0x02(START, STOP
         * @param routine_identifier
         * @param identifier the identifier of the routine
        Response&Request */
        void routineControl(int id, int sub_function, int routine_identifier, bool response=false);
        /**
         * @param id 
         * @param response 
         * Response&Request
         */
        void testerPresent(int id, bool response=false);
        /**
         * Check the commentary from the readDataByIdentifier() method
         * @param id 
         * @param memory_size 
         * @param memory_address 
         * @param response 
         * Response&Request
         */
        void readMemoryByAddress(int id, int memory_size, int memory_address, std::vector<int> response = {});
        /**
         * Check the commentary from the readDataByIdentifier() method
         * @param id 
         * @param memory_size 
         * @param memory_address 
         * @param response 
         * @param first_frame 
         */
        void readMemoryByAddressLongResponse(int id, int memory_size, int memory_address, std::vector<int> response, bool first_frame = true);
        /**
         * Check the commentary from the readDataByIdentifier() method
         * @param id 
         * @param identifier 
         * @param data_parameter 
         * Response&Request
         */
        void writeDataByIdentifier(int id, int identifier, std::vector<int> data_parameter = {});
        /**
         * Check the commentary from the readDataByIdentifier() method
         * @param id 
         * @param identifier 
         * @param data_parameter 
         * @param first_frame 
         */
        void writeDataByIdentifierLongData(int id, int identifier, std::vector<int> data_parameter, bool first_frame = true);
        /**
         * @param id 
         * @param sub_function can be use only for subfunction 'Number of DTCs'
         * @param dtc_status_mask 
         */
        void readDtcInformation(int id, int sub_function, int dtc_status_mask);
        /**
         * @brief Response for readDTCINformation sub_function 0x01
         * 
         * @param id 
         * @param status_availability_mask 
         * @param dtc_format_identifier 
         * @param dtc_count 
         */
        void readDtcInformationResponse01(int id, int status_availability_mask, int dtc_format_identifier, int dtc_count);
        /**
         * @param id
         * @param group_of_dtc if not set, the default value(FFFFFF) clear all DTCs
         * @param response
         Response&Request
         */
        void clearDiagnosticInformation(int id, std::vector<int> group_of_dtc = {0xFF, 0xFF, 0xFF}, bool response=false);
        /**
         * @param id 
         * @param sub_function 
         * @param response 
         * Response&Request
         */
        void accessTimingParameters(int id, int sub_function, bool response=false);
        /**
         * @param id 
         * @param nrc 
         */
        void negativeResponse(int id, int nrc);
        /*
         * OTA Services
         * The next methods create and send frames for the specific services
        */
        /**
         * @param id 
         * @param data_format_identifier 
         * @param memory_address 
         * @param memory_size 
         */
        void requestDownload(int id, int data_format_identifier, int memory_address, int memory_size);
        /**
         * @brief Response to requestDownload
         * @param id 
         * @param max_number_block 
         */
        void requestDownloadResponse(int id, int max_number_block);
        /**
         * Check the commentary from the readDataByIdentifier() method
         * @param id 
         * @param block_sequence_counter 
         * @param transfer_request 
         * Response&Request
         */
        void transferData(int id, int block_sequence_counter, std::vector<int> transfer_request = {});
        /**
         * Check the commentary from the readDataByIdentifier() method
         * @param id 
         * @param block_sequence_counter 
         * @param transfer_request 
         * @param first_frame
         * Response&Request
         */
        void transferDataLong(int id, int block_sequence_counter, std::vector<int> transfer_request, bool first_frame = true);
        /**
         * @param id 
         * @param response
         * Response&Request
         */
        void requestTransferExit(int id, bool response=false);
        /*NOT IMPLEMENTED!!*/
        bool requestUpdateStatus(int id, bool response=false);
    private:
        /**
         * @brief Set the socket parameter
         * 
         * @param socket 
         */
        void addSocket(int socket);
        /**
         * @brief Create a Frame object
         * 
         * @param id 
         * @param data 
         * @return struct can_frame 
         */
        struct can_frame createFrame(int& id, std::vector<int>& data);
        /**
         * @brief Count the number of digits in a number
         * @param number
         */
        int countDigits(int number);
        /**
         * @brief Insert in a std::vector a number but separated in bytes
         * 
         * @param[out] byteVector Vector in which the data will be inserted
         * @param num number to be inserted
         * @param numBytes 
         */
        void insertBytes(std::vector<int>& byteVector, unsigned int num, int numBytes);
        /**
         * @brief Generate consecutive frames
         * 
         * @param id 
         * @param sid 
         * @param identifier 
         * @param response 
         * @param first_frame 
         */
        void generateFrameLongData(int id, int sid, int identifier, std::vector<int> response, bool first_frame);
};

#endif