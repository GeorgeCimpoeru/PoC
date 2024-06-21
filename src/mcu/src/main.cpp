#include "../include/MCUModule.h"

int main() {
    MCUModule mcuModule(0x01);
    mcuModule.StartModule();
    mcuModule.recvFrames();
    return 0;
}