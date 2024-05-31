/**
 *        This class is used to manage a virtual CAN network interface (vcan) 
 *        in a Linux environment due to the fact there is no access to actual CAN hardware.
 *        The class has a constructor that initializes the class with the interface name 
 *        (the name of vcan interface).
 *        The method create_interface is used to create a vcan interface.
 *        The method start_interface is used to bring the vcan interface up.
 *        The method stop_interface is used to bring the vcan interface down.
 *        The method delete_interface is used to delete the vcan interface.
 *        The method get_socket is used to return the socket file descriptor.
 *        The method get_interface_name is used to return the vcan interface name.
 */


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

/* class designed to manage the virtual CAN network interface (vcan) */
class INTERFACE_module
{    
    protected:
        std::string interface_name;        
        struct sockaddr_can addr;
        struct ifreq ifr;
        int _socket;

    public:
        /* Constructor that takes the interface name as an argument */
        INTERFACE_module(std::string interface_name);

        /* Method to create a vcan interface */
        int create_interface();

        /* Method to start a vcan interface */
        int start_interface();

        /* Method to stop a vcan interface */
        int stop_interface();

        /* Method to delete a vcan interface when no longer needed */
        int delete_interface();

        /* Method to get the socket descriptor */
        int get_socket();

        /* Method to get the vcan name interface */
        std::string get_interface_name();
};

#endif