/**
 * @file HandleFrames.h
 * @author Stoisor Miruna
 * @brief The library facilitates the transmission of Controller Area Network (CAN) 
 * frames through an interface utilizing sockets.
 * The library also gives some methods for the creation of specific 
 * frames for the main services.
 * How to use example:
 *     GenerateFrames g1 = GenerateFrames(socket);
 *     std::vector<int> x = {0x11, 0x34, 0x56};
 *     g1.SendFrame(0x23, x);
 *     g1.SessionControl(0x34A, 0x1);
 * @version 0.1
 * @date 2024-05-27
 * @copyright Copyright (c) 2024
 */
#ifndef POC_INCLUDE_RECEIVEFRAME_H_
#define POC_INCLUDE_RECEIVEFRAME_H_

#include <iostream>
#include <vector>
#include <unistd.h>
#include <linux/can.h>
#include <sstream>
#include <iomanip>
#include <cstring>
#include <deque>
#include <thread>
#include <mutex>
#include <condition_variable>
#include <atomic>
#include <poll.h> 
#include <future>
#include <set>

#include "HandleFrames.h"
#include "GenerateFrames.h"
#include "Logger.h"

/* List of service we have implemented. */
const std::vector<uint8_t> service_sids = {
    /* ECU Reset */
    0x11,
    /* Diagnostic Session Control */
    0x10,
    /* Read Data By Identifier */
    0x22,
    /* Authentication */
    0x29,
    /* Routine Control (Testing) -> will be decided */
    0x31,
    /* Tester Present */
    0x3E,
    /* Read Memory By Address */
    0x23,
    /* Write Data By Identifier */
    0x2E,
    /* Read DTC Information */
    0x19,
    /* Clear Diagnostic Information */
    0x14,
    /* Access Timing Parameters */
    0x83,
    /* Request Download */
    0x34,
    /* Transfer Data */
    0x36,
    /* Request Transfer Exit */
    0x37,
    /* Request update status */
    0x32
};

/* Define lists of SIDs using p2_max_time and p2_star_max_time */
static const std::set<uint8_t> sids_using_p2_max_time = {
    /* ECU Reset */
    0x11,
    /* Diagnostic Session Control */
    0x10,
    /* Read Data By Identifier */
    0x22,
    /* Tester Present */
    0x3E,
    /* Read Memory By Address */
    0x23,
    /* Write Data By Identifier */
    0x2E,
    /* Read DTC Information */
    0x19,
    /* Clear Diagnostic Information */
    0x14,
    /* Access Timing Parameters */
    0x83,
    /* Request update status */
    0x32
};

static const std::set<uint8_t> sids_using_p2_star_max_time = {
    /* Routine Control */
    0x31,
    /* Request Download */
    0x34,
    /* Transfer Data */
    0x36,
    /* Request Transfer Exit */
    0x37,
    /* Authentication */
    0x29
};

class ReceiveFrames 
{
private:
    /* Descriptor for the socket connection */
    int socket = -1;             
    /* Battery Module ID for filtering frames for this module */              
    int current_module_id;                 
    /* Define frame_buffer as a deque of tuples */ 
    std::deque<std::tuple<can_frame, int>> frame_buffer; 
    /* Mutex for ensuring thread safety when accessing the frame buffer */   
    std::mutex mtx;              
    /* Condition variable for thread synchronization */                 
    std::condition_variable cv;               
    /* Flag indicating whether the receive threads should continue running */     
    std::atomic<bool> running;                
    /* Thread for buffering in receiving frames */                
    std::thread bufferFrameInThread;
    /* The logger used to write the logs. */
    Logger& receive_logger;
    /* Update security for ECU based on notify */
    static bool ecu_state;

    /**
     * @brief bufferFrameIn thread function that reads frames from the socket and adds them to the buffer.
     */
    void bufferFrameIn();

    /**
     * @brief Consumer thread function that processes frames from the buffer.
     * 
     * @param handle_frame HandleFrame object used for getting new frames.
     */
    void bufferFrameOut(HandleFrames &handle_frame);

    /* Method that start time processing frame. */
    void startTimer(uint8_t frame_dest_id, uint8_t sid);
    /* Method that stop time processing frame. */
    void stopTimer(uint8_t frame_dest_id, uint8_t sid);
    
protected:
    HandleFrames handle_frame;
    
public:

    /**
     * @brief Parameterized constructor.
     * 
     * @param socket The socket file descriptor.
     * @param current_module_id Frame identifier.
     * @param receive_logger A logger instance used to record information and errors during the execution.
     */
    ReceiveFrames(int socket, int current_module_id, Logger& receive_logger);

    /**
     * @brief Destructor.
     */
    ~ReceiveFrames();

    /**
     * @brief Debug function to print the details of a CAN frame.
     * 
     * @param frame The CAN frame to be printed.
     */
    void printFrame(const struct can_frame &frame);

    /**
     * @brief Gets the current state of the ecu security system.
     * 
     * @return true if the ecu security is enabled, false otherwise.
     */
    static bool getEcuState();

    /**
     * @brief Set the state of the ecu security system.
     * 
     */
    static void setEcuState(bool value);
    /**
     * @brief Starts the receive process by creating bufferFrameIn and bufferFrameOut threads.
     * 
     * @param handle_frame HandleFrame object used for getting new frames.
     */
    void receive(HandleFrames &handle_frame);

    /**
     * @brief Stops the receive process gracefully.
     */
    void stop();
};

#endif