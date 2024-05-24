#include "../include/BatteryModule.h"
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
                                 frameReceiver(nullptr)
{
#ifdef BATTERY_MODULE_DEBUG
    std::cout << "BatteryModule()" << std::endl;
    std::cout << "(BatteryModule)moduleId = " << this->moduleId << std::endl;
#endif
    // Initialize the CAN interface
    canInterface.init();

    // Initialize the Frame Receiver
    frameReceiver = new ReceiveFrames(canInterface.getSocketFd(), moduleId);
}

// Parameterized Constructor - initializes the BatteryModule with provided interface number and module ID
BatteryModule::BatteryModule(int _interfaceNumber, int _moduleId) : moduleId(_moduleId),
                                                                  voltage(12.5),
                                                                  current(5.0),
                                                                  temperature(20.0),
                                                                  running(false),
                                                                  canInterface("vcan" + std::to_string(_interfaceNumber)),
                                                                  frameReceiver(nullptr)
{
#ifdef BATTERY_MODULE_DEBUG
    std::cout << "BatteryModule(int interfaceNumber, int moduleId)" << std::endl;
    std::cout << "(BatteryModule)moduleId = " << this->moduleId << std::endl;
#endif
    // Initialize the CAN interface
    canInterface.init();
    // Initialize the Frame Receiver
    frameReceiver = new ReceiveFrames(canInterface.getSocketFd(), moduleId);
}

// Destructor
BatteryModule::~BatteryModule()
{
    stopBatteryModule();
    // canInterface.closeInterface();   // called in destructor of it's class
    delete frameReceiver;
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
#ifdef BATTERY_MODULE_DEBUG
    std::cout << "Battery Module - updateParamenters()" << std::endl;
#endif
    // Simulate some logic to update voltage, current, and temperature
    voltage += 0.01f;
    current += 0.01f;
    temperature += 0.1f;
#ifdef BATTERY_MODULE_DEBUG
    std::cout << "Voltage : " << voltage << std::endl;
    std::cout << "Current : " << current << std::endl;
    std::cout << "Temperature : " << temperature << std::endl;
#endif
    // Ensuring the parameters don't exceed certain values for simulation
    if (voltage > 15.0f)
    {
        voltage = 12.5;
    }
    
    if (current > 10.0f)
    {
        current = 5.0;
    }

    if (temperature > 30.0f)
    {
        temperature = 20.0;
    }
}

// Receive CAN frames
void BatteryModule::receiveFrames()
{
    HandleFrames handleFrames;
    frameReceiver->Receive(handleFrames);
    std::this_thread::sleep_for(std::chrono::milliseconds(100));
}

// Stop receiving frames
void BatteryModule::stopFrames()
{
    frameReceiver->Stop();
}

// Member Accessors
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