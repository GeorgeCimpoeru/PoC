#include "src/ecu_simulation/BatteryModule/include/BatteryModule.h"

int main() {
    sleep(5);
    BatteryModule batteryModule(0x00,0x11);
    batteryModule.fetchBatteryData();
    std::thread receiveFrThread([&batteryModule]()
                               { batteryModule.receiveFrames(); });
    sleep(200);
    batteryModule.stopFrames();
    receiveFrThread.join();
    return 0;
}
