#include "../include/MCUModule.h"

/* Constructor */
MCUModule::MCUModule(uint8_t interfacesNumber) : 
                interfaceModule(interfacesNumber), 
                isRunning(false),
                receiveFrames(nullptr) 
                {
    receiveFrames = new ReceiveFrames(interfaceModule.get_socketECU(), interfaceModule.get_socketAPI());
}

/* Default constructor */
MCUModule::MCUModule() : interfaceModule(0x01), 
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
        receiveFrames->startListenAPI();
        receiveFrames->startListenCANBus();
        /* Start a thread to process the queue */
        std::thread queueThreadProcess(&ReceiveFrames::processQueue, receiveFrames);

        /* Start a thread to listen on API socket */
        std::thread queueThreadListen(&ReceiveFrames::receiveFramesFromAPI, receiveFrames);

        /* Receive frames from the CAN bus */
        receiveFrames->receiveFramesFromCANBus();

        receiveFrames->stopListenAPI();
        receiveFrames->stopListenCANBus();

        /* Wait for the threads to finish */
        queueThreadProcess.join();
        queueThreadListen.join();
    }
}