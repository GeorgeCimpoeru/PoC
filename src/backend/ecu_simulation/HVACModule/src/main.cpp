#include "../include/HVACModule.h"

int main()
{
    #ifdef UNIT_TESTING_MODE
    hvacModuleLogger = new Logger;
    #else
    hvacModuleLogger = new Logger("hvacModuleLogger", "logs/hvacModuleLogger.log");
    #endif

    hvac = new HVACModule();
    hvac->printHvacInfo();
    std::thread receiveFrThread([]()
                               { hvac->_ecu->startFrames(); });
    sleep(200);
    receiveFrThread.join();
    return 0;
}