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
#ifndef HANDLE_FRAME_H_
#define HANDLE_FRAME_H_

#include <vector>
#include <iostream>
#include <linux/can.h>
#include <algorithm>
#include <iterator>
#include <cstring>
#include <cerrno>
#include <thread>

class HandleFrames 
{
private:
    /* Vector to store received data */ 
    std::vector<int> stored_data;        
    /* Expected total size of data */         
    size_t expected_data_size;    
    /* Position of the Service Identifier (SID) in the received data */                
    size_t sid_position;      
    /* Flag to track the number of consecutive frames expected */ 
    int flag;                                    
    /* Protocol Control Information (PCI) byte */ 
    int pci;                                   
    /* Service Identifier (SID) */   
    int sid;                    
    /* Flag indicating if the frame is a single frame message */                  
    bool is_single_frame;    
    /* Flag indicating if the first frame is received */
    bool first_frame = false;      
                    
public:
    /**
     * @brief Construct a new Handle Frames object
     * 
     */
    HandleFrames() : expected_data_size(0), flag(0) {}
    /**
     * @brief Method for checking the validity of the received CAN frame
     * 
     * @param nbytes 
     * @param frame 
     * @return true 
     * @return false 
     */
    bool checkReceivedFrame(int nbytes, const struct can_frame &frame);
    /**
     * @brief Method for processing received CAN frame
     * 
     * @param frame 
     */
    void processReceivedFrame(const struct can_frame &frame);
    /**
     * @brief Method for handling complete data after reception
     * 
     * @param id 
     * @param data 
     * @param is_single_frame 
     */
    void handleCompleteData(int id, const std::vector<int>& data, bool is_single_frame);
};

#endif // HANDLE_FRAME_H_