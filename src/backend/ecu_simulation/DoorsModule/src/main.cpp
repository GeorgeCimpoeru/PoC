#include "../include/DoorsModule.h"

int main() {
    #ifdef UNIT_TESTING_MODE
    doorsModuleLogger = new Logger;
    #else
    doorsModuleLogger = new Logger("doorsModuleLogger", "logs/doorsModuleLogger.log");
    #endif /* UNIT_TESTING_MODE */
    doors = new DoorsModule();
    std::thread receiveFrThread([]()
                               { doors->_ecu->startFrames(); });
    sleep(200);
    receiveFrThread.join();
    return 0;
}