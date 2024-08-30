#include "../include/AccessTimingParameter.h"
#include "../../../ecu_simulation/BatteryModule/include/BatteryModule.h"
#include "../../../ecu_simulation/EngineModule/include/EngineModule.h"
#include "../../../ecu_simulation/DoorsModule/include/DoorsModule.h"
#include "../../../mcu/include/MCUModule.h"

// Define static constants
const uint16_t AccessTimingParameter::DEFAULT_P2_MAX_TIME = 40;
const uint16_t AccessTimingParameter::DEFAULT_P2_STAR_MAX_TIME = 400;

// Define static variables
uint16_t AccessTimingParameter::p2_max_time = AccessTimingParameter::DEFAULT_P2_MAX_TIME;
uint16_t AccessTimingParameter::p2_star_max_time = AccessTimingParameter::DEFAULT_P2_STAR_MAX_TIME;

AccessTimingParameter::AccessTimingParameter(Logger logger, int socket) : atp_logger(logger),
                                                                            socket(socket)
{
    LOG_INFO(atp_logger.GET_LOGGER(), "Access Timing Parameter (0x83) started.");
}

AccessTimingParameter::~AccessTimingParameter()
{
    LOG_INFO(atp_logger.GET_LOGGER(), "Access Timing Parameter object out of scope.");
}

void AccessTimingParameter::handleRequest(canid_t frame_id, uint8_t sub_function, std::vector<uint8_t> frame_data)
{
    LOG_INFO(atp_logger.GET_LOGGER(), "Function that handle frames in ATP service called with subfunction 0x{:x}.", sub_function);\
    uint8_t receiver_id = frame_id & 0xFF;
    switch (sub_function)
    {
        case 0x01:
            readExtendedTimingParameters(frame_id);
            break;
        case 0x02:
            setTimingParametersToDefault(frame_id);
            break;
        case 0x03:
            readCurrentlyActiveTimingParameters(frame_id);
            break;
        case 0x04:
            if(frame_data.size() > 3)
            {
                setTimingParameters(frame_id, frame_data);
            }
            else
            {
                /* Call negative response service with NRC 0x13 */
                LOG_ERROR(atp_logger.GET_LOGGER(), "Incorrect Message Length Or Invalid Format");
                NegativeResponse negative_response(socket, atp_logger);
                negative_response.sendNRC(frame_id, 0x83, 0x13);
                if (receiver_id == 0x10)
                {
                    MCU::mcu->stop_flags[0x83] = false;
                } else if (receiver_id == 0x11)
                {
                    battery->stop_flags[0x83] = false;
                } else if (receiver_id == 0x12)
                {
                    engine->stop_flags[0x83] = false;
                } else if (receiver_id == 0x13)
                {
                    engine->stop_flags[0x83] = false;
                }
            }
            break;
        default:
            /* Call negative response service with NRC 0x12 */
            LOG_ERROR(atp_logger.GET_LOGGER(), "Unsupported sub-function");
            NegativeResponse negative_response(socket, atp_logger);
            negative_response.sendNRC(frame_id, 0x83, 0x12);
            if (receiver_id == 0x10)
            {
                MCU::mcu->stop_flags[0x83] = false;
            } else if (receiver_id == 0x11)
            {
                battery->stop_flags[0x83] = false;
            } else if (receiver_id == 0x12)
            {
                engine->stop_flags[0x83] = false;
            }
            else if (receiver_id == 0x13)
            {
                engine->stop_flags[0x83] = false;
            }
            break;
    }
}

