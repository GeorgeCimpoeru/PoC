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
    /* Adjust timer until programming session will expire here. 
     * Now it's set to 10 seconds.
    */
    static constexpr uint8_t S3_TIMER =  0x0A;
private:
    int socket = -1;
    Logger& logger;
    GenerateFrames generate_frames;
    DiagnosticSessionControl& sessionControl;
    /**
    * end_time is the total time until the delay timer will expire.
    * Set end_time to a distant future point. This ensures that it is always.
    * greater than the current time unless a specific delay timer is activated.
    */
    static std::chrono::steady_clock::time_point end_time;
    /**
    * Time left until the delay timer will expire.
    */
    static uint32_t time_left;

public:
    TesterPresent(int socket, Logger& logger, DiagnosticSessionControl& sessionControl);

    /**
     * @brief Method to handle the Tester Present request
     * 
     * @param can_id CAN ID
     * @param request Request data
    */
    void handleTesterPresent(uint32_t can_id, std::vector<uint8_t> request);
    /**
     * @brief Retrieves the end time of the programming session.
     * 
     * This function returns the exact point in time when the current
     * programming session is set to expire. It uses the system's steady
     * clock to provide the time.
     * 
     * @return std::chrono::steady_clock::time_point The end time of the programming session.
    */
    static std::chrono::steady_clock::time_point getEndTimeProgrammingSession();
    /**
     * @brief Resets the end time of the programming session.
     * This function sets the end time of the programming session by adding
     * a predefined amount of time to the current system
     * time. It is used to restart the duration of the session.
     * @param isProgramming tells if we are in programming session or not to 
     * change the endTimer correctly.
    */
    static void setEndTimeProgrammingSession(bool isProgramming = false);
};

#endif
