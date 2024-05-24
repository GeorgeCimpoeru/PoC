/*
    This class represents the MCU module that interacts with the CAN bus.
    It provides methods to start and stop the module, as well as to receive CAN frames.
    These methods are from the InterfaceModule and ReceiveFrames classes.
*/

#ifndef POC_MCU_MODULE_H
#define POC_MCU_MODULE_H

#include "HandleFrames.h"
#include "InterfaceModule.h"
#include "ReceiveFrames.h"

class MCUModule {
public:
    MCUModule(int interfaceNumber);
    MCUModule();
    ~MCUModule();
    void StartModule();
    void StopModule();
    void recvFrames();

private:
    bool isRunning;
    int socket;
    INTERFACE_module interfaceModule;
    ReceiveFrames* receiveFrames;
};

#endif 