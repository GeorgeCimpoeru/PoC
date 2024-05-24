#include <iostream>
#include <thread>
#include "../include/BatteryModule.h"

int main() {
    BatteryModule batteryModule;
    // BatteryModule batteryModule(1, 0x102);   // this will call the parameterized constructor of battery class
                                                // and the interface-name will be 'vcan1', with moduleId '0x102' (258)
    batteryModule.simulate();
    batteryModule.receiveFrames();
    batteryModule.stopFrames();
    return 0;
}