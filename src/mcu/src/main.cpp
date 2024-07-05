#include "../include/MCUModule.h"

int main() {
    MCU::mcu.StartModule();
    MCU::mcu.recvFrames();
    return 0;
}