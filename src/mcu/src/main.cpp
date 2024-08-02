#include "../include/MCUModule.h"

int main() {

    #ifndef UNIT_TESTING_MODE
    MCULogger = new Logger("MCULogger", "logs/MCULogs.log");
    #else
    MCULogger = new Logger;
    #endif /* UNIT_TESTING_MODE */
    MCU::mcu = new MCU::MCUModule(0x01);
    MCU::mcu->StartModule();
    MCU::mcu->recvFrames();
    return 0;
}