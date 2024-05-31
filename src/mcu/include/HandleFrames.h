
/**
 * @brief This class is used to call the service needed by analyzing the SID
 *        if frame-type is a request or to send the response to API if 
 *        frame-type is a response.
 *        The method handleFrame takes a parameter of type can_frame,
 *        parses the SID and calls the appropiate function within a switch-case.
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
    /**
     * @brief Method used to call a service or to handle a response from ECU.
     * Takes a can_frame as parameter, parses the SID and calls the appropiate
     * function within a switch-case.
     * 
     * @param[in] frame 
    */
    void handleFrame(const struct can_frame &frame);
};

#endif // HANDLE_FRAMES_H