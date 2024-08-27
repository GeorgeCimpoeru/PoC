#include "../include/DoorsModule.h"

int main() {
    #ifdef UNIT_TESTING_MODE
    doorsModuleLogger = new Logger;
    #else
    doorsModuleLogger = new Logger("doorsModuleLogger", "logs/doorsModuleLogger.log");
    #endif /* UNIT_TESTING_MODE */
    doors = new DoorsModule(0x13,0x13);
    doors->fetchDoorsData();
    std::thread receiveFrThread([]()
                               { doors->receiveFrames(); });
    sleep(200);
    doors->stopFrames();
    receiveFrThread.join();
    return 0;
}
