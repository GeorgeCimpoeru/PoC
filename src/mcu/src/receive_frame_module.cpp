#include "../include/receive_frame_module.h"

ReceiveFrameModule::ReceiveFrameModule(int socket) : s(socket) {}

int ReceiveFrameModule::ReceiveFrameFromCANBus(){
    struct can_frame frame;
    while (true){
        int nbytes = read(s, &frame, sizeof(frame)); // Read frames
        if (nbytes < 0){
            std::cerr << "Read Error" << std::endl;
            return 1;
        }else if (nbytes == 0) {
            std::cerr << "No CAN frame received" << std::endl;
        } else{
            std::cout << "-------------------\n";
            std::cout << "Received CAN frame:" << std::endl;
            std::cout << "CAN ID: 0x" << std::hex << frame.can_id << std::endl;
            std::cout << "Data Length: " << std::dec << int(frame.can_dlc) << std::endl;
            std::cout << "Data: ";
            for (int i = 0; i < frame.can_dlc; ++i) {
                std::cout << std::hex << int(frame.data[i]) << " ";
            }
            std::cout << std::endl;

            // Compare the first data byte with hexValueId
            if (frame.can_id == hexValueId) {
                if (frame.data[0] == frame.can_dlc - 1) {
                    std::cout << "Frame matches hexValueId and data length code." << std::endl;
                    //handleFrame(frame)
                }
            } else {
                std::cout << "The frame was't read completely!";
            }
        }
    }
 return 0;

}