#include "../include/MCUModule.h"

/* Constructor */
MCUModule::MCUModule(int interfaceNumber) : 
                interfaceModule("vcan" + std::to_string(interfaceNumber)), 
                isRunning(false),
                receiveFrames(nullptr) 
                {
    interfaceModule.create_interface();
    interfaceModule.start_interface();
    receiveFrames = new ReceiveFrames(interfaceModule.get_socket(), interfaceModule.get_socket());
}

/* Default constructor */
MCUModule::MCUModule() : interfaceModule("vcan0"), 
                                            isRunning(false),
                                            receiveFrames(nullptr) {}

/* Destructor */
MCUModule::~MCUModule() 
{
    interfaceModule.stop_interface();
    delete receiveFrames;
}

/* Start the module */
void MCUModule::StartModule() { isRunning = true; }

/* Stop the module */
void MCUModule::StopModule() { isRunning = false; }

/* Receive frames */
void MCUModule::recvFrames() 
{
    while (isRunning) 
    {
        /* Start a thread to process the queue */
        std::thread queueThread(&ReceiveFrames::processQueue, receiveFrames);

        /* Receive frames from the CAN bus */
        receiveFrames->receiveFramesFromCANBus();

        /* Wait for the queue thread to finish */
        queueThread.join();
    }
}