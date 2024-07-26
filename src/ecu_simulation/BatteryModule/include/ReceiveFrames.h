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
#include "HandleFrames.h"
#include "../../../utils/include/GenerateFrames.h"
#include "BatteryModuleLogger.h"

class ReceiveFrames 
{
private:
    /* Descriptor for the socket connection */
    int socket = -1;            
    /* Module ID for filtering incoming frames */  
    int frame_id;    
    /* Battery Module ID for filtering frames for this module */              
    uint8_t current_module_id = 0x11;                 
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
    
protected:
    HandleFrames handle_frame;
    
public:
    /**
     * @brief Parameterized constructor.
     * 
     * @param socket The socket file descriptor.
     * @param frame_id Frame identifier.
     */
    ReceiveFrames(int socket, int frame_id);

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