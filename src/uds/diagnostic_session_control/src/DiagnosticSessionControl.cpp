#include "../include/DiagnosticSessionControl.h"
#include "../../../ecu_simulation/BatteryModule/include/BatteryModule.h"
#include "../../../mcu/include/MCUModule.h"

// Initialize current_session
DiagnosticSession DiagnosticSessionControl::current_session = DEFAULT_SESSION;

/* Default constructor, used in MCU */
DiagnosticSessionControl::DiagnosticSessionControl(Logger* logger, int socket) : dsc_logger(logger)
{
    this->socket = socket;
    LOG_INFO(dsc_logger->GET_LOGGER(), "Diagnostic Session Control (0x10) started. Current session: {}", getCurrentSessionToString());
}

/* Parameterized constructor, used for ECUs */
DiagnosticSessionControl::DiagnosticSessionControl(int module_id, Logger* logger, int socket) : module_id(module_id), 
                                                                                   dsc_logger(logger)
{
    this->socket = socket;
    LOG_INFO(dsc_logger->GET_LOGGER(), "Diagnostic Session Control (0x10) started. Current session: {}", getCurrentSessionToString());
}

DiagnosticSessionControl::~DiagnosticSessionControl()
{
    LOG_INFO(dsc_logger->GET_LOGGER(), "Diagnostic Session Control object out of scope");
}

/* Method to control the sessions of service */
void DiagnosticSessionControl::sessionControl(canid_t frame_id, uint8_t sub_function)
{
    LOG_INFO(dsc_logger->GET_LOGGER(), "Sessiom Control request, SID: 0x{:X} Sub-Function: 0x{:X}", 0x10, sub_function);

    switch (sub_function)
    {
    case SUB_FUNCTION_DEFAULT_SESSION:
        switchToDefaultSession(frame_id);
        break;
    case SUB_FUNCTION_PROGRAMMING_SESSION:
        switchToProgrammingSession(frame_id);
        break;
    default:
        LOG_ERROR(dsc_logger->GET_LOGGER(), "Unsupported sub-function");
        NegativeResponse negative_response(socket, *dsc_logger);
        negative_response.sendNRC(frame_id, 0x10, 0x12);
        break;
    }
}

/* Method to switch current session to Default Session */
void DiagnosticSessionControl::switchToDefaultSession(canid_t frame_id)
{
    /* Switch to Default Session */
    current_session = DEFAULT_SESSION;

    LOG_INFO(dsc_logger->GET_LOGGER(), "Switched to Default Session. Current session: {}", getCurrentSessionToString());

    /* Create instance of Generate Frames to send response frame */
    GenerateFrames response_frame(socket, *dsc_logger);

    /** Check the module where the request was made from
     * More ECUs can be added here in future.
     * If no module_id is provided, request of sessionControl was made from MCU
     */

    /* Form the new id */
    int id = ((frame_id & 0xFF) << 8) | ((frame_id >> 8) & 0xFF);
    uint8_t receiver_id = frame_id & 0xFF;

    switch(receiver_id)
    {
        case 0x10:
            if (MCU::mcu->stop_flags.find(0x10) != MCU::mcu->stop_flags.end())
            {
                /* Send response frame */
                response_frame.sessionControl(id, 0x01, true);
                LOG_INFO(dsc_logger->GET_LOGGER(), "Sent pozitive response");
            } else
            {
                LOG_INFO(dsc_logger->GET_LOGGER(), "Service with SID {:x} failed to send the response frame.", 0x10);
                NegativeResponse negative_response(socket, *dsc_logger);
                negative_response.sendNRC(id, 0x10, 0x78);
            }
            break;
        case 0x11:
            if (battery->stop_flags.find(0x10) != battery->stop_flags.end())
            {
                /* Send response frame */
                response_frame.sessionControl(id, 0x01, true);
                LOG_INFO(dsc_logger->GET_LOGGER(), "Sent pozitive response");
            } else
            {
                LOG_INFO(dsc_logger->GET_LOGGER(), "Service with SID {:x} failed to send the response frame.", 0x10);
                NegativeResponse negative_response(socket, *dsc_logger);
                negative_response.sendNRC(id, 0x10, 0x78);
            }
            break;
        default:
            LOG_ERROR(dsc_logger->GET_LOGGER(), "Module with id {:x} not supported.", receiver_id);
    } 
}

/* Method to switch current session to Programming Session */
void DiagnosticSessionControl::switchToProgrammingSession(canid_t frame_id)
{
    /* Switch to Programming Session */
    current_session = PROGRAMMING_SESSION;

    LOG_INFO(dsc_logger->GET_LOGGER(), "Switched to Programming Session. Current session: {}", getCurrentSessionToString());

    /* Create instance of Generate Frames to send response frame */
    GenerateFrames response_frame(socket, *dsc_logger);

    /* Form the new id */
    int id = ((frame_id & 0xFF) << 8) | ((frame_id >> 8) & 0xFF);
    uint8_t receiver_id = frame_id & 0xFF;

    switch(receiver_id)
    {
        case 0x10:
            if (MCU::mcu->stop_flags.find(0x10) != MCU::mcu->stop_flags.end())
            {
                /* Send response frame */
                response_frame.sessionControl(id, 0x02, true);
                LOG_INFO(dsc_logger->GET_LOGGER(), "Service with SID {:x} successfully sent the response frame.", 0x10);
            } else
            {
                LOG_INFO(dsc_logger->GET_LOGGER(), "Service with SID {:x} failed to send the response frame.", 0x10);
                NegativeResponse negative_response(socket, *dsc_logger);
                negative_response.sendNRC(id, 0x10, 0x78);
            }
            break;
        case 0x11:
            if (battery->stop_flags.find(0x10) != battery->stop_flags.end())
            {
                /* Send response frame */
                response_frame.sessionControl(id, 0x02, true);
                LOG_INFO(dsc_logger->GET_LOGGER(), "Service with SID {:x} successfully sent the response frame.", 0x10);
            } else
            {
                LOG_INFO(dsc_logger->GET_LOGGER(), "Service with SID {:x} failed to send the response frame.", 0x10);
                NegativeResponse negative_response(socket, *dsc_logger);
                negative_response.sendNRC(id, 0x10, 0x78);
            }
            break;
        default:
            LOG_ERROR(dsc_logger->GET_LOGGER(), "Module with id {:x} not supported.", receiver_id);
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
    default:
        return "UNKNOWN_SESSION";
    }
}