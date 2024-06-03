#include "../include/BatteryModule.h"

int main() {
    BatteryModule batteryModule;
    std::thread receiveFrThread([&batteryModule]()
                               { batteryModule.receiveFrames(); });
    sleep(8);
    batteryModule.stopFrames();
    receiveFrThread.join();
    return 0;
}
