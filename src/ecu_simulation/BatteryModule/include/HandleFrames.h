/*Author:Stoisor Miruna, 2024*/

#ifndef HANDLE_FRAME_H_
#define HANDLE_FRAME_H_

#include <vector>
#include <iostream>
#include <linux/can.h>
#include "../include/GenerateFrames.h"

class HandleFrames {
private:
    // Reference to Service instance
public:
    // Constructor to initialize GenerateFrame reference
    void ProcessReceivedFrame(const struct can_frame &frame);
};

#endif // HANDLE_FRAME_H_