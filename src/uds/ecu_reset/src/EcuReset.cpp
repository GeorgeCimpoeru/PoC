#include "../include/EcuReset.h"

EcuReset::EcuReset(uint32_t can_id, uint8_t sub_function, int socket, Logger &logger)
    : can_id(can_id), sub_function(sub_function), response_socket(socket), ECUResetLog(logger)
{
}

EcuReset::~EcuReset()
{
}

void EcuReset::ecuResetRequest()
{
    /* Hard Reset case */
    if (sub_function == 0x01) {
        LOG_INFO(ECUResetLog.GET_LOGGER(), "Reset Mode: Hard Reset");
        this->hardReset();
    /* Keys off Reset case */
    } else if (sub_function == 0x02) {
        LOG_INFO(ECUResetLog.GET_LOGGER(), "Reset Mode: Key Off Reset");
        this->keyOffReset();
    }
}
void EcuReset::hardReset()
{
    CreateInterface* interface = CreateInterface::getInstance(0x00, ECUResetLog);
    /* Deletes the interface */
    uint8_t interface_name = interface->getInterfaceName();
    LOG_INFO(ECUResetLog.GET_LOGGER(), "interface {} deleted", interface_name);
    interface->deleteInterface();

    /* Recreate the interface */
    LOG_INFO(ECUResetLog.GET_LOGGER(), "interface {} created", interface_name);
    interface->createInterface();
    interface->startInterface();

    /* Temporary code to set up again the canbus until the automated metod is fixed */
    system("sudo cangw -A -s vcan0 -d vcan1 -e");
    system("sudo cangw -A -s vcan1 -d vcan0 -e");
    /***                        END OF TEMPORARY CODE BLOCK                        ***/

    /* Send response */
    this->ecuResetResponse();
}

void EcuReset::keyOffReset()
{
    CreateInterface* interface = CreateInterface::getInstance(0x00, ECUResetLog);
    /* Turns down the interface */
    uint8_t interface_name = interface->getInterfaceName();

    LOG_INFO(ECUResetLog.GET_LOGGER(), "interface {} down", interface_name);
    interface->stopInterface();

    /* Turns up the interface */
    LOG_INFO(ECUResetLog.GET_LOGGER(), "interface {} up", interface_name);
    interface->startInterface();

    /* Sens response */
    this->ecuResetResponse();
}

void EcuReset::ecuResetResponse()
{
    /* Generate the response frame and send it */
    GenerateFrames generate_frames(response_socket, ECUResetLog);

    uint8_t frame_dest_id = (can_id >> 8) & 0xFF;
    LOG_INFO(ECUResetLog.GET_LOGGER(), "frame_dest_id = 0x{0:x}", frame_dest_id);

    uint8_t frame_sender_id = can_id & 0xFF;
    LOG_INFO(ECUResetLog.GET_LOGGER(), "frame_sender_id = 0x{0:x}", frame_sender_id);
    
    can_id = (frame_sender_id << 8) | frame_dest_id;
    LOG_INFO(ECUResetLog.GET_LOGGER(), "can_id = 0x{0:x}", can_id);

    generate_frames.ecuReset(can_id, sub_function, generate_frames.getSocket(), true);
    LOG_INFO(ECUResetLog.GET_LOGGER(), "Response sent");
}
