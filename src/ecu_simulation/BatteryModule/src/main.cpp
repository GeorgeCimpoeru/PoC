#include "../include/BatteryModule.h"

int main() {
    BatteryModule batteryModule;
    batteryModule.simulate();
    batteryModule.fetchBatteryData();
    batteryModule.receiveFrames();
    batteryModule.stopFrames();
    return 0;
}
