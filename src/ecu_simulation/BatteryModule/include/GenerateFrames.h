/*
    The library facilitates the transmission of Controller Area Network (CAN) 
    frames through an interface utilizing sockets.
    The libary also give some methods for creation of specific 
    frames for the main services.
    How to use example:
        GenerateFrames g1 = GenerateFrames(socket);
        std::vector<int> x = {0x11,0x34,0x56};
        g1.SendFrame(0x23,3,x);
        g1.SessionControl(0x34A,0x1);

    Author:Mujdei Ruben, 2024
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
        GenerateFrames(int socket);
        int getSocket();
        //For custom frames
        bool sendFrame(int id, std::vector<int> data);
        //Predefine frames for services
        //UDS
        void sessionControl(int id, int sub_function, bool response=false);
        void ecuReset(int id, bool response=false);
        //Consider using method ReadDataByIdentifierLongResponse(), if the response
        //is greater than 5 bytes, to split the response into multiple frames.
        //Example:
        //if (response.size() > 5)
        //{
        //  gf.ReadDataByIdentifierLongResponse(0x0,0x0,response,true); //First frame
        //  // !!!! IMPORTANT: Wait for the Flow control frame from the Client !!!!
        //  gf.ReadDataByIdentifierLongResponse(0x0,0x0,response,false); //Remainig frames
        //}
        //PS: same for WriteDataByIdentifier() and ReadMemoryByAddress()
        void readDataByIdentifier(int id,int identifier, std::vector<int> response = {});
        void readDataByIdentifierLongResponse(int id,int identifier, std::vector<int> response = {}, bool first_frame = true);
        void flowControlFrame(int id);
        void authenticationRequestSeed(int id, const std::vector<int> &seed = {});
        void authenticationSendKey(int id, const std::vector<int> &key = {});
        void routineControl(int id, int sub_function, int routin_identifier, bool response=false);
        void testerPresent(int id, bool response=false);
        void readMemoryByAdress(int id, int memory_size, int memory_address, std::vector<int> response = {});
        void readMemoryByAdressLongResponse(int id, int memory_size, int memory_address, std::vector<int> response = {}, bool first_frame = true);
        void writeDataByIdentifier(int id, int identifier, std::vector<int> data_parameter = {});
        void writeDataByIdentifierLongData(int id, int identifier, std::vector<int> data_parameter = {}, bool first_frame = true);
        void readDtcInformation(int id, int sub_function, int dtc_status_mask);
        void readDtcInformationResponse01(int id, int status_availability_mask, int dtc_format_identifier, int dtc_count);
        void clearDiagnosticInformation(int id, std::vector<int> group_of_dtc = {0xFF,0xFF,0xFF}, bool response=false);
        void accessTimingParameters(int id, int sub_function, bool response=false);
        void negativeResponse(int id, int nrc);
        //OTA
        void requestDownload(int id, int data_format_identifier, int memory_address, int memory_size);
        void requestDownloadResponse(int id, int max_number_block);
        void transferData(int id, int block_sequence_counter, std::vector<int> transfer_request = {});
        void transferDataLong(int id, int block_sequence_counter, std::vector<int> transfer_request = {}, bool first_frame = true);
        void requestTransferExit(int id, bool response=false);
        bool requestUpdateStatus(int id, bool response=false);
    private:
        void addSocket(int socket);
        struct can_frame createFrame(int &id, std::vector<int> &ata);
        int numDigits(int number);
        void insertBytes(std::vector<int>& byteVector, unsigned int num, int numBytes);
        void generateFrameLongData(int id, int sid, int identifier, std::vector<int> response, bool first_frame);
};

#endif