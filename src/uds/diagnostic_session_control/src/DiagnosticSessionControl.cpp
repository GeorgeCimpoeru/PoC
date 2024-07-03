#include "../include/DiagnosticSessionControl.h"
#include <iostream>

#ifdef UNIT_TESTING_MODE
Logger dscLogger;
#else
Logger dscLogger("dscLogger", "logs/dscLogger.log");
#endif /* UNIT_TESTING_MODE */

DiagnosticSessionControl::DiagnosticSessionControl() : currentSession(DEFAULT_SESSION),
                                                       canInterface(CreateInterface::getInstance(0x00, dscLogger))
{
    LOG_INFO(dscLogger.GET_LOGGER(), "Diagnostic Session Control (0x10) started. Current session: {}", getCurrentSessionToString());
}

DiagnosticSessionControl::~DiagnosticSessionControl()
{
    LOG_INFO(dscLogger.GET_LOGGER(), "Diagnostic Session Control object out of scope");
}

/* Method to control the sessions of service */
void DiagnosticSessionControl::sessionControl(int id, int sub_function)
{
    uint8_t request[] = {id, sub_function};
    size_t requestLength = sizeof(request) / sizeof(request[0]);

    handleRequest(request, requestLength);
}

/* Method to handle the Request for switching the Sessions */
void DiagnosticSessionControl::handleRequest(const uint8_t *request, size_t length)
{
    /* Expecting exactly 2 bytes: SID + Sub-function */
    if (length != 2)
    {
        sendNegativeResponse(NR_INCORRECT_MESSAGE_LENGTH);
        return;
    }

    uint8_t sid = request[0];
    uint8_t subFunction = request[1];

    LOG_INFO(dscLogger.GET_LOGGER(), "Sessiom Control request, SID: 0x{:X} Sub-Function: 0x{:X}", sid, subFunction);

    if (sid == SID_DIAGNOSTIC_SESSION_CONTROL)
    {
        switch (subFunction)
        {
        case SUB_FUNCTION_DEFAULT_SESSION:
            switchToDefaultSession();
            break;
        default:
            /* std::cerr << "Unsupported sub-function" << std::endl; */
            LOG_ERROR(dscLogger.GET_LOGGER(), "Unsupported sub-function");
            /* Typically, another negative response could be added here */
            break;
        }
    }
    else
    {
        /* std::cerr << "Unsupported SID" << std::endl; */
        LOG_ERROR(dscLogger.GET_LOGGER(), "Unsupported SID");
        /* Other negative responses... */
    }
}

/* Method to switch current session to Default Session */
void DiagnosticSessionControl::switchToDefaultSession()
{
    /** Simulate an authentication check (for demonstration)
     * Replace with actual check */
    bool authenticated = true;
    if (!authenticated)
    {
        sendNegativeResponse(NR_AUTHENTICATION_FAILED);
        LOG_WARN(dscLogger.GET_LOGGER(), "Sent Negative Response with code {}", NR_AUTHENTICATION_FAILED);
        return;
    }

    /** Simulate resource check
     * Replace with actual check */
    bool resourceAvailable = true;
    if (!resourceAvailable)
    {
        sendNegativeResponse(NR_RESOURCE_TEMP_UNAVAILABLE);
        LOG_WARN(dscLogger.GET_LOGGER(), "Sent Negative Response with code {}", NR_RESOURCE_TEMP_UNAVAILABLE);
        return;
    }

    /* Switch to Default Session */
    currentSession = DEFAULT_SESSION;
    /* std::cout << "Switched to Default Session" << std::endl; */
    LOG_INFO(dscLogger.GET_LOGGER(), "Switched to Default Session. Current session: {}", getCurrentSessionToString());

    /* Create instance of Generate Frames to send response frame */
    GenerateFrames generateFrames(canInterface->getSocketEcuWrite(), dscLogger);

    /* Send response frame */
    generateFrames.sessionControl(0x1110, 0x01, true);
    LOG_INFO(dscLogger.GET_LOGGER(), "Sent pozitive response frame to MCU");
}

/* Method for sending Negative Response */
void DiagnosticSessionControl::sendNegativeResponse(uint8_t responseCode)
{
    /* std::cerr << "Sending Negative Response: " << static_cast<int>(responseCode) << std::endl; */
    LOG_INFO(dscLogger.GET_LOGGER(), "Sending Negative Response: {}", static_cast<int>(responseCode));
    /* TO DO: Implement logic to send the negative response */
}

/* Method to get the current session of module */
DiagnosticSession DiagnosticSessionControl::getCurrentSession() const
{
    return currentSession;
}

/* Method to get the current value of session as a String */
std::string DiagnosticSessionControl::getCurrentSessionToString() const
{
    switch (currentSession)
    {
    case DEFAULT_SESSION:
        return "DEFAULT_SESSION";
    case PROGRAMMING_SESSION:
        return "PROGRAMMING_SESSION";
    default:
        return "UNKNOWN_SESSION";
    }
}
