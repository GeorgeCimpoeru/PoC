/*The ReceiveFrames library is responsible for receiving CAN frames, invoking a background 
method from the handle module for processing. It validates each received frame for completeness, 
emptiness, and correct addressing to the respective module. Additionally, it provides debugging 
support by printing the received CAN frames.*/

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

class ReceiveFrames 
{
private:
    int socket = -1;                              /*Descriptor for the socket connection*/
    int moduleID = 0x101;                         /*Module ID for filtering incoming frames*/
    std::queue<struct can_frame> frameBuffer;     /*Buffer to store incoming can frames*/
    std::mutex mtx;                               /*Mutex for ensuring thread safety when accessing the frame buffe*/
    std::condition_variable cv;                   /*Condition variable for thread synchronization*/
    bool running;                                 /*Flag indicating whether the receive threads should continue running*/ 
    std::thread producerThread;                   /*Thread for producing (receiving) frames*/
    std::thread consumerThread;                   /*Thread for consuming (handling) frames*/

    void producer();
    void consumer(HandleFrames &handleFrame);
protected:
    HandleFrames handleFrame;
    

public:
    ReceiveFrames(int socket, int moduleID);
    ~ReceiveFrames();
    void printFrame(const struct can_frame &frame);
    void Receive(HandleFrames &handleFrame);
    void Stop();
};

#endif
