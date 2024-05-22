#ifndef MCU_MODULE_H
#define MCU_MODULE_H

#include <iostream>
#include <string>
#include <cstring>

#include <sys/ioctl.h>
#include <sys/socket.h>
#include <net/if.h>

#include<linux/can.h>

class MCU_module
{
    public:
        int id;

    protected:
        std::string interface_name;
        struct can_frame frame;
        struct sockaddr_can addr;
        struct ifreq ifr;

    public:
        MCU_module(std::string interface_name, int id);
        int create_interface();
        int delete_interface();
};

#endif