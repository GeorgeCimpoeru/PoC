#include "../include/GenerateFrame.h"

/* Constructor which create a CAN frame */
GenerateFrame::GenerateFrame(int socket) 
{
    if (socket < 0) 
    {
        throw std::invalid_argument("Invalid socket");
    }
}

/* Default constructor */
GenerateFrame::GenerateFrame() {}

/* Function to create a CAN frame */
can_frame GenerateFrame::CreateFrame(uint32_t can_id, std::vector <uint8_t> data, FrameType frameType) 
{
    frame.can_id = can_id;

    switch (frameType) 
    {
        case DATA_FRAME:
            frame.can_id &= CAN_EFF_MASK;
            frame.can_dlc = data.size();
            std::memcpy(frame.data, data.data(), data.size());
            break;
        case REMOTE_FRAME:
            frame.can_id &= CAN_EFF_MASK;
            frame.can_id |= CAN_RTR_FLAG;
            frame.can_dlc = data.size();
            std::memcpy(frame.data, data.data(), data.size());
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
int GenerateFrame::SendFrame(uint32_t can_id, std::vector <uint8_t> data, FrameType frameType) 
{
    int s;
    struct sockaddr_can addr;
    struct ifreq ifr;

    /* Create the CAN frame */
    frame = CreateFrame(can_id, data, frameType);

    /* Send the CAN frame */
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
