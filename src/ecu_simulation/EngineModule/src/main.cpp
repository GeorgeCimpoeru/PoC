
#include "../include/EngineModule.h"

int main() {
    #ifdef UNIT_TESTING_MODE
    engineModuleLogger = new Logger;
    #else
    engineModuleLogger = new Logger("engineModuleLogger", "logs/engineModuleLogger.log");
    #endif /* UNIT_TESTING_MODE */
    engine = new EngineModule(0x00,0x12);
    std::thread receiveFrThread([]()
                               { engine->receiveFrames(); });
    sleep(200);
    receiveFrThread.join();
    return 0;
}
