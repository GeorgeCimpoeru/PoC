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
#include "../../../utils/include/ECU.h"
#include "HVACModuleLogger.h"

#define HVAC_ECU_ID 0x14

class HVACModule
{
private:
    Logger& _logger = *hvacModuleLogger;
    std::unordered_map<uint16_t, std::vector<uint8_t>> hvac_data =
    {
        {0x0140, {0}}, /* Ambient temperature */
        {0x0150, {0}}, /* Cabin temperature */
        {0x01C0, {0}}, /* HVAC temperature */
        {0x01D0, {0}}, /* Fan speed (Duty cycle)*/
        {0x01F0, {0}}  /* HVAC modes */
    };

public:
    ECU *_ecu;

    HVACModule();
    ~HVACModule();

    void initHVAC();
    int getSocket();
};

extern HVACModule *hvac;

#endif