#include "../include/GenerateFrame.h"

/* Constructor which create a CAN frame */
GenerateFrame::GenerateFrame(FrameType frameType, uint32_t can_id, const uint8_t *data, uint8_t dlc) 
{
    CreateFrame(frameType, can_id, data, dlc);
}

/* Default constructor */
GenerateFrame::GenerateFrame() {}

/* Function to create a CAN frame */
can_frame GenerateFrame::CreateFrame(FrameType frameType, uint32_t can_id, const uint8_t *data, uint8_t dlc) 
{
    frame.can_id = can_id;

    switch (frameType) 
    {
        case DATA_FRAME:
            frame.can_id &= CAN_EFF_MASK;
            frame.can_dlc = dlc;
            std::memcpy(frame.data, data, dlc);
            break;
        case REMOTE_FRAME:
            frame.can_id &= CAN_EFF_MASK;
            frame.can_id |= CAN_RTR_FLAG;
            frame.can_dlc = dlc;
            break;
        case ERROR_FRAME:
            frame.can_id = CAN_ERR_FLAG;
            frame.can_dlc = 0;
            break;
        case OVERLOAD_FRAME:
            frame.can_id = CAN_ERR_FLAG;
            frame.can_dlc = 0;
            break;
        default:
            throw std::invalid_argument("Invalid frame type");
    }
    return frame;
}

/* Function to send a CAN frame */
int GenerateFrame::SendFrame(const std::string& interface, int s) 
{
    if (write(s, &frame, sizeof(frame)) != sizeof(frame)) 
    {
        perror("Write");
        close(s);
        return -1;
    }

    /* Close the socket */
    close(s);
    return 0;
}

/* Function to get the CAN frame */
can_frame GenerateFrame::getFrame() 
{
    return frame;
}
