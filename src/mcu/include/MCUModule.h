/**
 * This class represents the MCU module that interacts with the CAN bus.
 * It provides methods to start and stop the module, as well as to receive CAN frames.
 * These methods are from the InterfaceModule and ReceiveFrames classes.
*/

#ifndef POC_MCU_MODULE_H
#define POC_MCU_MODULE_H

#include "HandleFrames.h"
#include "InterfaceModule.h"
#include "ReceiveFrames.h"

#include <thread>

class MCUModule {
public:
    /** 
     * Constructor that takes the interface number as an argument.
     * When the constructor is called, it creates a new interface with the
     * given number and starts the interface.
    */
    MCUModule(uint8_t interfaces_number);
    
    /* Default constructor */
    MCUModule();

    /* Destructor */
    ~MCUModule();

    /**
     * Method to start the module.
     * This method sets the isRunning flag to true.
    */
    void StartModule();

    /**
     * Method to stop the module.
     * This method sets the isRunning flag to false.
    */
    void StopModule();

    /**
     * Method to receive frames.
     * This method starts a thread to process the queue and receives frames
     * from the CAN bus.
    */
    void recvFrames();

private:
    bool is_running;
    INTERFACE_module interface_module;
    ReceiveFrames* receive_frames;
};

#endif 