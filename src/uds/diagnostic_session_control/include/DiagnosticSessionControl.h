#ifndef DIAGNOSTICSESSIONCONTROL_H
#define DIAGNOSTICSESSIONCONTROL_H

#include <cstdlib>
#include "../../../utils/include/CreateInterface.h"
#include "../../../utils/include/GenerateFrames.h"
#include "../../../utils/include/Logger.h"

/* Diagnostic Control Session codes */
const uint8_t SID_DIAGNOSTIC_SESSION_CONTROL = 0x10;
const uint8_t SUB_FUNCTION_DEFAULT_SESSION = 1;
const uint8_t SUB_FUNCTION_PROGRAMMING_SESSION = 2;

/* Negative response codes */
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
    DiagnosticSessionControl();
    /**
     * @brief Construct a new Diagnostic Session Control object
     * with a parameter given for 'module_id'. For example, battery
     * will currently use 0x11 as 'module_id'.
     * 
     * @param module_id 
     */
    DiagnosticSessionControl(int module_id);
    ~DiagnosticSessionControl();

    /**
     * @brief Method to control the switch between sessions
     * 
     * @param id 
     * @param sub_function 
     */
    void sessionControl(int id, int sub_function);

    /**
     * @brief Method to handle the Request for switching the Sessions
     * 
     * @param request 
     * @param length 
     */
    void handleRequest(const uint8_t *request, size_t length);

    /**
     * @brief Get the Current Session of object
     * 
     * @return DiagnosticSession 
     */
    DiagnosticSession getCurrentSession() const;

    /**
     * @brief Get the Current Session of object, as a String
     * 
     * @return std::string 
     */
    std::string getCurrentSessionToString() const;

private:
    int module_id;
    CreateInterface *can_interface;
    DiagnosticSession current_session;
    void switchToDefaultSession();

    /**
     * @brief Method to handle the negative responses,
     * this will have to be improved in future.
     * 
     * @param response_code 
     */
    void sendNegativeResponse(uint8_t response_code);
};

#endif
