
#include "../include/EngineModule.h"

int main() {
    #ifdef UNIT_TESTING_MODE
    engineModuleLogger = new Logger;
    #else
    engineModuleLogger = new Logger("engineModuleLogger", "logs/engineModuleLogger.log");
    #endif /* UNIT_TESTING_MODE */
    engine = new EngineModule(0x00,0x12);
    engine->fetchEngineData();
    std::thread receiveFrThread([]()
                               { engine->receiveFrames(); });
    sleep(200);
    engine->stopFrames();
    receiveFrThread.join();
    return 0;
}
