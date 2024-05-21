#include "../include/GenerateFrame.h"

// Constructor which create a CAN frame
CANFrame::CANFrame(FrameType frameType, uint32_t can_id, const uint8_t *data, uint8_t dlc) {
    GenerateFrame(frameType, can_id, data, dlc);
}

// Function to create a CAN frame
void CANFrame::GenerateFrame(FrameType frameType, uint32_t can_id, const uint8_t *data, uint8_t dlc) {
    frame.can_id = can_id;

    switch (frameType) {
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
}

// Function to send a CAN frame
int CANFrame::SendFrame(const std::string& interface) {
    // Create a socket
    int s = socket(PF_CAN, SOCK_RAW, CAN_RAW);
    if (s < 0) {
        perror("Socket");
        return -1;
    }

    // Get the interface index
    struct ifreq ifr;
    std::strncpy(ifr.ifr_name, interface.c_str(), IFNAMSIZ - 1);
    ifr.ifr_name[IFNAMSIZ - 1] = '\0';
    if (ioctl(s, SIOCGIFINDEX, &ifr) < 0) {
        perror("Ioctl");
        close(s);
        return -1;
    }

    // Bind the socket to the CAN interface
    struct sockaddr_can addr;
    addr.can_family = AF_CAN;
    addr.can_ifindex = ifr.ifr_ifindex;

    if (bind(s, (struct sockaddr *)&addr, sizeof(addr)) < 0) {
        perror("Bind");
        close(s);
        return -1;
    }


    if (write(s, &frame, sizeof(frame)) != sizeof(frame)) {
        perror("Write");
        close(s);
        return -1;
    }

    // Close the socket
    close(s);
    return 0;
}
