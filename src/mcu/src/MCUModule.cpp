#include "../include/MCUModule.h"

#ifndef UNIT_TESTING_MODE
Logger MCULogger("MCULogger", "logs/MCULogs.log");
#else
Logger MCULogger;
#endif /* UNIT_TESTING_MODE */
namespace MCU
{
    MCUModule mcu(0x01);
    /* Constructor */
    MCUModule::MCUModule(uint8_t interfaces_number) : 
                    create_interface(CreateInterface::getInstance(interfaces_number, MCULogger)),
                    is_running(false),
                    receive_frames(nullptr) 
                    {
        receive_frames = new ReceiveFrames(create_interface->getSocketEcuRead(), create_interface->getSocketApiRead());

    }

    /* Default constructor */
    MCUModule::MCUModule() : create_interface(CreateInterface::getInstance(0x01, MCULogger)),
                                                is_running(false),
                                                receive_frames(nullptr) {}

    /* Destructor */
    MCUModule::~MCUModule() 
    {
        create_interface->stopInterface();
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
}