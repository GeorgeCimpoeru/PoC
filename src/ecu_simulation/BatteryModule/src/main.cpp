#include "src/ecu_simulation/BatteryModule/include/BatteryModule.h"

int main() {
    #ifdef UNIT_TESTING_MODE
    batteryModuleLogger = new Logger;
    #else
    batteryModuleLogger = new Logger("batteryModuleLogger", "logs/batteryModuleLogger.log");
    #endif /* UNIT_TESTING_MODE */
    battery = new BatteryModule(0x00,0x11);
    battery->fetchBatteryData();
    std::thread receiveFrThread([]()
                               { battery->receiveFrames(); });
    sleep(200);
    battery->stopFrames();
    receiveFrThread.join();
    return 0;
}
