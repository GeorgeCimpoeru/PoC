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
#include "../include/HandleFrames.h"

class ReceiveFrames 
{
private:
    /* Descriptor for the socket connection */
    int socket = -1;            
    /* Module ID for filtering incoming frames */                  
    int module_id = 0x101;                 
    /* Define frame_buffer as a deque of tuples */ 
    std::deque<std::tuple<can_frame, int>> frame_buffer; 
    /* Mutex for ensuring thread safety when accessing the frame buffer */   
    std::mutex mtx;              
    /* Condition variable for thread synchronization */                 
    std::condition_variable cv;               
    /* Flag indicating whether the receive threads should continue running */     
    bool running;                
    /* Thread for producing (receiving) frames */                
    std::thread producerThread;        
    /* Thread for consuming (handling) frames */         
    std::thread consumerThread;                   
    /**
     * @brief Producer thread function that reads frames from the socket and adds them to the buffer
     * 
     */
    void producer();
    /**
     * @brief Consumer thread function that processes frames from the buffer
     * 
     * @param handle_frame 
     */
    void consumer(HandleFrames &handle_frame);
    
protected:
    HandleFrames handle_frame;
    
public:
    /**
     * @brief Construct a new Receive Frames object
     * 
     * @param socket 
     * @param module_id 
     */
    ReceiveFrames(int socket, int module_id);
    /**
     * @brief Destroy the Receive Frames object
     * 
     */
    ~ReceiveFrames();
    /**
     * @brief Debug function to print the details of a CAN frame
     * 
     * @param frame 
     */
    void printFrame(const struct can_frame &frame);
    /**
     * @brief Starts the receive process by creating producer and consumer threads
     * 
     * @param handle_frame 
     */
    void Receive(HandleFrames &handle_frame);
    /**
     * @brief Stops the receive process gracefully
     * 
     */
    void Stop();
};

#endif