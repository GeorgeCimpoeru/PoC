/*
    The library facilitates the transmission of Controller Area Network (CAN) 
    frames through an interface utilizing sockets.
    The libary also give some methods for creation of specific 
    frames for the main services.
    How to use example:
        GenerateFrames g1 = GenerateFrames(socket);
        int x[] = {0x11,0x34,0x56};
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
        //For custom frames
        bool SendFrame(int id, std::vector<int> data);
        //Predefine frames for services
        //UDS
        void SessionControl(int id, int sub_function, bool response=false);
        void EcuReset(int id, bool response=false);
        //Consider using method ReadDataByIdentifierLongResponse(), if the response
        //is greater than 4 bytes, to split the response into multiple frames.
        //Example:
        //if (response.size() > 4)
        //{
        //  gf.ReadDataByIdentifierLongResponse(0x0,0x0,response,true); //First frame
        //  // !!!! IMPORTANT: Wait for the Flow control frame from the Client !!!!
        //  gf.ReadDataByIdentifierLongResponse(0x0,0x0,response,false); //Remainig frames
        //}
        //PS: same for WriteDataByIdentifier()
        void ReadDataByIdentifier(int id,int identifier, std::vector<int> response = {});
        void ReadDataByIdentifierLongResponse(int id,int identifier, std::vector<int> response = {}, bool first_frame = true);
        void FlowControlFrame(int id);
        void AuthenticationRequestSeed(int id, bool response=false, const std::vector<int> &seed = {});
        void AuthenticationSendKey(int id, std::vector<int> &key, bool response=false);
        void RoutineControl(int id, bool response=false);
        void TesterPresent(int id, bool response=false);
        void ReadMemoryByAdress(int id, int memory_size, int memory_address, std::vector<int> response = {});
        void WriteDataByIdentifier(int id, int identifier, std::vector<int> data_parameter = {});
        void WriteDataByIdentifierLongData(int id, int identifier, std::vector<int> data_parameter = {}, bool first_frame = true);
        void ReadDtcInformation(int id, int sub_function, int dtc_status_mask, bool response);
        void ClearDiagnosticInformation(int id, std::vector<int> group_of_dtc = {0xFF,0xFF,0xFF}, bool response=false);
        void AccessTimingParameters(int id, bool response=false);
        void NegativeResponse(int id, int nrc, bool response=false);
        //OTA
        void RequestDownload(int id, int data_format_identifier, int memory_address, int memory_size, int max_number_block = 0);
        void TransferData(int id, int block_sequence_counter, int transfer_request, bool response=false);
        void RequestTransferExit(int id, int transfer_request, bool response=false);
        bool RequestUpdateStatus(int id, bool response=false);

    private:
        void AddSocket(int socket);
        struct can_frame CreateFrame(int &id, std::vector<int> &ata);
        int numDigits(int number);
        void insertBytes(std::vector<int>& byteVector, unsigned int num, int numBytes);
        void GenerateFrameLongData(int id, int sid, int identifier, std::vector<int> response, bool first_frame);
};

#endif