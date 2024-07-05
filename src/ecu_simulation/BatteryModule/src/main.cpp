#include "../include/BatteryModule.h"

int main() {
    sleep(5);
    BatteryModule batteryModule;
    batteryModule.fetchBatteryData();
    std::thread receiveFrThread([&batteryModule]()
                               { batteryModule.receiveFrames(); });
    sleep(8);
    batteryModule.stopFrames();
    receiveFrThread.join();
    return 0;
}
