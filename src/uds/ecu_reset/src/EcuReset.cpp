#include "../include/EcuReset.h"

EcuReset::EcuReset(uint32_t can_id, uint8_t sub_function, int socket, Logger &logger)
    : can_id(can_id), sub_function(sub_function), ECUResetLog(logger), generate_frames(socket, logger)
{
    this->socket = socket;
}

EcuReset::~EcuReset()
{
}

void EcuReset::ecuResetRequest()
{
    /* Hard Reset case */
    if (sub_function == 0x01) {
        LOG_INFO(ECUResetLog.GET_LOGGER(), "EcuReset: mode Hard Reset");
        this->hardReset();
    /* Keys off Reset case */
    } else if (sub_function == 0x02) {
        LOG_INFO(ECUResetLog.GET_LOGGER(), "EcuReset: mode Key Off Reset");
        this->keyOffReset();
    }
}
void EcuReset::hardReset()
{
    /* Deletes the interface */
    uint8_t interface_name = interface->getInterfaceName();
    LOG_INFO(ECUResetLog.GET_LOGGER(), "ECUReset: interface deleted {}", interface_name);
    interface->deleteInterface();

    /* Recreate the interface */
    LOG_INFO(ECUResetLog.GET_LOGGER(), "ECUReset: interface created {}", interface_name);
    interface = new CreateInterface(interface_name, ECUResetLog);

    sleep(4);

    /* Send response */
    this->ecuResetResponse();
}

void EcuReset::keyOffReset()
{
    /* Turns down the interface */
    uint8_t interface_name = interface->getInterfaceName();

    LOG_INFO(ECUResetLog.GET_LOGGER(), "ECUReset: interface down {}", interface_name);
    interface->stopInterface();

    /* Turns up the interface */
    LOG_INFO(ECUResetLog.GET_LOGGER(), "ECUReset: interface up: {}", interface_name);
    interface->startInterface();

    /* Sens response */
    this->ecuResetResponse();
}

void EcuReset::ecuResetResponse()
{
    /* Generate the response frame and send it */
    LOG_INFO(ECUResetLog.GET_LOGGER(), "ECUReset: Response sent");


    uint8_t frame_dest_id = (can_id >> 8) & 0xFF;
    LOG_INFO(ECUResetLog.GET_LOGGER(), "frame_dest_id = 0x{0:x}", frame_dest_id);

    uint8_t frame_sender_id = can_id & 0xFF;
    LOG_INFO(ECUResetLog.GET_LOGGER(), "frame_sender_id = 0x{0:x}", frame_sender_id);
    
    can_id = (frame_sender_id << 8) | frame_dest_id;
    LOG_INFO(ECUResetLog.GET_LOGGER(), "can_id = 0x{0:x}", can_id);

    generate_frames.ecuReset(can_id, sub_function, socket, true);
}
