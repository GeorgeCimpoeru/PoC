/**
 * @file ECU.h
 * @author Iancu Daniel
 * @brief 
 * @version 0.1
 * @date 2024-08-26
 * 
 * @copyright Copyright (c) 2024
 * 
 */

#ifndef ECU_H
#define ECU_H

#include <iostream>
#include "Logger.h"
#include "GenerateFrames.h"
#include "CreateInterface.h"
#include "ReceiveFrames.h"

class ECU
{
public:

    int _ecu_socket = -1;
    uint8_t _module_id;
    ReceiveFrames *_frame_receiver = nullptr;
    CreateInterface *_can_interface;
    Logger& _logger;

    ECU(uint8_t module_id, Logger& logger);
    ~ECU();

    void sendNotificationToMCU();
    void startFrames();
    void stopFrames();
};

#endif