void AccessTimingParameter::readExtendedTimingParameters(canid_t frame_id)
{
    LOG_INFO(atp_logger.GET_LOGGER(), "Read Extended Timing Parameters method invoked");

    LOG_INFO(atp_logger.GET_LOGGER(), "DEFAULT P2 MAX TIME: {} in miliseconds", DEFAULT_P2_MAX_TIME);
    LOG_INFO(atp_logger.GET_LOGGER(), "DEFAULT P2 STAR MAX TIME: {} in miliseconds", DEFAULT_P2_STAR_MAX_TIME);

    std::vector<uint8_t> response;
    /* PCI */
    response.push_back(0x06);
    /* Response sid */
    response.push_back(0xc3);
    /* Subfunction */
    response.push_back(0x01);
    /* Add the default P2 max time (split into two bytes) */
    /* Most significant byte */
    response.push_back(static_cast<uint8_t>(DEFAULT_P2_MAX_TIME >> 8));
    /* Least significant byte */
    response.push_back(static_cast<uint8_t>(DEFAULT_P2_MAX_TIME & 0xFF));
    /* Add the default P2 star max time (split into two bytes) */
    /* Most significant byte */
    response.push_back(static_cast<uint8_t>(DEFAULT_P2_STAR_MAX_TIME >> 8));
    /* Least significant byte */
    response.push_back(static_cast<uint8_t>(DEFAULT_P2_STAR_MAX_TIME & 0xFF));

    GenerateFrames response_frame(socket, atp_logger);
    int id = ((frame_id & 0xFF) << 8) | ((frame_id >> 8) & 0xFF);

    uint8_t receiver_id = frame_id & 0xFF;
    switch(receiver_id)
        {
            case 0x10:
                if (MCU::mcu->stop_flags.find(0x83) != MCU::mcu->stop_flags.end())
                {
                    /* Send response frame */
                    LOG_INFO(atp_logger.GET_LOGGER(), "Service with SID {:x} successfully sent the response frame.", 0x83);
                    response_frame.accessTimingParameters(id, 0x01, response, true);
                } else
                {
                    LOG_INFO(atp_logger.GET_LOGGER(), "Service with SID {:x} failed to send the response frame.", 0x83);
                    NegativeResponse negative_response(socket, atp_logger);
                    negative_response.sendNRC(id, 0x83, 0x78);
                }
                break;
            case 0x11:
                if (battery->stop_flags.find(0x83) != battery->stop_flags.end())
                {
                    /* Send response frame */
                    LOG_INFO(atp_logger.GET_LOGGER(), "Service with SID {:x} successfully sent the response frame.", 0x83);
                    response_frame.accessTimingParameters(id, 0x01, response, true);
                } else
                {
                    LOG_INFO(atp_logger.GET_LOGGER(), "Service with SID {:x} failed to send the response frame.", 0x83);
                    NegativeResponse negative_response(socket, atp_logger);
                    negative_response.sendNRC(id, 0x83, 0x78);
                }
                break;
            case 0x12:
                if (engine->stop_flags.find(0x83) != engine->stop_flags.end())
                {
                    /* Send response frame */
                    LOG_INFO(atp_logger.GET_LOGGER(), "Service with SID {:x} successfully sent the response frame.", 0x83);
                    response_frame.accessTimingParameters(id, 0x01, response, true);
                } else
                {
                    LOG_INFO(atp_logger.GET_LOGGER(), "Service with SID {:x} failed to send the response frame.", 0x83);
                    NegativeResponse negative_response(socket, atp_logger);
                    negative_response.sendNRC(id, 0x83, 0x78);
                }
                case 0x13:
                if (doors->stop_flags.find(0x83) != doors->stop_flags.end())
                {
                    /* Send response frame */
                    LOG_INFO(atp_logger.GET_LOGGER(), "Service with SID {:x} successfully sent the response frame.", 0x83);
                    response_frame.accessTimingParameters(id, 0x01, response, true);
                } else
                {
                    LOG_INFO(atp_logger.GET_LOGGER(), "Service with SID {:x} failed to send the response frame.", 0x83);
                    NegativeResponse negative_response(socket, atp_logger);
                    negative_response.sendNRC(id, 0x83, 0x78);
                }
                break;
            default:
                LOG_ERROR(atp_logger.GET_LOGGER(), "Module with id {:x} not supported.", receiver_id);
        }
        if (receiver_id == 0x10) {
            MCU::mcu->stop_flags[0x83] = false;
        } else if (receiver_id == 0x11) {
            battery->stop_flags[0x83] = false;
        } else if (receiver_id == 0x12) {
            engine->stop_flags[0x83] = false;
        }
        else if (receiver_id == 0x13) {
            doors->stop_flags[0x83] = false;
        }
}

