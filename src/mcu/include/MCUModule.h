#ifndef POC_MCU_MODULE_H
#define POC_MCU_MODULE_H

#include "GenerateFrame.h"
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