#include <iostream>
#include <thread>
#include "../include/BatteryModule.h"

int main() {
    BatteryModule batteryModule;
    std::thread simulationThread([&]() {
        batteryModule.simulate();
    });

    std::thread receiveThread([&]() {
        batteryModule.receiveFrames();
    });

    std::this_thread::sleep_for(std::chrono::seconds(10));

    simulationThread.join();
    receiveThread.join();

    return 0;
}