void AccessTimingParameter::setTimingParametersToDefault(canid_t frame_id)
{
    LOG_INFO(atp_logger.GET_LOGGER(), "Read Extended Timing Parameters method invoked");

    LOG_INFO(atp_logger.GET_LOGGER(), "Current P2 Max Time: {} in miliseconds", p2_max_time);
    LOG_INFO(atp_logger.GET_LOGGER(), "Current P2 Star Max Time: {} in miliseconds", p2_star_max_time);

    p2_max_time = DEFAULT_P2_MAX_TIME;
    p2_star_max_time = DEFAULT_P2_STAR_MAX_TIME;

    LOG_INFO(atp_logger.GET_LOGGER(), "Updated P2 Max Time to default: {} in miliseconds", p2_max_time);
    LOG_INFO(atp_logger.GET_LOGGER(), "Updated P2 Star Max Time to default: {} in miliseconds", p2_star_max_time);

    GenerateFrames response_frame(socket, atp_logger);
    int id = ((frame_id & 0xFF) << 8) | ((frame_id >> 8) & 0xFF);

    uint8_t receiver_id = frame_id & 0xFF;
    switch(receiver_id)
        {
            case 0x10:
                if (MCU::mcu->stop_flags.find(0x83) != MCU::mcu->stop_flags.end())
                {
                    /* Send response frame */
                    LOG_INFO(atp_logger.GET_LOGGER(), "Service with SID {:x} successfully sent the response frame.", 0x83);
                    response_frame.accessTimingParameters(id, 0x02, {}, true);
                } else
                {
                    LOG_INFO(atp_logger.GET_LOGGER(), "Service with SID {:x} failed to send the response frame.", 0x83);
                    NegativeResponse negative_response(socket, atp_logger);
                    negative_response.sendNRC(id, 0x83, 0x78);
                }
                break;
            case 0x11:
                if (battery->stop_flags.find(0x83) != battery->stop_flags.end())
                {
                    /* Send response frame */
                    LOG_INFO(atp_logger.GET_LOGGER(), "Service with SID {:x} successfully sent the response frame.", 0x83);
                    response_frame.accessTimingParameters(id, 0x02, {}, true);
                } else
                {
                    LOG_INFO(atp_logger.GET_LOGGER(), "Service with SID {:x} failed to send the response frame.", 0x83);
                    NegativeResponse negative_response(socket, atp_logger);
                    negative_response.sendNRC(id, 0x83, 0x78);
                }
                break;
            case 0x12:
                if (engine->stop_flags.find(0x83) != engine->stop_flags.end())
                {
                    /* Send response frame */
                    LOG_INFO(atp_logger.GET_LOGGER(), "Service with SID {:x} successfully sent the response frame.", 0x83);
                    response_frame.accessTimingParameters(id, 0x02, {}, true);
                } else
                {
                    LOG_INFO(atp_logger.GET_LOGGER(), "Service with SID {:x} failed to send the response frame.", 0x83);
                    NegativeResponse negative_response(socket, atp_logger);
                    negative_response.sendNRC(id, 0x83, 0x78);
                }
                break;
            case 0x13:
                if (doors->stop_flags.find(0x83) != doors->stop_flags.end())
                {
                    /* Send response frame */
                    LOG_INFO(atp_logger.GET_LOGGER(), "Service with SID {:x} successfully sent the response frame.", 0x83);
                    response_frame.accessTimingParameters(id, 0x02, {}, true);
                } else
                {
                    LOG_INFO(atp_logger.GET_LOGGER(), "Service with SID {:x} failed to send the response frame.", 0x83);
                    NegativeResponse negative_response(socket, atp_logger);
                    negative_response.sendNRC(id, 0x83, 0x78);
                }
                break;
            default:
                LOG_ERROR(atp_logger.GET_LOGGER(), "Module with id {:x} not supported.", receiver_id);
        }
        if (receiver_id == 0x10) {
            MCU::mcu->stop_flags[0x83] = false;
        } else if (receiver_id == 0x11) {
            battery->stop_flags[0x83] = false;
        } else if (receiver_id == 0x12) {
            engine->stop_flags[0x83] = false;
        }
        else if (receiver_id == 0x13) {
            doors->stop_flags[0x83] = false;
        }
}

