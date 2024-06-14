/**
 *  @file BatteryModule.h
 *  This library will be used to simulate a Battery Module
 *  providing some readings and informations (like energy, voltage, percentage, etc)
 *  extracted from Linux, in order to be passed through a virtual CAN interface.
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

#define BATTERY_MODULE_ID 0x101

/* comment-out to disable all the prints in module */
#define BATTERY_MODULE_DEBUG

#include <thread>
#include <cstdlib>
#include <chrono>
#include <iostream>
#include "InterfaceConfig.h"
#include "ReceiveFrames.h"
#include "GenerateFrames.h"
#include "../../../utils/Logger/include/Logger.h"

class BatteryModule
{
private:
    int moduleId;

    float energy;
    float voltage;
    float percentage;
    std::string state;

    /**
     * @brief Thread for simulation of battery
     * 
     */
    std::thread simulationThread;

    SocketCanInterface canInterface;
    ReceiveFrames* frameReceiver;    

public:
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
     * @brief Function to notify MCU if the module is Up & Running
     * 
     */
    void notifyUp();

    /**
     * @brief Function to notify MCU if the module is Down
     * 
     * This will be only temporary.
     */
    void notifyDown();

    /**
     * @brief Helper function to execute shell commands and fetch output
     * in order to read System Information about built-in Battery
     * 
     * this method is currently 'virtual' in order to be overridden in Test
     * 
     * @param cmd 
     * @return std::string 
     */
    virtual std::string exec(const char *cmd);

    /**
     * @brief This function will parse the data from the system about battery,
     * and will store all values in separate variables
     * 
     * @param data 
     * @param _energy 
     * @param _voltage 
     * @param _percentage 
     * @param _temperature 
     */
    void parseBatteryInfo(const std::string &data, float &energy, float &voltage, float &percentage, std::string &state);

    /**
     * @brief Function to fetch data from system about battery
     * 
     */
    void fetchBatteryData();

    /**
     * @brief simulate the readings of battery,
     * start the frame receiver, and stop the
     * frame receiver.
     */
    void receiveFrames();
    void stopFrames();

    /* Member Accessors */
    float getEnergy() const;
    float getVoltage() const;
    float getPercentage() const;

    /**
     * @brief Get the Linux Battery State - charging, discarging, fully-charged, etc.
     * 
     * @return std::string 
     */
    std::string getLinuxBatteryState();
};

#endif
