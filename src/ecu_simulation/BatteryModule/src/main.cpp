#include "../include/BatteryModule.h"

int main() {
    BatteryModule batteryModule;
    batteryModule.simulate();
    batteryModule.receiveFrames();
    batteryModule.stopFrames();
    return 0;
}
