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
        void ReadDataByIdentifier(int id,int identifier, std::vector<int> response = {});
        void AuthenticationRequestSeed(int id, bool response=false, const std::vector<int> &seed = {});
        void AuthenticationSendKey(int id, std::vector<int> &key, bool response=false);
        void RoutineControl(int id, bool response=false);
        void TesterPresent(int id, bool response=false);
        void ReadMemoryByAdress(int id, int address_and_length_identifier, int memory_size, int memory_address, std::vector<int> response = {});
        void WriteDataByIdentifier(int id, int identifier, std::vector<int> data_parameter = {});
        void ReadDtcInformation(int id, bool response=false);
        void ClearDiagnosticInfirmation(int id, bool response=false);
        void AccessTimingParameters(int id, bool response=false);
        void NegativeResponse(int id, int nrc, bool response=false);
        //OTA
        void GenerateFrames::RequestDownload(int id, int data_format_identifier, int address_length_identifier,
                                             int memory_address, int memory_size, bool response=false);
        void TransferData(int id, int block_sequence_counter, int transfer_request, bool response=false);
        void RequestTransferExit(int id, int transfer_request, bool response=false);
        bool RequestUpdateStatus(int id, bool response=false);

    public:
        void AddSocket(int socket);
        struct can_frame CreateFrame(int &id, std::vector<int> &ata);
};

#endif