/**
 * GenerateFrame.h
 * For creating a CAN frame
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

/* Enumeration for frame types */
enum FrameType {
    DATA_FRAME,
    REMOTE_FRAME,
    ERROR_FRAME,
    OVERLOAD_FRAME
};

/* Class to create a CAN frame */
class GenerateFrame {
    public:
        /* Constructors */
        GenerateFrame(int socket);
        GenerateFrame();
        
        /* Function to send a CAN frame */
        int SendFrame(
            int can_id, 
            std::vector<int> data, 
            FrameType frameType = DATA_FRAME
        );

        /* Function to get the CAN frame */
        can_frame getFrame();

        /* Function to get the socket */
        int getSocket();
        struct can_frame frame;

        void SessionControl(int can_id, int subfunction, bool response = false);
        void EcuReset(int can_id, bool response = false);
        void ReadDataByIdentifier(int can_id, int dataIdentifier, std::vector <int> response = {});
        void ReadDataByIdentifierLong(int can_id, int sid, int dataIdentifier, std::vector <int> response = {}, bool firstFrame = true);
        void FlowControlFrame(int can_id);
        void AuthenticationRequestSeed(int can_id, const std::vector<int>& seed = {});
        void AuthenticationSendKey(int can_id, const std::vector<int>& key = {});
        void RoutineControl(int can_id, int subfunction, int routine_identifier, bool response = false);
        void TesterPresent(int can_id, bool response = false);
        void ReadMemoryByAddress(int can_id, int memory_size, int memory_address, std::vector<int> response = {});
        void ReadMemoryByAddressLong(int can_id, int memory_size, int memory_address, std::vector<int> response, bool first_frame = true);
        void WriteDataByIdentifier(int can_id, int identifier, std::vector<int> data_parameter = {});
        void WriteDataByIdentifierLongData(int can_id, int identifier, std::vector<int> data_parameter, bool first_frame = true);
        void ReadDtcInformation(int can_id, int subfunction, int dtc_status_mask);
        void ReadDtcInformationResponse01(int can_id, int status_availability_mask, int dtc_format_identifier, int dtc_count);
        void ClearDiagnosticInformation(int can_id, std::vector<int> group_of_dtc = {0xFF, 0xFF, 0xFF}, bool response = false);
        void AccessTimingParameters(int can_id, int subfunction, bool response = false);
        void NegativeResponse(int can_id, int sid, int nrc);
        void RequestDownload(int can_id, int data_format_identifier, int memory_address, int memory_size);
        void RequestDownloadResponse(int can_id, int max_number_block);
        void TransferData(int can_id, int block_sequence_counter, std::vector<int> transfer_request = {});
        void TransferDataLong(int id, int block_sequence_counter, std::vector<int> transfer_request, bool first_frame = true);
        void RequestTransferExit(int id, bool response=false);

        /* Function to create a CAN frame */
        can_frame CreateFrame(
            int can_id,
            std::vector<int> data,
            FrameType frameType = DATA_FRAME
        );

    private:
        int socket = -1;
        struct sockaddr_can addr;
        struct ifreq ifr;
};

#endif
