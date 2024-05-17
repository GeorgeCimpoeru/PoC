#include "BatteryModule.h"
#include <cstdlib>
#include <chrono>
#include <iostream>

// Initialize GenerateFrames with the provided socket
BatteryModule::BatteryModule(int socket) : generateFrames(socket) {
    // Mock readings for now
    voltage = BATTERY_MODULE_INIT_VOLTAGE;
    current = BATTERY_MODULE_INIT_CURRENT;
    temperature = BATTERY_MODULE_INIT_TEMP;
    
    simulationThread = std::thread(&BatteryModule::simulate, this);
}

void BatteryModule::simulate() {
    while (true) {
        updateParamenters();
        sendCANFrames();
        std::this_thread::sleep_for(std::chrono::seconds(1));
    }
}

void BatteryModule::updateParamenters() {
    // Simulate some readings changings
    // starting with some mock values for now
    voltage = BATTERY_MODULE_INIT_VOLTAGE + static_cast<float>(rand()) / (static_cast<float>(RAND_MAX / 1.0));
    current = BATTERY_MODULE_INIT_CURRENT + static_cast<float>(rand()) / (static_cast<float>(RAND_MAX / 2.0));
    temperature = BATTERY_MODULE_INIT_TEMP + static_cast<float>(rand()) / (static_cast<float>(RAND_MAX / 10.0));
}

void BatteryModule::sendCANFrames() {
    // Convert all parameters to 'int' in order to be transmitted on CAN interface
    // To do

    // Send the readings
    // To do
}

float BatteryModule::getVoltage() const {
    return voltage;
}

float BatteryModule::getCurrent() const {
    return current;
}

float BatteryModule::getTemperature() const {
    return temperature;
}