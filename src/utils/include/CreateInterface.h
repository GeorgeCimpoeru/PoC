/**
 *        This class is used to manage two virtual CAN network interfaces (vcan) :
 *        one for ECU and one for API
 *        in a Linux environment due to the fact there is no access to actual CAN hardware.
 *        The class has a constructor that initializes the class with the interface indicator number 
 *        The method create_interface is used to create the vcan interfaces.
 *        The method start_interface is used to bring the vcan interfaces up.
 *        The method stop_interface is used to bring the vcan interfaces down.
 *        The method delete_interface is used to delete the vcan interfaces.
 *        The method get_socket is used to return the sockets file descriptor.
 */


#ifndef INTERFACE_MODULE_H
#define INTERFACE_MODULE_H

#include <iostream>
#include <string>
#include <cstring>

#include <sys/ioctl.h>
#include <sys/socket.h>
#include <net/if.h>

#include<linux/can.h>
#include<string.h>
#include <fcntl.h>
#include "Logger.h"
/* class designed to manage the virtual CAN network interface (vcan) */
class CreateInterface
{    
    protected:
        uint8_t interface_name;        
        struct sockaddr_can addr;
        struct ifreq ifr;
        int _socketECU;
        int _socketAPI;
        Logger& logger;

    public:
        /* Constructor that takes the interface indicator number as an argument */
        CreateInterface(uint8_t interface_name, Logger& logger);
        /**
        * @brief Set the socket to not block in the reading operation.
        * 
        * @return int 
        */
        int setSocketBlocking();
        /**
        * @brief Method to create vcan interfaces: one to communicate 
        * with ECU and one to communicate with API
        * @return Returns true if interfaces were created and false if an error was encountered.      
        */        
        bool create_interface();

        /**
        * @brief Method to start vcan interfaces: one to communicate 
        * with ECU and one to communicate with API.  
        * @return Returns true if interfaces were started and false if an error was encountered.      
        */        
        bool start_interface();

        /**
        * @brief Method to stop vcan interfaces: one to communicate 
        * with ECU and one to communicate with API. 
        * @return Returns true if interfaces were stopped and false if an error was encountered.      
        */    
        bool stop_interface();

        /**
        * @brief Method to delete vcan interfaces when no longer needed: 
        * one to communicate with ECU and one to communicate with API.
        * @return Returns true if interfaces were deleted and false if an error was encountered. 
        */    
        bool delete_interface();

        /**
        * @brief Method that returns ECU socket
        */  
        int get_socketECU();
        /**
        * @brief Method that returns API socket
        */  
        int get_socketAPI();
};

#endif