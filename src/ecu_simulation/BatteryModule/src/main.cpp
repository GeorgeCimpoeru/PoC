#include "../include/BatteryModule.h"

int main() {
    sleep(5);
    BatteryModule batteryModule(0x00, 0x11); /* no arguments */
    batteryModule.fetchBatteryData();
    std::thread receiveFrThread([&batteryModule]()
                               { batteryModule.receiveFrames(); });
    sleep(300); /* sleep for 8 seconds */
    batteryModule.stopFrames();
    receiveFrThread.join();
    return 0;
}
