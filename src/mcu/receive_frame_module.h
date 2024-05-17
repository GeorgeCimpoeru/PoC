#ifndef POC_SRC_MCU_RECEIVE_FRAME_MODULE_H
#define POC_SRC_MCU_RECEIVE_FRAME_MODULE_H

#include<string>
#include<stdlib.h>
#include<iostream>
#include<unistd.h>
#include<cstring>
#include<sstream>

#include<linux/can.h>

class ReceiveFrameModule{
 public:
  ReceiveFrameModule(int socket);
  int ReceiveFrameFromCANBus();
 private:
  int s;
  const uint32_t hexValueId = 0x100;

};

#endif