#ifndef MCU_MODULE_H
#define MCU_MODULE_H

#include <iostream>
#include <string>
#include <cstring>

#include <sys/ioctl.h>
#include <sys/socket.h>
#include <net/if.h>

#include<linux/can.h>
#include<string.h>

class INTERFACE_module
{    
    protected:
        std::string interface_name;        
        struct sockaddr_can addr;
        struct ifreq ifr;
        int _socket;

    public:
        INTERFACE_module(std::string interface_name);
        int create_interface();
        int start_interface();
        int stop_interface();
        int delete_interface();
        int get_socket();
        std::string get_interface_name();
};

#endif