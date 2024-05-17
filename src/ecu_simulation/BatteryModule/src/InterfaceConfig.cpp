/**
 * @file InterfaceConfig.cpp
 * @author Iancu Daniel
 * @brief 
 * @version 0.1
 * @date 2024-05-15
 * 
 * @copyright Copyright (c) 2024
 * 
 */

#include "../include/InterfaceConfig.h"
#include <iostream> 
#include <sys/socket.h>
#include <sys/ioctl.h>
#include <linux/can/raw.h>
#include <string.h>

SocketCanInterface::SocketCanInterface(const std::string& interfaceName) : _interfaceName{interfaceName}
{

}

void SocketCanInterface::callSystem(std::string& cmd) const
{
    if(system(cmd.c_str()) != 0)
    {
        std::cout << cmd << " failed\n";
        exit(EXIT_FAILURE);
    }
}
bool SocketCanInterface::createLinuxVCanInterface()
{
    std::string cmd = "ip link add " + _interfaceName + " type vcan";
    callSystem(cmd);

    cmd = "ip link set " + _interfaceName + " up";
    callSystem(cmd);
}

void SocketCanInterface::connectLinuxVCanInterfaces(std::string& sourceInterface, std::string& destinationInterface)
{
    std::string cmd = "cangw -A -s " + sourceInterface + " -d " + destinationInterface + " -e";
    callSystem(cmd);
}

void SocketCanInterface::deleteLinuxVCanInterface()
{
    std::string cmd = "link set " + _interfaceName + " down";
    callSystem(cmd);

    cmd = "ip link delete " + _interfaceName + " type can";
    callSystem(cmd);
}

bool SocketCanInterface::open()
{
    //Create the socket
    _socketFd = socket(PF_CAN, SOCK_RAW, CAN_RAW);
    if(_socketFd < 0)
    {
        std::cout<<"Error trying to create the socket\n";
        return 1;
    }

    //Giving name and index to the interface created
    strcpy(_ifr.ifr_name, _interfaceName.c_str() );
    ioctl(_socketFd, SIOCGIFINDEX, &_ifr);

    //Set addr structure with info. of the CAN interface
    _addr.can_family = AF_CAN;
    _addr.can_ifindex = _ifr.ifr_ifindex;

    //Bind the socket to the CAN interface
    if(bind(_socketFd, (struct sockaddr * )&_addr, sizeof(_addr)) < 0)
    {
        std::cout<<"Error binding\n";
        return 1;
    }
    return 0;
}

void SocketCanInterface::close()
{

}

SocketCanInterface::~SocketCanInterface()
{

}