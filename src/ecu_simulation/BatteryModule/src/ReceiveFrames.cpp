/*Author:Stoisor Miruna, 2024*/

#include "../include/ReceiveFrames.h"
#include "../include/HandleFrames.h"

ReceiveFrame::ReceiveFrame(int socket, int moduleID) {
    if (socket >= 0)
    {
        this->socket = socket;
        return;
    }
    std::cout<<"Error: Pass a valid Socket\n";
    exit(EXIT_FAILURE);

    const int MIN_VALID_ID = 0x00000000;
    const int MAX_VALID_ID = 0x7FFFFFFF;

    if (moduleID >= MIN_VALID_ID && moduleID <= MAX_VALID_ID) {
        this->moduleID = moduleID;
        return;
    }
    
    std::cout << "Error: Pass a valid Module ID\n";
    exit(EXIT_FAILURE);
}

bool ReceiveFrame::Receive(HandleFrames &handleFrame) {
    struct can_frame frame;
    int nbytes = read(this->socket, &frame, sizeof(struct can_frame));

    // Check if the received frame is valid and complete
    if (nbytes < 0) {
        std::cerr << "Read error\n";
        return false;
    } else if (nbytes < sizeof(struct can_frame)) {
        std::cerr << "Incomplete frame read\n";
        return false;
    }

    // Check if the received frame is for your module
    if (frame.can_id != this->moduleID) {
        std::cerr << "Received frame is not for this module\n";
        return false;
    }

    // Check if the received frame is empty
    if (frame.can_dlc == 0) {
        std::cerr << "Received empty frame\n";
        return false;
    }

    // Print the frame for debugging
    PrintFrame(frame);
    
    // Process the received frame
    handleFrame.ProcessReceivedFrame(frame);

    return true;
}

void ReceiveFrame::PrintFrame(const struct can_frame &frame) {
    std::cout << "Received CAN frame:" << std::endl;
    std::cout << "CAN ID: 0x" << std::hex << int(frame.can_id) << std::endl;
    std::cout << "Data Length: " << int(frame.can_dlc) << std::endl;
    std::cout << "Data:";
    for (int i = 0; i < frame.can_dlc; ++i) {
        std::cout << " 0x" << std::hex << int(frame.data[i]);
    }oid HandleFrames::ProcessReceivedFrame(const struct can_frame &frame) {
    int id = frame.can_id;
    std::vector<int> data(frame.data, frame.data + frame.can_dlc);

    // Extract the Service Identifier (SID)
    int sid = data[1];

    switch (sid) {
        //UDS
        case 0x10: // SessionControl method --to be implemented
            // int sub_function = data[2];
            // int resp = 0;
            // if (sub_function == 0x01){
            //     resp = //SessionControlDefault(id, data[2]);
            // } else if (sub_function == 0x02){
            //     resp = //SessionControlProgramming(id, data[2]);
            // } else {
            //     std::cerr << "Unknown sub_function: " << std::hex << sub_function << std::dec << "\n";
            // }
            //SessionControl.SendFrame(id, resp);
            break;



