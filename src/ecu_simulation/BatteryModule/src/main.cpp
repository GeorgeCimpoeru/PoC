#include <iostream>
#include <thread>
#include "../include/BatteryModule.h"

int main() {
    BatteryModule batteryModule;
    batteryModule.simulate();
    batteryModule.receiveFrames();
    return 0;
}