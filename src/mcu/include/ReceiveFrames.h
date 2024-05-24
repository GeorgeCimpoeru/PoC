/*
    The ReceiveFrameModule library facilitates the reception of Controller Area Network (CAN) 
    frames through an interface utilizing sockets. This library allows you to read CAN frames 
    from the CAN bus and process them based on specific criteria.

    The library provides methods for receiving and handling CAN frames:
    - ReceiveFrameFromCANBus: Continuously reads CAN frames from the specified socket and processes them.
      It checks if the received frame's ID matches a predefined value (hexValueId) and processes the frame data accordingly.

    How to use example:
        int socket = //... initialize your socket here
        ReceiveFrameModule rfm(socket);
        rfm.ReceiveFrameFromCANBus();

    Author: Dirva Nicolae, 2024
*/

#ifndef POC_SRC_MCU_RECEIVE_FRAME_MODULE_H
#define POC_SRC_MCU_RECEIVE_FRAME_MODULE_H

#include<string>
#include<stdlib.h>
#include<iostream>
#include<unistd.h>
#include<cstring>
#include<sstream>
#include<vector>
#include <queue>
#include <mutex>
#include <condition_variable>

#include<linux/can.h>
#include "HandleFrames.h"

class ReceiveFrames{
 public:
  ReceiveFrames(int socket);
  int ReceiveFramesFromCANBus();
  void PrintFrames(const struct can_frame &frame);
  void ProcessQueue();
 protected:
  int s;
  const uint32_t hexValueId = 0x10;
  std::queue<struct can_frame> frameQueue;
  std::mutex queueMutex;
  std::condition_variable queueCondVar;
  HandleFrames handler;
};

#endif