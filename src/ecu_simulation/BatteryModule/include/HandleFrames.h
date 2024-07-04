/**
 * @file HandleFrames.h
 * @author Stoisor Miruna
 * @brief The library facilitates the transmission of Controller Area Network (CAN) 
 * frames through an interface utilizing sockets.
 * The library also gives some methods for the creation of specific 
 * frames for the main services.
 * How to use example:
 *     GenerateFrames g1 = GenerateFrames(socket);
 *     std::vector<uint8_t> x = {0x11, 0x34, 0x56};
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
#include <sstream>
#include "../include/BatteryModuleLogger.h"
#include "../../../uds/diagnostic_session_control/include/DiagnosticSessionControl.h"

class HandleFrames 
{
private:
    int moduleId = 0x11;
    /* Vector to store received data */ 
    std::vector<uint8_t> stored_data;    
    /* Vector to store sid */ 
    int stored_sid;       
    /* Expected total size of data */         
    uint8_t expected_data_size;    
    /* Position of the Service Identifier (SID) in the received data */                
    size_t sid_position;      
    /* Flag to track the number of consecutive frames expected */ 
    uint8_t flag;                                    
    /* Protocol Control Information (PCI) byte */ 
    uint8_t pci;                                   
    /* Service Identifier (SID) */   
    int sid;                    
    /* Flag indicating if the frame is a single frame message */                  
    bool is_single_frame;    
    /* Flag indicating if the first frame is received */
    bool first_frame = false;  
    /* Vector to store data subfunction */ 
    uint8_t sub_function;

    /**
     * @brief Diagnostic Control Session object instance
     * this will enable the Default Session at module start
     * 
     */
    DiagnosticSessionControl diagnosticSessionControl;
                    
public:
    /**
     * @brief Default constructor for Handle Frames object.
     */
    HandleFrames() : expected_data_size(0), flag(0), diagnosticSessionControl(moduleId, batteryModuleLogger) {}

    /**
     * @brief Method for checking the validity of the received CAN frame.
     * 
     * @param nbytes Number of bytes.
     * @param frame Frame to be checked
     * @return Returns true of the processing is done or false if an error is encountered. 
     */
    bool checkReceivedFrame(int nbytes, const struct can_frame &frame);

    /**
     * @brief Method for processing received CAN frame.
     * 
     * @param frame Frame to be processed.
     */
    void processReceivedFrame(const struct can_frame &frame);

    /**
     * @brief Method for handling complete data after reception.
     * 
     * @param id The can identifier.
     * @param data Frame data.
     * @param is_single_frame Flag to verify if it is a sinle frame or multiple frames.
     */
    void handleCompleteData(int id, const std::vector<uint8_t>& data, bool is_single_frame);

    /**
     * @brief  Method used to send a frame based on the nrc(negative response code) received.
     * It takes as parameters frame_id, sid to identify the service, and nrc to send the correct
     * negative response code back to who made the request.
     * 
     * @param frame_id Frame identifier.
     * @param sid Service identifier.
     * @param nrc Negative response code.
     */
    void processNrc(int id, uint8_t sid, uint8_t nrc);
};

#endif /* HANDLE_FRAME_H_ */ 