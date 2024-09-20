
#include "../include/EngineModule.h"

int main() {
    #ifdef UNIT_TESTING_MODE
    engineModuleLogger = new Logger;
    #else
    engineModuleLogger = new Logger("engineModuleLogger", "logs/engineModuleLogger.log");
    #endif /* UNIT_TESTING_MODE */
    engine = new EngineModule();
    std::thread receiveFrThread([]()
                               { engine->_ecu->startFrames(); });
    sleep(200);
    receiveFrThread.join();
    return 0;
}
