#include "../include/MCUModule.h"

/* Constructor */
MCUModule::MCUModule(uint8_t interfaces_number) : 
                interface_module(interfaces_number), 
                is_running(false),
                receive_frames(nullptr) 
                {
    receive_frames = new ReceiveFrames(interface_module.get_socketECU(), interface_module.get_socketAPI());
}

/* Default constructor */
MCUModule::MCUModule() : interface_module(0x01), 
                                            is_running(false),
                                            receive_frames(nullptr) {}

/* Destructor */
MCUModule::~MCUModule() 
{
    interface_module.stop_interface();
    delete receive_frames;
}

/* Start the module */
void MCUModule::StartModule() { is_running = true; }

/* Stop the module */
void MCUModule::StopModule() { is_running = false; }

/* Receive frames */
void MCUModule::recvFrames() 
{
    while (is_running)
    {
        receive_frames->startListenAPI();
        receive_frames->startListenCANBus();
        /* Start a thread to process the queue */
        std::thread queue_thread_process(&ReceiveFrames::processQueue, receive_frames);

        /* Start a thread to listen on API socket */
        std::thread queue_thread_listen(&ReceiveFrames::receiveFramesFromAPI, receive_frames);

        /* Receive frames from the CAN bus */
        receive_frames->receiveFramesFromCANBus();

        receive_frames->stopListenAPI();
        receive_frames->stopListenCANBus();

        /* Wait for the threads to finish */
        queue_thread_process.join();
        queue_thread_listen.join();
    }
}