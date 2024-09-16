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

    /** 
     * @brief Private constructor to prevent direct instantiation 
     * 
     * @param interface_name The name of the virtual CAN network interface.
     * @param logger A logger instance used to record information and errors during the execution.
    */
    CreateInterface(uint8_t interface_name, Logger& logger);

    /* Delete copy constructor and assignment operator */
    CreateInterface(const CreateInterface&) = delete;
    CreateInterface& operator=(const CreateInterface&) = delete;
    protected:
        uint8_t interface_name;        
        struct sockaddr_can addr;
        struct ifreq ifr;
        Logger& logger;

    public:
        /**
        * @brief Method that returns an instance to the object
        * 
        * @param interface_name The name of the virtual CAN network interface.
        * @param logger A logger instance used to record information and errors during the execution.
        */
        static CreateInterface* getInstance(uint8_t interface_name, Logger& logger);

        /**
         * @brief Get the Interface Name (used for ECU Reset)
         * 
         * @return Returns the interface name 
         */
        uint8_t getInterfaceName();
        /**
         * @brief Create the socket
         * 
         * @param interface_number The interface indicator number.
         * @return Returns the socket file descriptor.
         */
        int createSocket(uint8_t interface_number);
        /**
        * @brief Set the socket to not block in the reading operation.
        * 
        * @param socket socket file descriptor needed to be set as non blocking
        * @return int 
        */
        int setSocketBlocking(int socket);
        /**
        * @brief Method to create vcan interfaces: one to communicate 
        * with ECU and one to communicate with API
        * 
        * @return Returns true if interfaces were created and false if an error was encountered.      
        */        
        bool createInterface();

        /**
        * @brief Method to start vcan interfaces: one to communicate 
        * with ECU and one to communicate with API.
        * 
        * @return Returns true if interfaces were started and false if an error was encountered.      
        */        
        bool startInterface();

        /**
        * @brief Method to stop vcan interfaces: one to communicate 
        * with ECU and one to communicate with API.
        * 
        * @return Returns true if interfaces were stopped and false if an error was encountered.      
        */    
        bool stopInterface();

        /**
        * @brief Method to delete vcan interfaces when no longer needed: 
        * one to communicate with ECU and one to communicate with API.
        * 
        * @return Returns true if interfaces were deleted and false if an error was encountered. 
        */    
        bool deleteInterface();
        friend class EcuReset;
};

#endif