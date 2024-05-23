/*Author:Stoisor Miruna, 2024*/
#ifndef POC_INCLUDE_RECEIVEFRAME_H_
#define POC_INCLUDE_RECEIVEFRAME_H_

#include <iostream>
#include <vector>
#include <unistd.h>
#include <linux/can.h>
#include <queue>
#include <thread>
#include <mutex>
#include <condition_variable>
#include "../include/HandleFrames.h"

class ReceiveFrames {
private:
    int socket = -1;
    int moduleID = 0x101;
    std::queue<struct can_frame> frameBuffer;
    std::mutex mtx;
    std::condition_variable cv;
    bool running; // Flag to indicate when to stop the threads
    std::thread producerThread;
    std::thread consumerThread;

    void Producer();
    void Consumer(HandleFrames &handleFrame);
protected:
    HandleFrames handleFrame;
    

public:
    ReceiveFrames(int socket, int moduleID);
    ~ReceiveFrames();
    void PrintFrame(const struct can_frame &frame);
    void Receive(HandleFrames &handleFrame);
    void Stop();
};

#endif
