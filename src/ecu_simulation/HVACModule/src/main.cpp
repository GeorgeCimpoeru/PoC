#include "../include/HVACModule.h"

int main()
{
    #ifdef UNIT_TESTING_MODE
    hvacModuleLogger = new Logger;
    #else
    hvacModuleLogger = new Logger("hvacModuleLogger", "logs/hvacModuleLogger.log");
    #endif

    HVACModule hvac;
    hvac.fetchHvacData();
    hvac.printHvacInfo();
    std::thread receiveFrThread([&hvac]()
                               { hvac._ecu->startFrames(); });
    sleep(200);
    hvac._ecu->stopFrames();
    receiveFrThread.join();
    return 0;
}