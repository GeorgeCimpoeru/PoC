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

enum DiagnosticSession {
    DEFAULT_SESSION,
    PROGRAMMING_SESSION
    /* Other sessions can be defined here */
};

class DiagnosticSessionControl {
public:
    DiagnosticSessionControl();
    ~DiagnosticSessionControl();
    void sessionControl(int id, int sub_function);
    void handleRequest(const uint8_t* request, size_t length);
    DiagnosticSession getCurrentSession() const;
    std::string getCurrentSessionToString() const;

private:
    int socket = -1;

    CreateInterface* canInterface;
    DiagnosticSession currentSession;

    void switchToDefaultSession();
    void sendNegativeResponse(uint8_t responseCode);
};

#endif
