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
    private:
    /* Singleton instance */
    static CreateInterface* create_interface_instance;

    /* Private constructor to prevent direct instantiation */
    CreateInterface(uint8_t interface_name, Logger& logger);

    /* Delete copy constructor and assignment operator */
    CreateInterface(const CreateInterface&) = delete;
    CreateInterface& operator=(const CreateInterface&) = delete;
    protected:
        uint8_t interface_name;        
        struct sockaddr_can addr;
        struct ifreq ifr;
        int socket_ecu_read = -1;
        int socket_api_read = -1;
        int socket_ecu_write = -1;
        int socket_api_write = -1;
        Logger& logger;

    public:
        /**
        * @brief Method that returns an instance to the object
        */
        static CreateInterface* getInstance(uint8_t interface_name, Logger& logger);

        /**
         * @brief Get the Interface Name (used for ECU Reset)
         * 
         * @return Returns the interface name 
         */
        uint8_t getInterfaceName();

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
        bool createInterface();

        /**
        * @brief Method to start vcan interfaces: one to communicate 
        * with ECU and one to communicate with API.  
        * @return Returns true if interfaces were started and false if an error was encountered.      
        */        
        bool startInterface();

        /**
        * @brief Method to stop vcan interfaces: one to communicate 
        * with ECU and one to communicate with API. 
        * @return Returns true if interfaces were stopped and false if an error was encountered.      
        */    
        bool stopInterface();

        /**
        * @brief Method to delete vcan interfaces when no longer needed: 
        * one to communicate with ECU and one to communicate with API.
        * @return Returns true if interfaces were deleted and false if an error was encountered. 
        */    
        bool deleteInterface();

        /**
        * @brief Method that returns ECU socket for read
        */  
        int getSocketEcuRead();
        /**
        * @brief Method that returns API socket for read
        */  
        int getSocketApiRead();
        /**
        * @brief Method that returns ECU socket for write
        */  
        int getSocketEcuWrite();
        /**
        * @brief Method that returns API socket for write
        */  
        int getSocketApiWrite();

        friend class EcuReset;
};

#endif