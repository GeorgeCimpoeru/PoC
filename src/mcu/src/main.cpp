#include "../include/MCUModule.h"

int main() {

    #ifndef UNIT_TESTING_MODE
    MCULogger = new Logger("MCULogger", std::string(PROJECT_PATH) + "/src/mcu/logs/MCULogs.log");
    #else
    MCULogger = new Logger;
    #endif /* UNIT_TESTING_MODE */
    MCU::mcu = new MCU::MCUModule(0x01);
    MCU::mcu->StartModule();
    std::thread receiveFrThread([]()
    { 
        MCU::mcu->recvFrames();
    });
    sleep(3);
    /* commented this as we don't want it to stop, it was for test purposes */
    /* MCU::mcu->StopModule(); */
    receiveFrThread.join();
    return 0;
}