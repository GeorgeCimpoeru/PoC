/**
 *  @file DoorsModule.h
 *  This library will be used to simulate a Doors Module
 *  providing some readings and informations (like door status, lock status, door ajar warning, etc)
 *  extracted from Linux, in order to be passed through a virtual CAN interface.
 * 
 *  How to use : Simply instantiate the class in Main solution, and access its methods there.
 *  DoorsModule DoorsObj;  *  Default Constructor for Doors object with
 *                             *  moduleID 0x13
 *  DoorsModule DoorsObj(interfaceNumber, moduleID);  *  Custom Constructor, for Doors object
 *                             *  with custom moduleID, custom interface name
 * @author: Loredana Dugulean
 * @date: August 2024
 * @version 1.0
 */

#ifndef POC_INCLUDE_DOORS_MODULE_H
#define POC_INCLUDE_DOORS_MODULE_H

#define DOORS_MODULE_ID 0x13

#include <thread>
#include <cstdlib>
#include <chrono>
#include <iostream>
#include <unistd.h>
#include <future>
#include <fstream>
#include "../../../utils/include/CreateInterface.h"
#include "../../../utils/include/ReceiveFrames.h"
#include "../../../utils/include/GenerateFrames.h"
#include "DoorsModuleLogger.h"

class DoorsModule
{
private:
    int moduleId;    
    int doors_socket = -1;

    CreateInterface* canInterface;
    ReceiveFrames* frameReceiver;

public:
    /* Static dictionary to store SID and processing time */
    static std::map<uint8_t, double> timing_parameters;
    /* Store active timers for SIDs */
    static std::map<uint8_t, std::future<void>> active_timers;
    /* Stop flags for each SID. */
    static std::map<uint8_t, std::atomic<bool>> stop_flags;
    /* Variable to store ecu data: 0:closed; 1:open; 0:unlocked; 1:locked; 0:no warning; 1: warning */
    static std::unordered_map<uint16_t, std::vector<uint8_t>> default_DID_doors;
    /**
     * @brief Default constructor for Doors Module object.
     */
    DoorsModule();
    /**
     * @brief Parameterized constructor for Doors Module object with custom interface name, custom moduleId.
     * 
     * @param _interfaceNumber Interface number used to create vcan interface.
     * @param _moduleId Custom module identifier.
     */
    DoorsModule(int _interfaceNumber, int _moduleId);
    /**
     * @brief Destructor Doors Module object.
     */
    virtual ~DoorsModule();

    /**
     * @brief Function to notify MCU if the module is Up & Running.
     */
    void sendNotificationToMCU();
       
    /**
     * @brief Function to fetch data from system about doors.
     */
    void fetchDoorsData();

    /**
     * @brief Function that starts the frame receiver.
     */
    void receiveFrames();

    /**
     * @brief Function that stops the frame receiver.
     */
    void stopFrames();

    /**
     * @brief Get the door Socket.
     * 
     * @return Returns the sid of the socket. 
     */
    int getDoorsSocket() const;

    /**
     * @brief Recreates and bind the door Socket on a given interface.
     * 
     * @param interface_number The interface on which the socket will be created.
     */
    void setDoorsSocket(uint8_t interface_number);

    /**
     * @brief Write the default_did or the date before reset in doors_data.txt
     * 
     */
    void writeDataToFile();
};
extern DoorsModule* doors;

#endif