/**
 *  @file BatteryModule.h
 *  This library will be used to simulate a Battery Module
 *  providing some readings and informations (like voltage, current, temperature, etc)
 *  in order to be passed through a virtual CAN interface.
 * 
 *  How to use : Simply instantiate the class in Main solution, and access it's methods there.
 *  BatteryModule batteryObj;  *  Default Constructor for battery object with
 *                             *  moduleID 0x101 and interface name 'vcan0'
 *  BatteryModule batteryObj(interfaceNumber, moduleID);  *  Custom Constructor, for battery object
 *                             *  with custom moduleID, custom interface name
 * @author: Alexandru Nagy
 * @date: May 2024
 * @version 1.0
 */

#ifndef POC_INCLUDE_BATTERY_MODULE_H
#define POC_INCLUDE_BATTERY_MODULE_H

#define BATTERY_MODULE_INIT_VOLTAGE 12.5
#define BATTERY_MODULE_INIT_CURRENT 5.0
#define BATTERY_MODULE_INIT_TEMP 20.0
#define BATTERY_MODULE_MAX_VOLTAGE 15.0f
#define BATTERY_MODULE_MAX_CURRENT 10.0f
#define BATTERY_MODULE_MAX_TEMP 30.0f
#define BATTERY_MODULE_PARAM_INCREMENT 0.01f

#define BATTERY_MODULE_ID 0x101

/* comment-out to disable all the prints in module */
#define BATTERY_MODULE_DEBUG

#include <thread>
#include <cstdlib>
#include <chrono>
#include <iostream>
#include "InterfaceConfig.h"
#include "ReceiveFrames.h"

class BatteryModule
{
private:
    int moduleId;

    float voltage;
    float current;
    float temperature;

    /**
     * @brief flag to know the state of module
     * 
     */
    bool running;
    /**
     * @brief Thread for simulation of battery
     * 
     */
    std::thread simulationThread;

    SocketCanInterface canInterface;
    ReceiveFrames* frameReceiver;

    /* Battery Class private methods */
    void startBatteryModule();
    void stopBatteryModule();

public:
    /**
     * @brief Function to update the parameters values,
     * simulating some logic for reading changings.
     * 
     * Currently in 'public' access level for test purposes
     */
    void updateParamenters();
    /**
     * @brief Construct a new Battery Module object
     * 
     */
    BatteryModule();
    /**
     * @brief Construct a new Battery Module object with custom interface name, custom moduleId
     * 
     * @param _interfaceNumber 
     * @param _moduleId 
     */
    BatteryModule(int _interfaceNumber, int _moduleId);
    /**
     * @brief Destroy the Battery Module object
     * 
     */
    ~BatteryModule();

    /**
     * @brief simulate the readings of battery,
     * start the frame receiver, and stop the
     * frame receiver.
     */
    void simulate();
    void receiveFrames();
    void stopFrames();

    /* Member Accessors */
    float getVoltage() const;
    float getCurrent() const;
    float getTemperature() const;
};

#endif