void AccessTimingParameter::readCurrentlyActiveTimingParameters(canid_t frame_id)
{
    LOG_INFO(atp_logger.GET_LOGGER(), "Read Currently Active Timing Parameters method invoked");

    LOG_INFO(atp_logger.GET_LOGGER(), "Current P2 Max Time: {} in miliseconds", p2_max_time);
    LOG_INFO(atp_logger.GET_LOGGER(), "Current P2 Star Max Time: {} in miliseconds", p2_star_max_time);

    std::vector<uint8_t> response;
    /* PCI */
    response.push_back(0x02);
    /* Response sid */
    response.push_back(0xc3);
    /* Subfunction */
    response.push_back(0x03);
    /* Add the default P2 max time (split into two bytes) */
    /* Most significant byte */
    response.push_back(static_cast<uint8_t>(p2_max_time >> 8));
    /* Least significant byte */
    response.push_back(static_cast<uint8_t>(p2_max_time & 0xFF));
    /* Add the default P2 star max time (split into two bytes) */
    /* Most significant byte */
    response.push_back(static_cast<uint8_t>(p2_star_max_time >> 8));
    /* Least significant byte */
    response.push_back(static_cast<uint8_t>(p2_star_max_time & 0xFF));

    GenerateFrames response_frame(socket, atp_logger);
    int id = ((frame_id & 0xFF) << 8) | ((frame_id >> 8) & 0xFF);

    uint8_t receiver_id = frame_id & 0xFF;
    switch(receiver_id)
        {
            case 0x10:
                if (MCU::mcu->stop_flags.find(0x83) != MCU::mcu->stop_flags.end())
                {
                    /* Send response frame */
                    LOG_INFO(atp_logger.GET_LOGGER(), "Service with SID {:x} successfully sent the response frame.", 0x83);
                    response_frame.accessTimingParameters(id, 0x03, response, true);
                } else
                {
                    LOG_INFO(atp_logger.GET_LOGGER(), "Service with SID {:x} failed to send the response frame.", 0x83);
                    NegativeResponse negative_response(socket, atp_logger);
                    negative_response.sendNRC(id, 0x83, 0x78);
                }
                break;
            case 0x11:
                if (battery->stop_flags.find(0x83) != battery->stop_flags.end())
                {
                    /* Send response frame */
                    LOG_INFO(atp_logger.GET_LOGGER(), "Service with SID {:x} successfully sent the response frame.", 0x83);
                    response_frame.accessTimingParameters(id, 0x03, response, true);
                } else
                {
                    LOG_INFO(atp_logger.GET_LOGGER(), "Service with SID {:x} failed to send the response frame.", 0x83);
                    NegativeResponse negative_response(socket, atp_logger);
                    negative_response.sendNRC(id, 0x83, 0x78);
                }
                break;
            case 0x12:
                if (engine->stop_flags.find(0x83) != engine->stop_flags.end())
                {
                    /* Send response frame */
                    LOG_INFO(atp_logger.GET_LOGGER(), "Service with SID {:x} successfully sent the response frame.", 0x83);
                    response_frame.accessTimingParameters(id, 0x03, response, true);
                } else
                {
                    LOG_INFO(atp_logger.GET_LOGGER(), "Service with SID {:x} failed to send the response frame.", 0x83);
                    NegativeResponse negative_response(socket, atp_logger);
                    negative_response.sendNRC(id, 0x83, 0x78);
                }
                break;
            case 0x13:
                if (doors->stop_flags.find(0x83) != doors->stop_flags.end())
                {
                    /* Send response frame */
                    LOG_INFO(atp_logger.GET_LOGGER(), "Service with SID {:x} successfully sent the response frame.", 0x83);
                    response_frame.accessTimingParameters(id, 0x03, response, true);
                } else
                {
                    LOG_INFO(atp_logger.GET_LOGGER(), "Service with SID {:x} failed to send the response frame.", 0x83);
                    NegativeResponse negative_response(socket, atp_logger);
                    negative_response.sendNRC(id, 0x83, 0x78);
                }
                break;
            default:
                LOG_ERROR(atp_logger.GET_LOGGER(), "Module with id {:x} not supported.", receiver_id);
        }
        if (receiver_id == 0x10) {
            MCU::mcu->stop_flags[0x83] = false;
        } else if (receiver_id == 0x11) {
            battery->stop_flags[0x83] = false;
        } else if (receiver_id == 0x12) {
            engine->stop_flags[0x83] = false;
        }
        else if (receiver_id == 0x13) {
            engine->stop_flags[0x83] = false;
        }
}

