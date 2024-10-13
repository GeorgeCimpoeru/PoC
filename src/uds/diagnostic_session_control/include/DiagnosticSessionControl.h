/**
 * @file DiagnosticSessionControl.h
 * @author Alexandru Nagy and Dirva Nicolae
 * @brief This library represents the Diagnostic Session Control UDS service.
 * It manages the transition between different diagnostic sessions, such as default and programming sessions.
 * By default, the session is set to the default session. The service responds to requests for session control by
 * switching to the specified session based on the received CAN frame. The request frame has the format:
 * frame.data = {PCI_L(1 byte), SID(1 byte = 0x10), SUB_FUNCTION(1 byte)}.
 * The positive response frame sent by the service will have the format:
 * frame.data = {PCI_L(1 byte), RESPONSE_SID(1 byte = 0x50), SUB_FUNCTION(1 byte)}.
 * The negative response frame sent by the service will have the format:
 * frame.data = {PCI_L(1 byte), 0x7F, SID(1 byte = 0x10), NRC(1 byte)} indicating errors such as
 * unsupported sub-functions or incorrect message length.
 */

#ifndef DIAGNOSTICSESSIONCONTROL_H
#define DIAGNOSTICSESSIONCONTROL_H

#include <cstdlib>
#include <iostream>
#include "../../../utils/include/GenerateFrames.h"
#include "../../../utils/include/Logger.h"
#include "../../../utils/include/NegativeResponse.h"

/* Diagnostic Control Session codes */
const uint8_t SID_DIAGNOSTIC_SESSION_CONTROL = 0x10;
const uint8_t SUB_FUNCTION_DEFAULT_SESSION = 1;
const uint8_t SUB_FUNCTION_PROGRAMMING_SESSION = 2;
const uint8_t SUB_FUNCTION_EXTENDED_DIAGNOSTIC_SESSION = 3;

enum DiagnosticSession
{
    DEFAULT_SESSION = 0x01,
    PROGRAMMING_SESSION,
    EXTENDED_DIAGNOSTIC_SESSION
    /* Other sessions can be defined here */
};

class DiagnosticSessionControl
{
public:
    /**
     * @brief Construct a new Diagnostic Session Control object
     * this will be used in MCU module.
     * 
     * @param logger A logger instance used to record information and errors during the execution.
     * @param socket The socket descriptor used for communication over the CAN bus.
     */
    DiagnosticSessionControl(Logger& logger, int socket);

    /**
     * @brief Destroy the Diagnostic Session Control object
     * 
     */
    ~DiagnosticSessionControl();

    /**
     * @brief Method to control the switch between sessions
     * 
     * @param id An unique identifier for the CAN frame.
     * @param sub_function A 1 byte value that specifies a particular diagnostic service.
     * @param is_tp Parameter true when the method is called by tester present, for not 
     * send the response
     */
    void sessionControl(canid_t frame_id, uint8_t sub_function, bool is_tp = false);

    /**
     * @brief Get the Current Session of object
     * 
     * @return DiagnosticSession 
     */
    static DiagnosticSession getCurrentSession();

    /**
     * @brief Get the Current Session of object, as a String
     * 
     * @return std::string + The current session
     */
    static std::string getCurrentSessionToString();
   
    static DiagnosticSession current_session;

private:
    Logger& dsc_logger;
    int socket = -1;

    /**
     * @brief Method to switch the current session
     * 
     * @param frame_id The id of the received frame.
     * @param session New session
     * @param is_tp Parameter true when the method is called by tester present, for not 
     * send the response
     */
    void switchSession(canid_t frame_id, DiagnosticSession session, bool is_tp = false);
};

#endif
