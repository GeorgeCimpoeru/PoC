#include "../include/RoutineControl.h"

RoutineControl::RoutineControl(int socket, Logger& rc_logger) 
            : generate_frames(socket, rc_logger), rc_logger(rc_logger)
{
    this->socket = socket;
}

/* Function to handle the RoutineControl request */
void RoutineControl::routineControl(canid_t can_id, const std::vector<uint8_t>& request)
{
    uint16_t routine_identifier = request[3] << 8 | request[4]; 
    std::vector<uint8_t> response;
    uint8_t lowerbits = can_id & 0xFF;
    uint8_t upperbits = can_id >> 8 & 0xFF;
    /* reverse ids */
    can_id = lowerbits << 8 | upperbits;
    if (request.size() < 6)
    {
        /* Incorrect message length or invalid format - prepare a negative response */
        generate_frames.negativeResponse(can_id, 0x31, 0x13);
        return;
    }
    else if (request[2] < 0x01 || request [2] > 0x03)
    {
        /* Sub Function not supported - prepare a negative response */
        generate_frames.negativeResponse(can_id, 0x31, 0x12);
        return;
    }
    /* when our identifiers will be defined, this range should be smaller */
    else if (routine_identifier < 0x0100 || routine_identifier > 0xEFFF)
    {
        /* Request Out of Range - prepare a negative response */
        generate_frames.negativeResponse(can_id, 0x31, 0x31);
        return;
    }
    else
    {
        switch(routine_identifier)
        {
            case 0x0101:
                /* Erase memory or specific data */
                /* call eraseMemory routine */
                generate_frames.routineControl(can_id, request[2], routine_identifier, true);
                LOG_INFO(rc_logger.GET_LOGGER(), "eraseMemory routine called.");
                break;
            case 0x0201:
                /* Install updates */
                /* call installUpdates routine*/
                generate_frames.routineControl(can_id, request[2], routine_identifier, true);
                LOG_INFO(rc_logger.GET_LOGGER(), "installUpdates routine called.");
                break;
            default:
                LOG_INFO(rc_logger.GET_LOGGER(), "Unknown routine.");
                break;
        }
    }
}