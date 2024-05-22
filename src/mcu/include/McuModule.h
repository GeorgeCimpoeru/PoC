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
    protected:
        std::string interface_name;        
        struct sockaddr_can addr;
        struct ifreq ifr;

    public:
        MCU_module(std::string interface_name);
        int create_interface();
        int delete_interface();
};

#endif