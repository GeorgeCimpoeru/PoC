/*
   ECU Battery Module
   This library will be used to simulate a Battery Module
   providing some readings and informations (like voltage, current, temperature, etc)
   in order to be passed through a virtual CAN interface.

   Author : Alexandru Nagy
   Date : 15.05.2024
*/

#ifndef POC_INCLUDE_BATTERY_MODULE_H
#define POC_INCLUDE_BATTERY_MODULE_H

#define BATTERY_MODULE_INIT_VOLTAGE 12.5
#define BATTERY_MODULE_INIT_CURRENT 5.0
#define BATTERY_MODULE_INIT_TEMP 20.0

#define BATTERY_MODULE_ID 0x101

#define BATTERY_MODULE_DEBUG

#include <thread>
#include "InterfaceConfig.h"
#include "ReceiveFrames.h"

class BatteryModule
{
private:
    int moduleId;

    float voltage;
    float current;
    float temperature;

    bool running;   // flag to know the state of module
    std::thread simulationThread;

    SocketCanInterface canInterface;
    ReceiveFrames* frameReceiver;

    void updateParamenters();
    void startBatteryModule();
    void stopBatteryModule();

public:
    BatteryModule();
    BatteryModule(int _interfaceNumber, int _moduleId);
    ~BatteryModule();

    void simulate();
    void receiveFrames();
    void stopFrames();

    float getVoltage() const;
    float getCurrent() const;
    float getTemperature() const;
};

#endif