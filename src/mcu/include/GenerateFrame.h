/**
 * GenerateFrame.h
 * For creating a CAN frame
 * This class is used to create a CAN frame and send it to the CAN bus.
 * The class has a constructor that takes the frame type, CAN ID, data, and
 * data length as arguments.
 * The private method GenerateFrame creates the CAN frame with the given
 * arguments.
 * The SendFrame method sends the created frame to the CAN bus.
 * It uses the CAN socket interface to send the frame to the CAN bus.
*/

#ifndef CREATE_FRAME_H
#define CREATE_FRAME_H

#include <linux/can.h>
#include <string>
#include <cstring>
#include <stdexcept>
#include <iostream>
#include <sys/ioctl.h>
#include <net/if.h>
#include <sys/socket.h>
#include <unistd.h>

/* Enumeration for frame types */
enum FrameType {
    DATA_FRAME,
    REMOTE_FRAME,
    ERROR_FRAME,
    OVERLOAD_FRAME
};

/* Class to create a CAN frame */
class GenerateFrame {
    public:
        GenerateFrame(
            FrameType frameType,
            uint32_t can_id,
            const uint8_t *data,
            uint8_t dlc
        );
        
        GenerateFrame();
        
        /* Function to send a CAN frame */
        int SendFrame(const std::string& interface, int s);

        /* Function to get the CAN frame */
        can_frame getFrame();

        struct can_frame frame;

    private:

        /* Function to create a CAN frame */
        can_frame CreateFrame(
            FrameType frameType,
            uint32_t can_id,
            const uint8_t *data,
            uint8_t dlc
        );
};

#endif
