#include "../include/ReceiveFrames.h"
#include "../include/HandleFrames.h"

ReceiveFrames::ReceiveFrames(int socket, int moduleID) : socket(socket), moduleID(moduleID), running(true) 
{
    if (socket < 0) 
    {
        std::cerr << "Error: Pass a valid Socket\n";
        exit(EXIT_FAILURE);
    }

    const int MIN_VALID_ID = 0x00000000;
    const int MAX_VALID_ID = 0x7FFFFFFF;

    if (moduleID < MIN_VALID_ID || moduleID > MAX_VALID_ID) 
    {
        std::cerr << "Error: Pass a valid Module ID\n";
        exit(EXIT_FAILURE);
    }

    /* Print the moduleID for debugging */ 
    std::cout << "Module ID: 0x" << std::hex << std::setw(8) << std::setfill('0') << this->moduleID << std::endl;
}

ReceiveFrames::~ReceiveFrames() 
{
    Stop();
}

void ReceiveFrames::Receive(HandleFrames &handleFrame) 
{
    std::cout << "Starting Receive Method for Module ID: 0x" << std::hex << std::setw(8) << std::setfill('0') << this->moduleID << std::endl;
    try 
    {
        producerThread = std::thread(&ReceiveFrames::producer, this);
        std::cout << "ProducerThread\n";
        this->consumer(handleFrame);
        std::cout << "ConsumerThread\n";
    } 
    catch (const std::exception &e) 
    {
        std::cerr << "Exception in starting threads: " << e.what() << std::endl;
        Stop();
    }
}

void ReceiveFrames::Stop() 
{
    running = false;
    cv.notify_all();
    if (producerThread.joinable()) {
        producerThread.join();
    }
}

void ReceiveFrames::producer() 
{
    try{
        while (running) 
        {
            struct can_frame frame;
            int nbytes = read(this->socket, &frame, sizeof(struct can_frame));
            if (nbytes < 0) 
            {
                /* No data available or error occurred */ 
                if (errno == EWOULDBLOCK || errno == EAGAIN) 
                {
                    /* No data available, wait for a short duration */ 
                    std::this_thread::sleep_for(std::chrono::milliseconds(10000));
                    continue;
                } 
                else 
                {
                    /* Other error occurred, handle it */ 
                    std::cerr << "Read error: " << strerror(errno) << std::endl;
                    continue;
                }
            } 
            else if (nbytes < sizeof(struct can_frame)) 
            {
                std::cerr << "Incomplete frame read\n";
                continue;
            } 
            else if (nbytes == 0) 
            {
                /* Connection closed by peer */ 
                std::cerr << "Connection closed by peer" << std::endl;
                continue;
            }
            
            /* Check if the received frame is for your module */ 
            if (frame.can_id != static_cast<canid_t>(this->moduleID)) 
            {
                std::cerr << "Received frame is not for this module\n";
                continue;
            }

            /* Check if the received frame is empty */ 
            if (frame.can_dlc == 0) 
            {
                std::cerr << "Received empty frame\n";
                continue;
            }
            
            /* Print the frame for debugging */ 
            printFrame(frame);

            std::unique_lock<std::mutex> lock(mtx);
            frameBuffer.push(frame);
            cv.notify_one();
        }
    }
    catch(const std::exception &e) 
    {
        std::cerr << "Exception in Producer: " << e.what() << std::endl;
        Stop();
    }
}

void ReceiveFrames::consumer(HandleFrames &handleFrame) 
{
    while (running) 
    {
        std::unique_lock<std::mutex> lock(mtx);
        cv.wait(lock, [this]{ return !frameBuffer.empty() || !running; });
        if (!running && frameBuffer.empty()) 
        {
            break;
        }

        struct can_frame frame = frameBuffer.front();
        frameBuffer.pop();
        lock.unlock();

        /* Process the received frame */ 
        handleFrame.processReceivedFrame(frame);
    }
}

void ReceiveFrames::printFrame(const struct can_frame &frame) 
{
    std::cout << "\nReceived CAN frame:" << std::endl;
    std::cout << "CAN ID: 0x" << std::hex << int(frame.can_id) << std::endl;
    std::cout << "Data Length: " << int(frame.can_dlc) << std::endl;
    std::cout << "Data:";
    for (int i = 0; i < frame.can_dlc; ++i) 
    {
        std::cout << " 0x" << std::hex << int(frame.data[i]);
    }
}


