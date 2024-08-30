#ifndef POC_INCLUDE_ENGINE_MODULE_H
#define POC_INCLUDE_ENGINE_MODULE_H

#define ENGINE_MODULE_ID 0x12

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
#include "EngineModuleLogger.h"

class EngineModule
{
private:
    int moduleId;
    int engine_socket = -1;

    std::string state;

    CreateInterface* canInterface;
    ReceiveFrames* frameReceiver;

public:
    /* Static dictionary to store SID and processing time */
    static std::map<uint8_t, double> timing_parameters;
    /* Store active timers for SIDs */
    static std::map<uint8_t, std::future<void>> active_timers;
    /* Stop flags for each SID. */
    static std::map<uint8_t, std::atomic<bool>> stop_flags;
    /* Variable to store ecu data */
    std::unordered_map<uint16_t, std::vector<uint8_t>> default_DID_engine = {
        /* Engine RPM */
        {0x0100, {0}},
        /* Engine Coolant Temperature */
        {0x010C, {0}},
        /* Throttle Position */
        {0x0110, {0}},
        /* Vehicle Speed */
        {0x0114, {0}},
        /* Engine Load */
        {0x011C, {0}},
        /* Fuel Level */
        {0x0120, {0}},
        /* Oil Temperature */
        {0x0124, {0}},
        /* Fuel Pressure */
        {0x012C, {0}},
        /* Intake Air Temperature */
        {0x0130, {0}},
        /* Mass Air Flow (MAF) Sensor */
        {0x0134, {0}},
        /* Ambient Air Temperature */
        {0x0140, {0}}
    };
    /**
     * @brief Default constructor for Engine Module object.
     */
    EngineModule();
    /**
     * @brief Parameterized constructor for Engine Module object with custom interface name, custom moduleId.
     * 
     * @param _interfaceNumber Interface number used to create vcan interface.
     * @param _moduleId Custom module identifier.
     */
    EngineModule(int _interfaceNumber, int _moduleId);
    /**
     * @brief Destructor Engine Module object.
     */
    virtual ~EngineModule();

    /**
     * @brief Function to notify MCU if the module is Up & Running.
     */
    void sendNotificationToMCU();

    /**
     * @brief This function generates random values for all DID entries defined in the default_DID_engine map
     * 
     */
    void fetchEngineData();
    /**
     * @brief Function that starts the frame receiver.
     */
    void receiveFrames();

    /**
     * @brief Function that stops the frame receiver.
     */
    void stopFrames();

    /**
     * @brief Get the Engine Socket.
     * 
     * @return Returns the sid of the socket. 
     */
    int getEngineSocket() const;

    /**
     * @brief Recreates and bind the Engine Socket on a given interface.
     * 
     * @param interface_number The interface on which the socket will be created.
     */
    void setEngineSocket(uint8_t interface_number);
};
extern EngineModule* engine;

#endif