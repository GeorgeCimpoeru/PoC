/**
 * @file TesterPresent.h
 * @brief UDS Service 0x3E Tester Present.
 * 
 */

#ifndef TESTER_PRESENT_H
#define TESTER_PRESENT_H

#include "../../../utils/include/GenerateFrames.h"
#include "../../../utils/include/CreateInterface.h"
#include "../../diagnostic_session_control/include/DiagnosticSessionControl.h"
#include <iostream>
#include <vector>
#include <thread>
#include <chrono>
#include <atomic>
// #include <condition_variable>
// #include <mutex>

class TesterPresent
{
private:
    Logger& logger;
    GenerateFrames* generate;
    DiagnosticSessionControl* sessionControl;
    std::thread timerThread;
    bool running;
    int socket;
    int timeout_duration;
    // std::chrono::steady_clock::time_point last_timer_reset_time;
    // std::condition_variable cv;
    // std::mutex mtx;
    // bool reset_timer = false;

public:
    /**
     * @brief Construct a new Tester Present object
     * 
     * @param logger Logger instance
     * @param socket CAN socket
     * @param timeout_duration Duration for the S3 timer in seconds
     * @param sessionControl DiagnosticSessionControl instance
     */
    TesterPresent(Logger &logger, DiagnosticSessionControl* sessionControl, int socket, int timeout_duration);

    /**
     * @brief Destroy the Tester Present object
     * 
     */
    ~TesterPresent();

    /**
     * @brief Method to handle the Tester Present request
     * 
     * @param can_id CAN ID
     * @param data Request data
     */
    void handleTesterPresent(uint32_t can_id, std::vector<uint8_t> data);

    /**
     * @brief Method to check the S3 timer
     * 
     */
    void checkTimer();

    /**
     * @brief Method to check if the tester is present
     * 
     */
    void startTimerThread();

    /**
     * @brief Stops the timer thread
     * 
     */
    void stopTimerThread();

private:
    /**
     * @brief Timer function to check the S3 timer
     * 
     */
    void timerFunction();
};

#endif // TESTER_PRESENT_H
