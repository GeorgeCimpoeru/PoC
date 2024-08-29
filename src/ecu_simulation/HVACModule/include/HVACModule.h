/**
 * @file HVACModule.h
 * @author Iancu Daniel
 * @brief 
 * HVAC MODES: 0b10101010
 *  b0 AC STATUS
 *  b1 LEGS
 *  b2 FRONT
 *  b3 AIR_CIRCULATION
 *  b4 DEFROST
 *  b5 
 *  b6
 *  b7
 * @version 0.1
 * @date 2024-08-21
 * 
 * @copyright Copyright (c) 2024
 * 
 */
#ifndef HVACMODULE_H
#define HVACMODULE_H

#include <iostream>
#include <unordered_map>
#include <vector>
#include <thread>
#include <random>
#include <ctime>
#include "../../../utils/include/ECU.h"
#include "HVACModuleLogger.h"
#include "HVACIncludes.h"

class HVACModule
{
private:
    Logger& _logger = *hvacModuleLogger;
    /* Static dictionary to store SID and processing time */
    static std::map<uint8_t, double> timing_parameters;
    /* Store active timers for SIDs */
    static std::map<uint8_t, std::future<void>> active_timers;
    /* Stop flags for each SID. */
    static std::map<uint8_t, std::atomic<bool>> stop_flags;
    /* Variable to store hvac data*/
    std::unordered_map<uint16_t, std::vector<uint8_t>> default_DID_hvac =
    {
        {AMBIENT_TEMPERATURE_DID, {0}}, /* Ambient temperature */
        {CABIN_TEMPERATURE_DID, {0}}, /* Cabin temperature */
        {HVAC_SET_TEMPERATURE_DID, {0}}, /* HVAC set temperature */
        {FAN_SPEED_DID, {0}}, /* Fan speed (Duty cycle) */
        {HVAC_MODES_DID, {0}}  /* HVAC modes */
    };

public:
    ECU *_ecu;
    /**
     * @brief Construct a new HVACModule object
     * 
     */
    HVACModule();
    /**
     * @brief Destroy the HVACModule object
     * 
     */
    ~HVACModule();

    /**
     * @brief 
     * 
     */
    void initHVAC();

    /**
     * @brief 
     * 
     */
    void fetchHvacData();

    /**
     * @brief 
     * 
     */
    void generateData();

    /**
     * @brief 
     * 
     */
    void writeDataToFile();

    /**
     * @brief 
     * 
     */
    void printHvacInfo();

    /**
     * @brief Get the Socket object
     * 
     * @return int 
     */
    int getSocket();
};

extern HVACModule *hvac;

#endif