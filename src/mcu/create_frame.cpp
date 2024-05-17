#include "create_frame.h"

struct can_frame CreateCanFrame(FrameType frameType, uint32_t can_id, const uint8_t *data, uint8_t dlc) {
    struct can_frame frame;
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
    return frame;
}

int SendCanFrame(const std::string& interface, struct can_frame& frame) {
    int s = socket(PF_CAN, SOCK_RAW, CAN_RAW);
    if (s < 0) {
        perror("Socket");
        return -1;
    }

    struct ifreq ifr;
    std::strncpy(ifr.ifr_name, interface.c_str(), IFNAMSIZ - 1);
    ifr.ifr_name[IFNAMSIZ - 1] = '\0';
    if (ioctl(s, SIOCGIFINDEX, &ifr) < 0) {
        perror("Ioctl");
        close(s);
        return -1;
    }

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

    close(s);
    return 0;
}
