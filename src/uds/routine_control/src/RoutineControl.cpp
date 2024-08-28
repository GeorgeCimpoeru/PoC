#include "../include/RoutineControl.h"
#include "../../../ecu_simulation/BatteryModule/include/BatteryModule.h"
#include "../../../ecu_simulation/EngineModule/include/EngineModule.h"
#include "../../../mcu/include/MCUModule.h"

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
    NegativeResponse nrc(socket, rc_logger);
    uint8_t lowerbits = can_id & 0xFF;
    uint8_t upperbits = can_id >> 8 & 0xFF;
    /* reverse ids */
    can_id = lowerbits << 8 | upperbits;
    if (request.size() < 6)
    {
        /* Incorrect message length or invalid format - prepare a negative response */
        nrc.sendNRC(can_id,ROUTINE_CONTROL_SID,NegativeResponse::IMLOIF);
        if (lowerbits == 0x10)
        {
            MCU::mcu->stop_flags[0x31] = false;
        } else if (lowerbits == 0x11)
        {
            battery->stop_flags[0x31] = false;
        } else if (lowerbits == 0x12)
        {
            engine->stop_flags[0x31] = false;
        }
        return;
    }
    else if (request[2] < 0x01 || request [2] > 0x03)
    {
        /* Sub Function not supported - prepare a negative response */
        nrc.sendNRC(can_id,ROUTINE_CONTROL_SID,NegativeResponse::SFNS);
        if (lowerbits == 0x10)
        {
            MCU::mcu->stop_flags[0x31] = false;
        } else if (lowerbits == 0x11)
        {
            battery->stop_flags[0x31] = false;
        } else if (lowerbits == 0x12)
        {
            engine->stop_flags[0x31] = false;
        }
        return;
    }
    else if (!SecurityAccess::getMcuState(rc_logger))
    {
        nrc.sendNRC(can_id,ROUTINE_CONTROL_SID,NegativeResponse::SAD);
        if (lowerbits == 0x10)
        {
            MCU::mcu->stop_flags[0x31] = false;
        } else if (lowerbits == 0x11)
        {
            battery->stop_flags[0x31] = false;
        } else if (lowerbits == 0x12)
        {
            engine->stop_flags[0x31] = false;
        }
    }
    /* when our identifiers will be defined, this range should be smaller */
    else if (routine_identifier < 0x0100 || routine_identifier > 0xEFFF)
    {
        /* Request Out of Range - prepare a negative response */
        nrc.sendNRC(can_id,ROUTINE_CONTROL_SID,NegativeResponse::ROOR);
        if (lowerbits == 0x10)
        {
            MCU::mcu->stop_flags[0x31] = false;
        } else if (lowerbits == 0x11)
        {
            battery->stop_flags[0x31] = false;
        } else if (lowerbits == 0x12)
        {
            engine->stop_flags[0x31] = false;
        }
        return;
    }
    else
    {
        switch(routine_identifier)
        {
            case 0x0101:
                /* Erase memory or specific data */
                /* call eraseMemory routine */
                switch(lowerbits)
                {
                    case 0x10:
                        /* Send response frame */
                        generate_frames.routineControl(can_id, request[2], routine_identifier, true);
                        LOG_INFO(rc_logger.GET_LOGGER(), "Service with SID {:x} successfully sent the response frame: eraseMemory routine.", 0x31);
                        MCU::mcu->stop_flags[0x31] = false;
                        break;
                    case 0x11:
                        /* Send response frame */
                        generate_frames.routineControl(can_id, request[2], routine_identifier, true);
                        LOG_INFO(rc_logger.GET_LOGGER(), "Service with SID {:x} successfully sent the response frame: eraseMemory routine.", 0x31);
                        battery->stop_flags[0x31] = false;
                        break;
                    case 0x12:
                        /* Send response frame */
                        generate_frames.routineControl(can_id, request[2], routine_identifier, true);
                        LOG_INFO(rc_logger.GET_LOGGER(), "Service with SID {:x} successfully sent the response frame: eraseMemory routine.", 0x31);
                        engine->stop_flags[0x31] = false;
                        break;
                    default:
                        LOG_ERROR(rc_logger.GET_LOGGER(), "Module with id {:x} not supported.", lowerbits);
                }
                break;
            case 0x0201:
                /* Install updates */
                /* call installUpdates routine*/
                switch(lowerbits)
                {
                    case 0x10:
                        /* Send response frame */
                        generate_frames.routineControl(can_id, request[2], routine_identifier, true);
                        LOG_INFO(rc_logger.GET_LOGGER(), "Service with SID {:x} successfully sent the response frame: installUpdates routine.", 0x31);
                        MCU::mcu->stop_flags[0x31] = false;
                        break;
                    case 0x11:
                        /* Send response frame */
                        generate_frames.routineControl(can_id, request[2], routine_identifier, true);
                        LOG_INFO(rc_logger.GET_LOGGER(), "Service with SID {:x} successfully sent the response frame: installUpdates routine.", 0x31);
                        battery->stop_flags[0x31] = false;
                        break;
                    case 0x12:
                        /* Send response frame */
                        generate_frames.routineControl(can_id, request[2], routine_identifier, true);
                        LOG_INFO(rc_logger.GET_LOGGER(), "Service with SID {:x} successfully sent the response frame: installUpdates routine.", 0x31);
                        engine->stop_flags[0x31] = false;
                        break;
                    default:
                        LOG_ERROR(rc_logger.GET_LOGGER(), "Module with id {:x} not supported.", lowerbits);
                }
                break;
            default:
                LOG_INFO(rc_logger.GET_LOGGER(), "Unknown routine.");
                break;
        }
    }
}