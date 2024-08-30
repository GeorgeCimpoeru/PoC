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
    Logger& _logger;
    
    /* Variable to store hvac data*/
    std::unordered_map<uint16_t, std::vector<uint8_t>> default_DID_hvac =
    {
        {AMBIENT_TEMPERATURE_DID, {DEFAULT_DID_VALUE}}, /* Ambient temperature */
        {CABIN_TEMPERATURE_DID, {DEFAULT_DID_VALUE}}, /* Cabin temperature */
        {HVAC_SET_TEMPERATURE_DID, {DEFAULT_DID_VALUE}}, /* HVAC set temperature */
        {FAN_SPEED_DID, {DEFAULT_DID_VALUE}}, /* Fan speed (Duty cycle) */
        {HVAC_MODES_DID, {DEFAULT_DID_VALUE}}  /* HVAC modes */
    };

public:

    /* ECU object used for sockets, frame handling and ecu specific parameters (timing, flags etc)*/
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
     * @brief Init method called in the constructor
     * 
     */
    void initHVAC();

    /**
     * @brief Method that generates random data and outputs it to file
     * 
     */
    void fetchHvacData();

    /**
     * @brief Method that generates random data for the hvac.
     * Each data has its own particularities (temperature boundaries etc)
     * 
     */
    void generateData();

    /**
     * @brief Write the deafault_DID_hvac data to file
     * 
     */
    void writeDataToFile();

    /**
     * @brief Print the hvac data with meaningful messages.
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