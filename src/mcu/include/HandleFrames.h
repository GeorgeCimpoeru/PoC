
/**
 * @brief 
 * @file HandleFrames.h
 * @author Tanasevici Mihnea
 * @date 2024-05-20
 * 
 */

#ifndef HANDLE_FRAMES_H
#define HANDLE_FRAMES_H

#include <linux/can.h>
#include <iostream>
#include <vector>

class HandleFrames {
public:
    HandleFrames() = default;
    void handleFrame(const struct can_frame &frame);
};

#endif // HANDLE_FRAMES_H