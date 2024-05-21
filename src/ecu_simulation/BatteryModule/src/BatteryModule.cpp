#include "BatteryModule.h"
#include <cstdlib>
#include <chrono>
#include <iostream>

// Constructor - initializes the BatteryModule with default values,
// sets up the CAN interface, and prepares the frame receiver.
BatteryModule::BatteryModule() : moduleId(0x101),
                                 voltage(12.5),
                                 current(5.0),
                                 temperature(20.0),
                                 running(false),
                                 canInterface("vcan0"),
                                 frameReceiver(canInterface.getSocketFd(), moduleId)
{
    canInterface.init();
}

// Destructor
BatteryModule::~BatteryModule()
{
    stopBatteryModule();
    // canInterface.closeInterface();   // called in destructor of it's class
}

// Start the simulation
void BatteryModule::simulate()
{
    running = true; // set the 'running' flag
    simulationThread = std::thread(&BatteryModule::startBatteryModule, this);
}

// Run the battery module simulation loop
void BatteryModule::startBatteryModule()
{
    while (running)
    {
        updateParamenters();
        std::this_thread::sleep_for(std::chrono::seconds(1));
    }
}

// Stop the simulation and join the thread
void BatteryModule::stopBatteryModule()
{
    running = false; // un-set the 'running' flag

    if (simulationThread.joinable())
    {
        simulationThread.join();
    }
}

void BatteryModule::updateParamenters()
{
    std::cout << "Battery Module - updateParamenters()" << std::endl;

    // Simulate some logic to update voltage, current, and temperature
    voltage += 0.01f;
    current += 0.01f;
    temperature += 0.1f;

    // Ensuring the parameters don't exceed certain values for simulation
    if (voltage > 15.0f)
    {
        voltage = BATTERY_MODULE_INIT_VOLTAGE;
    }
    if (current > 10.0f)
    {
        current = BATTERY_MODULE_INIT_CURRENT;
    }

    if (temperature > 30.0f)
    {
        temperature = BATTERY_MODULE_INIT_TEMP;
    }
}

// Receive CAN frames in a loop
void BatteryModule::receiveFrames()
{
    while (running)
    {
        HandleFrames handleFrames;
        frameReceiver.Receive(handleFrames);
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
    }
}

float BatteryModule::getVoltage() const
{
    return voltage;
}

float BatteryModule::getCurrent() const
{
    return current;
}

float BatteryModule::getTemperature() const
{
    return temperature;
}