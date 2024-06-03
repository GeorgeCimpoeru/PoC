#include "../include/ReceiveFrames.h"

ReceiveFrames::ReceiveFrames(int socket) : s(socket), generateFrames(socket){}

<<<<<<< HEAD
/*
=======
ReceiveFrames::~ReceiveFrames()
{
    stopRunning();
}

uint32_t ReceiveFrames::gethexValueId() {
    return hexValueId;
 }

/**
>>>>>>> Tests for ReceiveFrames class added
 * Function to read frames from the CAN bus and add them to a queue.
 * This function runs in a loop and continually reads frames from the CAN bus.
 */
int ReceiveFrames::receiveFramesFromCANBus()
{
    struct can_frame frame;
    while (running)
    {
        /* Read frames from the CAN socket */
        int nbytes = read(s, &frame, sizeof(frame));
        if (nbytes < 0)
        {
            std::cerr << "Read Error" << std::endl;
            /* Return error if read fails */
            return 1;
        }
        else
        {
            {
                /* Lock the queue before adding the frame to ensure thread safety */
                std::lock_guard<std::mutex> lock(queueMutex);
                frameQueue.push(frame);
            }
            /* Notify one waiting thread that a new frame has been added to the queue */
            queueCondVar.notify_one();
        }
    }
    return 0;
}

/*
 * Function to process frames from the queue.
 * This function runs in a loop and processes each frame from the queue.
 */
void ReceiveFrames::processQueue()
{
    std::cout << "Frame processing:..." << std::endl;
    can_frame frameParam;
    while (true) {
        /* Wait until the queue is not empty, then lock the queue */
        std::unique_lock<std::mutex> lock(queueMutex);
        queueCondVar.wait(lock, [this]{ return !frameQueue.empty(); });

        /* Extract the first element from the queue */
        struct can_frame frame = frameQueue.front();
        frameQueue.pop();
        /* Unlock the queue to allow other threads to add frames */
        lock.unlock();

        /* Print the received CAN frame details */
        printFrames(frame);

        /* Extracting the components from can_id */

        /* First byte: id_destinatar */
        uint8_t sender_id = (frame.can_id >> 12) & 0xFF;
        /* 1 bit: request for destinatar */
        bool is_for_dest = (frame.can_id >> 8) & 0x01;
        /* Last byte: id_sender or id_api */
        uint8_t dest_id = frame.can_id & 0xFF;

        /* Compare the CAN ID with the expected hexValueId */
        if (dest_id == hexValueId) {
            /* Check if the request is for the MCU */
            if (is_for_dest) {
                std::cout << "Frame for MCU Service" << std::endl;
                handler.handleFrame(frame);
            } else {
                /* Check if the request is for the API */
                if (sender_id == 0xFA) {
                    std::cout << "Frame for API Service" << std::endl;
                    /* call function to send the frame to API */
                } else {
                    /* For ECUs services */
                    int new_can_id = (sender_id << 8) | hexValueId;
                    std::vector<int> data(frame.data, frame.data + frame.can_dlc);
                    std::cout << "Frame for ECU Service" << std::endl;
<<<<<<< HEAD
                    generateFrames.GenerateFrame(frameParam);
                    generateFrames.SendFrame();
=======
                    generateFrame.SendFrame(new_can_id, data);
>>>>>>> Tests for ReceiveFrames class added
                }
            }
        }
        else if(dest_id == 0xFF){
            /* Test frame betweend MCU and ECU */
            std::cout << "Received the test frame " << std::endl;
<<<<<<< HEAD
            generateFrames.GenerateFrame(frame);
            generateFrames.SendFrame();
=======
        }
        if(!running)
        {
            break;
>>>>>>> Tests for ReceiveFrames class added
        }
    }
}

/**
 * Function to print the frames.
 */
void ReceiveFrames::printFrames(const struct can_frame &frame)
{
        std::cout << "-------------------\n";
        std::cout << "Processing CAN frame from queue:" << std::endl;
        std::cout << "CAN ID: 0x" << std::hex << frame.can_id << std::endl;
        std::cout << "Data Length: " << std::dec << int(frame.can_dlc) << std::endl;
        std::cout << "Data: ";
        for (int i = 0; i < frame.can_dlc; ++i)
        {
            std::cout << std::hex << int(frame.data[i]) << " ";
        }
        std::cout << std::endl;
}

/*
 * Function to send test frame 
*/
void ReceiveFrames::sendTestFrame()
{
<<<<<<< HEAD
    can_frame testFrame;
    /* Set the CAN ID to 0xFF */
    testFrame.can_id = 0xFF;
    /* Set the data length code to 0, indicating no data */
    testFrame.can_dlc = 0;

    /* Call GenerateFrame and SendFrame with the test frame */
    generateFrames.GenerateFrame(testFrame);
    generateFrames.SendFrame();
}
=======
    if(running)
    {
        /* Set the CAN ID to 0xFF */
        int can_id = 0xFF;
        /* Set the data to an empty vector*/
        std::vector<int> data;

        /* Call GenerateFrame and SendFrame with the test frame */
        generateFrame.SendFrame(can_id, data);
    }
}

void ReceiveFrames::stopRunning()
{
    running = false;
    queueCondVar.notify_all();
}

void ReceiveFrames::startRunning()
{
    running = true;
    queueCondVar.notify_all();
}

bool ReceiveFrames::getRunning()
{
    return running;
}
>>>>>>> Tests for ReceiveFrames class added
