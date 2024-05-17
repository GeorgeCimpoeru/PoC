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
#include <thread>
#include "GenerateFrames.h"

class BatteryModule {
private:
    float voltage;
    float current;
    float temperature;

    void updateParamenters();
    void sendCANFrames();

    std::thread simulationThread;
    GenerateFrames generateFrames;

public:
    BatteryModule(int socket);

    void simulate();

    float getVoltage() const;
    float getCurrent() const;
    float getTemperature() const;
};

#endif