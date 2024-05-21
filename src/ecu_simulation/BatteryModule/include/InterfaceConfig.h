#ifndef INTERFACECONFIG_H
#define INTERFACECONFIG_H

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

    void createLinuxVCanInterface();
    void deleteLinuxVCanInterface();

    bool openInterface();
    void closeInterface();

public:
    /* Init vcan communication */

    /* Create vcan  */

    SocketCanInterface(const std::string& interfaceName);

    inline void callSystem(std::string& cmd) const;
    void connectLinuxVCanInterfaces(std::string& sourceInterface, std::string& destinationInterface);
    void init();

    std::string& getInterfaceName();
    void setInterfaceName(std::string& interfaceName);

    int getSocketFd() const;

    ~SocketCanInterface();
};

#endif /* INTERFACECONFIG_H */