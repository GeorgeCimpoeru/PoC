#include "../include/EcuReset.h"
#include "../../../mcu/include/MCUModule.h"
#include "../../../ecu_simulation/BatteryModule/include/BatteryModule.h"

EcuReset::EcuReset(uint32_t can_id, uint8_t sub_function, int socket, Logger &logger)
    : can_id(can_id), sub_function(sub_function), response_socket(socket), ECUResetLog(logger)
{
}

EcuReset::~EcuReset()
{
}

void EcuReset::ecuResetRequest()
{
    uint8_t lowerbits = can_id & 0xFF;
    uint8_t upperbits = can_id >> 8 & 0xFF;
    can_id = ((lowerbits << 8) | upperbits);

    NegativeResponse nrc(response_socket, ECUResetLog);

    if (sub_function != 0x01 && sub_function != 0x02)
    {
        nrc.sendNRC(can_id,0x11,NegativeResponse::SFNS);
        if (lowerbits == 0x10)
        {
            MCU::mcu->stop_flags[0x11] = false;
        } else if (lowerbits == 0x11)
        {
            battery->stop_flags[0x11] = false;
        }
    }
    // else if (!SecurityAccess::getMcuState())
    // {
    //     nrc.sendNRC(can_id,0x11,NegativeResponse::SAD);
    // }
    /* Hard Reset case */
    else if (sub_function == 0x01) {
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
    /* Commented because we need to discuss about this in future if we need it */
    // uint8_t lowerbits = can_id & 0xFF;
    // /* Send response */
    // this->ecuResetResponse();
    // CreateInterface* interface = CreateInterface::getInstance(0x00, ECUResetLog);
    // /* Deletes the interface */
    // uint8_t interface_name = interface->getInterfaceName();
    // LOG_INFO(ECUResetLog.GET_LOGGER(), "interface {} deleted", interface_name);
    // interface->deleteInterface();

    // /* Close the sockets binded to the interface */
    // switch(lowerbits) {
    //     /* ECU Battery case */
    //     case 0x11:
    //         LOG_INFO(ECUResetLog.GET_LOGGER(), "ECU socket closed");
    //         close(battery->getBatterySocket());
    //         break;

    //     /* MCU case */
    //     case 0x10:
    //     {
    //         LOG_INFO(ECUResetLog.GET_LOGGER(), "MCU sockets closed");
    //         close(MCU::mcu->getMcuApiSocket());
    //         close(MCU::mcu->getMcuEcuSocket());
    //         break;
    //     }
    // }

    // /* Recreate the interface */
    // LOG_INFO(ECUResetLog.GET_LOGGER(), "interface {} created", interface_name);
    // interface->createInterface();
    // interface->startInterface();

    // /* Recreate the sockets */
    // switch(lowerbits) {
    //     /* ECU Battery case */
    //     case 0x11:
    //         LOG_INFO(ECUResetLog.GET_LOGGER(), "ECU socket recreated");
    //         battery->setBatterySocket(interface_name);
    //         break;

    //     /* MCU case */
    //     case 0x10:
    //     {
    //         LOG_INFO(ECUResetLog.GET_LOGGER(), "MCU sockets recreated");
    //         MCU::mcu->setMcuApiSocket(interface_name);
    //         MCU::mcu->setMcuEcuSocket(interface_name);
    //         break;
    //     }
    // }
    uint8_t lowerbits = can_id & 0xFF;
    /* Send response */
    this->ecuResetResponse();
    switch(lowerbits)
    {
        case 0x10:
            system("./../autoscripts/ecu_reset_hard.sh");
            break;
    }
}

void EcuReset::keyOffReset()
{
    uint8_t lowerbits = can_id & 0xFF;
    /* Sens response */
    this->ecuResetResponse();
    CreateInterface* interface = CreateInterface::getInstance(0x00, ECUResetLog);
    /* Turns down the interface */
    uint8_t interface_name = interface->getInterfaceName();

    LOG_INFO(ECUResetLog.GET_LOGGER(), "interface {} down", interface_name);
    interface->stopInterface();

    /* Close the sockets binded to the interface */
    switch(lowerbits) {
        /* ECU Battery case */
        case 0x11:
            LOG_INFO(ECUResetLog.GET_LOGGER(), "ECU socket closed");
            close(battery->getBatterySocket());
            break;

        /* MCU case */
        case 0x10:
        {
            LOG_INFO(ECUResetLog.GET_LOGGER(), "MCU sockets closed");
            close(MCU::mcu->getMcuApiSocket());
            close(MCU::mcu->getMcuEcuSocket());
            break;
        }
    }

    /* Turns up the interface */
    LOG_INFO(ECUResetLog.GET_LOGGER(), "interface {} up", interface_name);
    interface->startInterface();

    /* Recreate the sockets */
    switch(lowerbits) {
        /* ECU Battery case */
        case 0x11:
            LOG_INFO(ECUResetLog.GET_LOGGER(), "ECU socket recreated");
            battery->setBatterySocket(interface_name);
            break;

        /* MCU case */
        case 0x10:
        {
            LOG_INFO(ECUResetLog.GET_LOGGER(), "MCU sockets recreated");
            MCU::mcu->setMcuApiSocket(interface_name);
            MCU::mcu->setMcuEcuSocket(interface_name);
            break;
        }
    }
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

    switch(frame_dest_id)
    {
        case 0x10:
            /* Send response frame */
            generate_frames.ecuReset(can_id, sub_function, generate_frames.getSocket(), true);
            LOG_INFO(ECUResetLog.GET_LOGGER(), "Service with SID {:x} successfully sent the response frame.", 0x11);
            MCU::mcu->stop_flags[0x11] = false;
            break;
        case 0x11:
            /* Send response frame */
            generate_frames.ecuReset(can_id, sub_function, generate_frames.getSocket(), true);
            LOG_INFO(ECUResetLog.GET_LOGGER(), "Service with SID {:x} successfully sent the response frame.", 0x11);
            battery->stop_flags[0x11] = false;
            break;
        default:
            LOG_ERROR(ECUResetLog.GET_LOGGER(), "Module with id {:x} not supported.", frame_dest_id);
    }
}
