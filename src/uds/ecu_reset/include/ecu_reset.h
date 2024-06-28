/**
 * @file ecu_reset.h
 * @author Daniel Constantinescu
 * @brief Logic for ECU Reset service.
 * This class offer the needed methods for using the ECU Reset service.
 * It allows either a Hard Reset or a Key Off Reset
 */

#ifndef ECU_RESET_H
#define ECU_RESET_H

#include "../../../ecu_simulation/BatteryModule/include/HandleFrames.h"

#include <linux/can.h>
#include <iostream>



class EcuReset
{
public:
    void ecuReset(uint8_t reset_form); 

    void hardReset();

    void keyOffReset();
}