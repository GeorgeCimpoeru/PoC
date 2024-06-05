#include "../include/BatteryModule.h"

int main() {
    BatteryModule batteryModule;
    batteryModule.fetchBatteryData();
    batteryModule.receiveFrames();
    batteryModule.stopFrames();
    return 0;
}
