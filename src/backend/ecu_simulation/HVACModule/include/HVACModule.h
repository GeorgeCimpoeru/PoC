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
#include <string>
#include <ctime>
#include "../../../utils/include/ECU.h"
#include "HVACModuleLogger.h"
#include "HVACIncludes.h"

class HVACModule
{
private:
    Logger& _logger;
    

public:
    /* Variable to store hvac data*/
    static std::unordered_map<uint16_t, std::vector<uint8_t>> default_DID_hvac;

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

    /**
     * @brief Convert all the letters to lowercase
     * 
     */
    std::string to_lowercase(const std::string& str);
};

extern HVACModule *hvac;

#endif