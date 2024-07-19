#include "../include/DiagnosticSessionControl.h"
#include <iostream>

/* Default constructor, used in MCU */
DiagnosticSessionControl::DiagnosticSessionControl(Logger logger, int socket) : dsc_logger(logger),
                                                                    current_session(DEFAULT_SESSION)
{
    this->socket = socket;
    LOG_INFO(dsc_logger.GET_LOGGER(), "Diagnostic Session Control (0x10) started. Current session: {}", getCurrentSessionToString());
}

/* Parameterized constructor, used for ECUs */
DiagnosticSessionControl::DiagnosticSessionControl(int module_id, Logger logger, int socket) : module_id(module_id), 
                                                                                   dsc_logger(logger),
                                                                                   current_session(DEFAULT_SESSION)
{
    this->socket = socket;
    LOG_INFO(dsc_logger.GET_LOGGER(), "Diagnostic Session Control (0x10) started. Current session: {}", getCurrentSessionToString());
}

DiagnosticSessionControl::~DiagnosticSessionControl()
{
    LOG_INFO(dsc_logger.GET_LOGGER(), "Diagnostic Session Control object out of scope");
}

/* Method to control the sessions of service */
void DiagnosticSessionControl::sessionControl(uint8_t id, uint8_t sub_function)
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
    uint8_t sub_function = request[1];

    LOG_INFO(dsc_logger.GET_LOGGER(), "Sessiom Control request, SID: 0x{:X} Sub-Function: 0x{:X}", sid, sub_function);

    if (sid == SID_DIAGNOSTIC_SESSION_CONTROL)
    {
        switch (sub_function)
        {
        case SUB_FUNCTION_DEFAULT_SESSION:
            switchToDefaultSession();
            break;
        default:
            LOG_ERROR(dsc_logger.GET_LOGGER(), "Unsupported sub-function");
            /* Other negative responses can be added here */
            break;
        }
    }
    else
    {
        LOG_ERROR(dsc_logger.GET_LOGGER(), "Unsupported SID");
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
        LOG_WARN(dsc_logger.GET_LOGGER(), "Sent Negative Response with code {}", NR_AUTHENTICATION_FAILED);
        return;
    }

    /** Simulate resource check
     * Replace with actual check */
    bool resource_available = true;
    if (!resource_available)
    {
        sendNegativeResponse(NR_RESOURCE_TEMP_UNAVAILABLE);
        LOG_WARN(dsc_logger.GET_LOGGER(), "Sent Negative Response with code {}", NR_RESOURCE_TEMP_UNAVAILABLE);
        return;
    }

    /* Switch to Default Session */
    current_session = DEFAULT_SESSION;
    /* std::cout << "Switched to Default Session" << std::endl; */
    LOG_INFO(dsc_logger.GET_LOGGER(), "Switched to Default Session. Current session: {}", getCurrentSessionToString());

    /* Create instance of Generate Frames to send response frame */
    GenerateFrames response_frame(socket, dsc_logger);

    /** Check the module where the request was made from
     * More ECUs can be added here in future.
     * If no module_id is provided, request of sessionControl was made from MCU
     */
    if (this->module_id == 0x11)
    {
        /* Send response frame to ECU */
        response_frame.sessionControl(0x1011, 0x01, true);
        LOG_INFO(dsc_logger.GET_LOGGER(), "Sent pozitive response frame to ECU");
    }
    else
    {
        /* Send response frame to MCU */
        response_frame.sessionControl(0x1110, 0x01, true);
        LOG_INFO(dsc_logger.GET_LOGGER(), "Sent pozitive response frame to MCU");
    }
}

/* Method for sending Negative Response */
void DiagnosticSessionControl::sendNegativeResponse(uint8_t responseCode)
{
    /* std::cerr << "Sending Negative Response: " << static_cast<int>(responseCode) << std::endl; */
    LOG_INFO(dsc_logger.GET_LOGGER(), "Sending Negative Response: {}", static_cast<int>(responseCode));
    /* TO DO: Implement logic to send the negative response */
}

/* Method to get the current session of module */
DiagnosticSession DiagnosticSessionControl::getCurrentSession() const
{
    return current_session;
}

/* Method to get the current value of session as a String */
std::string DiagnosticSessionControl::getCurrentSessionToString() const
{
    switch (current_session)
    {
    case DEFAULT_SESSION:
        return "DEFAULT_SESSION";
    case PROGRAMMING_SESSION:
        return "PROGRAMMING_SESSION";
    default:
        return "UNKNOWN_SESSION";
    }
}
