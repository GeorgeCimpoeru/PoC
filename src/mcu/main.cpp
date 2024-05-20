#include <iostream>
#include "include/CreateFrame.h"

int main() {
    // First Frame
    uint8_t data_1[8] = {0x91, 0x92, 0x93, 0x94, 0x95, 0x96, 0x97, 0x98};
    CANFrame frame_1(DATA_FRAME, 0x109, data_1, 8);

    // Send First Frame at vcan0
    if (frame_1.SendFrame("vcan0") != 0) {
        std::cerr << "Failed to send frame 1" << std::endl;
        return 1;
    }
    std::cout << "Frame 1 sent successfully" << std::endl;

    // Second Frame
    uint8_t data_2[8] = {0x89, 0x88, 0x87, 0x86, 0x85, 0x84, 0x83, 0x82};
    CANFrame frame_2(DATA_FRAME, 0x108, data_2, 8);

    // Send Second Frame at vcan0
    if (frame_2.SendFrame("vcan0") != 0) {
        std::cerr << "Failed to send frame 2" << std::endl;
        return 1;
    }
    std::cout << "Frame 2 sent successfully" << std::endl;

    return 0;
}
