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
#include "../../../utils/include/GenerateFrames.h"
#include "../../../utils/include/Logger.h"
/* Diagnostic Control Session codes */
const uint8_t SID_DIAGNOSTIC_SESSION_CONTROL = 0x10;
const uint8_t SUB_FUNCTION_DEFAULT_SESSION = 1;
const uint8_t SUB_FUNCTION_PROGRAMMING_SESSION = 2;

/* Negative response codes */
const uint8_t NR_SUBFUNCION_NOT_SUPPORTED = 0x12;
const uint8_t NR_INCORRECT_MESSAGE_LENGTH = 0x13;
const uint8_t NR_AUTHENTICATION_FAILED = 0x34;
const uint8_t NR_RESOURCE_TEMP_UNAVAILABLE = 0x94;

enum DiagnosticSession
{
    DEFAULT_SESSION,
    PROGRAMMING_SESSION
    /* Other sessions can be defined here */
};

class DiagnosticSessionControl
{
public:
    /**
     * @brief Construct a new Diagnostic Session Control object
     * this will be used in MCU module.
     * 
     */
    DiagnosticSessionControl(Logger logger, int socket);
    /**
     * @brief Construct a new Diagnostic Session Control object
     * with a parameter given for 'module_id'. For example, battery
     * will currently use 0x11 as 'module_id'.
     * 
     * @param module_id 
     */
    DiagnosticSessionControl(int module_id, Logger logger, int socket);

    /**
     * @brief Destroy the Diagnostic Session Control object
     * 
     */
    ~DiagnosticSessionControl();

    /**
     * @brief Method to control the switch between sessions
     * 
     * @param id 
     * @param sub_function 
     */
    void sessionControl(canid_t frame_id, uint8_t sub_function);

    /**
     * @brief Get the Current Session of object
     * 
     * @return DiagnosticSession 
     */
    static DiagnosticSession getCurrentSession();

    /**
     * @brief Get the Current Session of object, as a String
     * 
     * @return std::string +
     */
    static std::string getCurrentSessionToString();

    /**
     * @brief Method that call the negative response service
     * 
     * @param response_code 
     */
    void sendNegativeResponse(uint8_t response_code);

private:
    int module_id;
    Logger dsc_logger;
    static DiagnosticSession current_session;
    int socket = -1;
    void switchToDefaultSession(canid_t frame_id);
    void switchToProgrammingSession(canid_t frame_id);
};

#endif