void AccessTimingParameter::setTimingParameters(canid_t frame_id, std::vector<uint8_t> frame_data)
{
    LOG_INFO(atp_logger.GET_LOGGER(), "Set Timing Parameters method invoked");
    
    LOG_INFO(atp_logger.GET_LOGGER(), "Current P2 Max Time: {} in miliseconds", p2_max_time);
    LOG_INFO(atp_logger.GET_LOGGER(), "Current P2 Star Max Time: {} in miliseconds", p2_star_max_time);

    /* Extract the values ​​for p2_max_time (frame_data[0] and frame_data[1]) */
    p2_max_time = (frame_data[0] << 8) | frame_data[1];
    /* Extract the values ​​for p2_star_max_time (frame_data[2] and frame_data[3]) */
    p2_star_max_time = (frame_data[2] << 8) | frame_data[3];

    LOG_INFO(atp_logger.GET_LOGGER(), "Updated P2 Max Time: {} in miliseconds", p2_max_time);
    LOG_INFO(atp_logger.GET_LOGGER(), "Updated P2 Star Max Time: {} in miliseconds", p2_star_max_time);

    std::vector<uint8_t> response;
    /* PCI */
    response.push_back(0x02);
    /* Response sid */
    response.push_back(0xc3);
    /* Subfunction */
    response.push_back(0x04);

    GenerateFrames response_frame(socket, atp_logger);
    int id = ((frame_id & 0xFF) << 8) | ((frame_id >> 8) & 0xFF);

    uint8_t receiver_id = frame_id & 0xFF;
    switch(receiver_id)
        {
            case 0x10:
                /* Send response frame */
                LOG_INFO(atp_logger.GET_LOGGER(), "Service with SID {:x} successfully sent the response frame.", 0x83);
                response_frame.accessTimingParameters(id, 0x04, response, true);
                MCU::mcu->stop_flags[0x83] = false;
                break;
            case 0x11:
                /* Send response frame */
                LOG_INFO(atp_logger.GET_LOGGER(), "Service with SID {:x} successfully sent the response frame.", 0x83);
                response_frame.accessTimingParameters(id, 0x04, response, true);
                battery->stop_flags[0x83] = false;
                break;
            case 0x12:
                /* Send response frame */
                LOG_INFO(atp_logger.GET_LOGGER(), "Service with SID {:x} successfully sent the response frame.", 0x83);
                response_frame.accessTimingParameters(id, 0x04, response, true);
                engine->stop_flags[0x83] = false;
                break;
            case 0x13:
                /* Send response frame */
                LOG_INFO(atp_logger.GET_LOGGER(), "Service with SID {:x} successfully sent the response frame.", 0x83);
                response_frame.accessTimingParameters(id, 0x04, response, true);
                doors->stop_flags[0x83] = false;
                break;
            default:
                LOG_ERROR(atp_logger.GET_LOGGER(), "Module with id {:x} not supported.", receiver_id);
        }
}
