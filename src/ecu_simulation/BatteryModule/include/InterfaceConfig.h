#ifndef INTERFACECONFIG_H
#define INTERFACECONFIG_H

#define CAN_PROTOCOL CAN_RAW

#include <string>
#include <linux/can.h>
#include <net/if.h>

class SocketCanInterface
{
private:
    int _socketFd = -1;
    std::string _interfaceName;
    struct can_frame _canFrame;
    struct sockaddr_can _addr;
    struct ifreq _ifr;
public:
    /* Init vcan communication */

    /* Create vcan  */

    SocketCanInterface(const std::string& interfaceName);

    inline void callSystem(std::string& cmd) const;
    void createLinuxVCanInterface();
    void connectLinuxVCanInterfaces(std::string& sourceInterface, std::string& destinationInterface);
    void deleteLinuxVCanInterface();
    void init();

    bool openInterface();
    void closeInterface();

    std::string& getInterfaceName();
    void setInterfaceName(std::string& interfaceName);

    ~SocketCanInterface();

};

#endif /* INTERFACECONFIG_H */