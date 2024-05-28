#include "../include/ReceiveFrames.h"
#include "../include/HandleFrames.h"

ReceiveFrames::ReceiveFrames(int socket, int module_id) : socket(socket), module_id(module_id), running(true) 
{
    if (socket < 0) 
    {
        std::cerr << "Error: Pass a valid Socket\n";
        exit(EXIT_FAILURE);
    }

    const int MIN_VALID_ID = 0x00000000;
    const int MAX_VALID_ID = 0x7FFFFFFF;

    if (module_id < MIN_VALID_ID || module_id > MAX_VALID_ID) 
    {
        std::cerr << "Error: Pass a valid Module ID\n";
        exit(EXIT_FAILURE);
    }

    /* Print the module_id for debugging */ 
    std::cout << "Module ID: 0x" << std::hex << std::setw(8) << std::setfill('0') << this->module_id << std::endl;
}

ReceiveFrames::~ReceiveFrames() 
{
    Stop();
}

void ReceiveFrames::Receive(HandleFrames &handle_frame) 
{
    std::cout << "Starting Receive Method for Module ID: 0x" << std::hex << std::setw(8) << std::setfill('0') << this->module_id << std::endl;
    try 
    {
        producerThread = std::thread(&ReceiveFrames::producer, this);
        std::cout << "ProducerThread\n";
        this->consumer(handle_frame);
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
            
            /* Check if the received frame is for your module */ 
            if (frame.can_id != static_cast<canid_t>(this->module_id)) 
            {
                std::cerr << "Received frame is not for this module\n";
                continue;
            }
            
            /* Print the frame for debugging */ 
            printFrame(frame);

            std::unique_lock<std::mutex> lock(mtx);
            frame_buffer.push_back(std::make_tuple(frame, nbytes));
            cv.notify_one();
        }
    }
    catch(const std::exception &e) 
    {
        std::cerr << "Exception in Producer: " << e.what() << std::endl;
        Stop();
    }
}

void ReceiveFrames::consumer(HandleFrames &handle_frame) 
{
    while (running) 
    {
        std::unique_lock<std::mutex> lock(mtx);
        cv.wait(lock, [this]{ return !frame_buffer.empty() || !running; });
        if (!running && frame_buffer.empty()) 
        {
            break;
        }

        // Extract frame and nbytes from the tuple in frame_buffer
        auto frameTuple = frame_buffer.front();
        frame_buffer.pop_front();
        lock.unlock();

        struct can_frame frame = std::get<0>(frameTuple);
        int nbytes = std::get<1>(frameTuple);

        /* Process the received frame */ 
        if (!handle_frame.checkReceivedFrame(nbytes, frame)) {
            std::cerr << "Failed to process frame\n";
        }
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