/**
 * @file TesterPresent.h
 * @author Theodor Stoica & Alexandru Doncea
 * @brief UDS Service 0x3E TesterPresent
 * @version 0.1
 * @date 2024-09-04
 * 
 * @copyright Copyright (c) 2024
 * 
 */
#ifndef TESTER_PRESENT_H
#define TESTER_PRESENT_H

#include "../../../utils/include/GenerateFrames.h"
#include "../../../utils/include/CreateInterface.h"
#include "../../diagnostic_session_control/include/DiagnosticSessionControl.h"
#include "../../../utils/include/NegativeResponse.h"
#include <iostream>
#include <vector>
#include <thread>
#include <chrono>
#include <atomic>
class TesterPresent
{
public:
    /* SID for TesterPresent */
    static constexpr uint8_t TESTER_PRESENT_SID = 0x3E;
private:
    int socket = -1;
    Logger& logger;
    GenerateFrames generate_frames;
    DiagnosticSessionControl& sessionControl;
    int timeout_duration;

public:
    TesterPresent(int socket, Logger& logger, DiagnosticSessionControl& sessionControl);

    /**
     * @brief Method to handle the Tester Present request
     * 
     * @param can_id CAN ID
     * @param request Request data
     */
    void handleTesterPresent(uint32_t can_id, std::vector<uint8_t> request);
};

#endif
