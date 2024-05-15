/*
    The library facilitates the transmission of Controller Area Network (CAN) 
    frames through an interface utilizing sockets.
    How to use example:
        GenerateFrames g1 = GenerateFrames(socket);
        int x[] = {0x11,0x34,0x56};
        g1.SendFrame(0x23,3,x);

    Author:Mujdei Ruben, 2024
*/
#ifndef POC_INCLUDE_GENERATEFRAMES_H_
#define POC_INCLUDE_GENERATEFRAMES_H_

#include <iostream>
#include <cstdlib>

#include <unistd.h>
#include <linux/can.h>

class GenerateFrames
{
    private:
        int socket = -1;
    public:
        GenerateFrames(int socket);
        bool SendFrame(int id, int data_length, int *data);
    public:
        void AddSocket(int socket);
        struct can_frame CreateFrame(int &id, int data_length, int *data);
};

#endif