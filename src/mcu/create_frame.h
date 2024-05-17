#ifndef CREATE_FRAME_H
#define CREATE_FRAME_H

#include <linux/can.h>
#include <linux/can/raw.h>
#include <string>
#include <cstring>
#include <stdexcept>
#include <iostream>
#include <sys/ioctl.h>
#include <net/if.h>
#include <sys/socket.h>
#include <unistd.h>

// Enumeration for frame types
enum FrameType {
    DATA_FRAME,
    REMOTE_FRAME,
    ERROR_FRAME,
    OVERLOAD_FRAME
};

// Function to create a CAN frame
struct can_frame CreateCanFrame(FrameType frameType, uint32_t can_id, const uint8_t *data, uint8_t dlc);

// Function to send a CAN frame
int SendCanFrame(const std::string& interface, struct can_frame& frame);

#endif // CREATE_FRAME_H
