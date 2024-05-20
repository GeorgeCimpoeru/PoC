/*Author:Stoisor Miruna, 2024*/
#ifndef POC_INCLUDE_RECEIVEFRAME_H_
#define POC_INCLUDE_RECEIVEFRAME_H_

#include <iostream>
#include <vector>
#include <unistd.h>
#include <linux/can.h>
#include "../include/HandleFrames.h"



class ReceiveFrames {
private:
    int socket = -1;
    int moduleID = 0x101;
protected:
    HandleFrames handleFrame;
    

public:
    ReceiveFrames(int socket, int moduleID);
    bool Receive(HandleFrames &handleFrame);
    void PrintFrame(const struct can_frame &frame);
};

#endif
