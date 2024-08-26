#include "../include/HVACModule.h"

int main()
{
    #ifdef UNIT_TESTING_MODE
    hvacModuleLogger = new Logger;
    #else
    hvacModuleLogger = new Logger("hvacModuleLogger", "logs/hvacModuleLogger.log");
    #endif

    HVACModule hvac;
    
    return 0;
}