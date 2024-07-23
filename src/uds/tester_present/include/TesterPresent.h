/**
 * @file TesterPresent.h
 * @brief UDS Service 0x3E Tester Present.
 * 
 */

#ifndef TESTER_PRESENT_H
#define TESTER_PRESENT_H

#include "../../../utils/include/GenerateFrames.h"
#include "../../../utils/include/CreateInterface.h"
#include <iostream>
#include <vector>
#include <thread>
#include <chrono>
#include <atomic>

class TesterPresent
{
private:
    GenerateFrames* generate;
    std::thread timerThread;
    std::atomic<bool> running;
    Logger logger;
    int socket;
    int timeout_duration = 5;

public:
    /**
     * @brief Construct a new Tester Present object
     * 
     * @param logger Logger instance
     * @param socket CAN socket
     * @param timeout_duration Duration for the S3 timer in seconds
     */
    TesterPresent(Logger logger, int socket, int timeout_duration = 5);
    TesterPresent();

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
