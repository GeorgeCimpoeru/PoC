#include "../include/DiagnosticSessionControl.h"
#include "../../../ecu_simulation/BatteryModule/include/BatteryModule.h"
#include "../../../ecu_simulation/EngineModule/include/EngineModule.h"
#include "../../../ecu_simulation/DoorsModule/include/DoorsModule.h"
#include "../../../ecu_simulation/HVACModule/include/HVACModule.h"
#include "../../../mcu/include/MCUModule.h"

// Initialize current_session
DiagnosticSession DiagnosticSessionControl::current_session = DEFAULT_SESSION;

/* Default constructor, used in MCU */
DiagnosticSessionControl::DiagnosticSessionControl(Logger& logger, int socket) : dsc_logger(logger)
{
    this->socket = socket;
    LOG_INFO(dsc_logger.GET_LOGGER(), "Diagnostic Session Control (0x10) started. Current session: {}", getCurrentSessionToString());
}

DiagnosticSessionControl::~DiagnosticSessionControl()
{
    LOG_INFO(dsc_logger.GET_LOGGER(), "Diagnostic Session Control object out of scope");
}

/* Method to control the sessions of service */
void DiagnosticSessionControl::sessionControl(canid_t frame_id, uint8_t sub_function, bool is_tp)
{
    LOG_INFO(dsc_logger.GET_LOGGER(), "Session Control request, SID: 0x{:X} Sub-Function: 0x{:X}", 0x10, sub_function);

    switch (sub_function)
    {
    case SUB_FUNCTION_DEFAULT_SESSION:
        switchSession(frame_id, DEFAULT_SESSION, is_tp);
        break;
    case SUB_FUNCTION_PROGRAMMING_SESSION:
        switchSession(frame_id, PROGRAMMING_SESSION, is_tp);
        break;
    case SUB_FUNCTION_EXTENDED_DIAGNOSTIC_SESSION:
        switchSession(frame_id, EXTENDED_DIAGNOSTIC_SESSION, is_tp);
        break;
    default:
        LOG_ERROR(dsc_logger.GET_LOGGER(), "Unsupported sub-function");
        NegativeResponse negative_response(socket, dsc_logger);
        /* Form the new id */
        int id = ((frame_id & 0xFF) << 8) | ((frame_id >> 8) & 0xFF);
        negative_response.sendNRC(id, 0x10, 0x12);
        uint8_t receiver_id = frame_id & 0xFF;
        AccessTimingParameter::stopTimingFlag(receiver_id, 0x10);
        break;
    }
}

void DiagnosticSessionControl::switchSession(canid_t frame_id, DiagnosticSession session, bool is_tp)
{
    if (!is_tp)
    {
        LOG_INFO(dsc_logger.GET_LOGGER(), "Session before change: {}", getCurrentSessionToString());
        /* Switch to requested Session */
        current_session = session;

        LOG_INFO(dsc_logger.GET_LOGGER(), "Current session: {}", getCurrentSessionToString());

        /* Create instance of Generate Frames to send response frame */
        GenerateFrames response_frame(socket, dsc_logger);

        /* Form the new id */
        int id = ((frame_id & 0xFF) << 8) | ((frame_id >> 8) & 0xFF);
        uint8_t receiver_id = frame_id & 0xFF;

        response_frame.sessionControl(id, static_cast<uint8_t>(session), true);
        LOG_INFO(dsc_logger.GET_LOGGER(), "Sent positive response");
        
        AccessTimingParameter::stopTimingFlag(receiver_id, 0x10);
    }
    else
    {
        current_session = session;
    }
}
/* Method to get the current session of module */
DiagnosticSession DiagnosticSessionControl::getCurrentSession()
{
    return current_session;
}

/* Method to get the current value of session as a String */
std::string DiagnosticSessionControl::getCurrentSessionToString()
{
    switch (current_session)
    {
    case DEFAULT_SESSION:
        return "DEFAULT_SESSION";
    case PROGRAMMING_SESSION:
        return "PROGRAMMING_SESSION";
    case EXTENDED_DIAGNOSTIC_SESSION:
        return "EXTENDED_DIAGNOSTIC_SESSION";
    default:
        return "UNKNOWN_SESSION";
    }
}