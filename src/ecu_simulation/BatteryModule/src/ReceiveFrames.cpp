#include "../include/ReceiveFrames.h"
#include "../include/HandleFrames.h"

ReceiveFrames::ReceiveFrames(int socket, int frame_id) : socket(socket), frame_id(frame_id), running(true) 
{
    if (socket < 0) 
    {
        std::cerr << "Error: Pass a valid Socket\n";
        exit(EXIT_FAILURE);
    }

    const int MIN_VALID_ID = 0x00000000;
    const int MAX_VALID_ID = 0x7FFFFFFF;

    if (frame_id < MIN_VALID_ID || frame_id > MAX_VALID_ID) 
    {
        std::cerr << "Error: Pass a valid Module ID\n";
        exit(EXIT_FAILURE);
    }

    /* Print the frame_id for debugging */ 
    std::cout << "Module ID: 0x" << std::hex << std::setw(8) << std::setfill('0') << this->frame_id << std::endl;
}

ReceiveFrames::~ReceiveFrames() 
{
    stop();
}

void ReceiveFrames::receive(HandleFrames &handle_frame) 
{
    std::cout << "Starting receive Method for Module ID: 0x" << std::hex << std::setw(8) << std::setfill('0') << this->frame_id << std::endl;
    try 
    {
        bufferFrameInThread = std::thread(&ReceiveFrames::bufferFrameIn, this);
        std::cout << "bufferFrameInThread\n";
        this->bufferFrameOut(handle_frame);
        std::cout << "bufferFrameOutThread\n";
    } 
    catch (const std::exception &e) 
    {
        std::cerr << "Exception in starting threads: " << e.what() << std::endl;
        stop();
    }
}

/* !!!!!!Set the socket to non-blocking mode. */
void ReceiveFrames::stop() 
{   
    {
        std::unique_lock<std::mutex> lock(mtx);
        running = false;
    }
    cv.notify_all();
    if (bufferFrameInThread.joinable())
    {
    bufferFrameInThread.join();
    }
}

void ReceiveFrames::bufferFrameIn() 
{
    try{
        while (running) 
        {
            struct can_frame frame = {};
            int nbytes = 0;
            while(running)
            {
                nbytes = read(this->socket, &frame, sizeof(struct can_frame));
                if(nbytes > 0)
                {
                    break;
                }
                sleep(1);
            }
            if (!running) {
                break;
            }
            
            std::unique_lock<std::mutex> lock(mtx);
            frame_buffer.push_back(std::make_tuple(frame, nbytes));
            cv.notify_one();
        }
    }
    catch(const std::exception &e) 
    {
        std::cerr << "Exception in bufferFrameIn: " << e.what() << std::endl;
        stop();
    }
}

void ReceiveFrames::bufferFrameOut(HandleFrames &handle_frame) 
{
    while (running) 
    {
        std::unique_lock<std::mutex> lock(mtx);
        cv.wait(lock, [this]{ return !frame_buffer.empty() || !running; });
        if (!running && frame_buffer.empty()) 
        {
            break;
        }

        /* Extract frame and nbytes from the tuple in frame_buffer */ 
        auto frameTuple = frame_buffer.front();
        frame_buffer.pop_front();
        lock.unlock();

        struct can_frame frame = std::get<0>(frameTuple);
        int nbytes = std::get<1>(frameTuple);

        /* Print the frame for debugging */ 
        printFrame(frame);

        uint8_t frame_dest_id = frame.can_id & 0xFF;
        this_module_id = frame_id & 0xFF;
         /* Check if the received frame is for your module */ 
        if (static_cast<int>(frame_dest_id) != this_module_id)
        {
            std::cerr << "Received frame is not for this module\n";
            continue;
        }

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
    for (int frame_byte = 0; frame_byte < frame.can_dlc; ++frame_byte) 
    {
        std::cout << " 0x" << std::hex << int(frame.data[frame_byte]);
    }
}