#include "../include/MCUModule.h"

int main() {
    MCUModule mcuModule(0);
    mcuModule.StartModule();
    mcuModule.recvFrames();
    return 0;
}