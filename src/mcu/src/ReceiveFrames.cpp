#include "../include/ReceiveFrames.h"

ReceiveFrames::ReceiveFrames(int socketCANBus, int socketAPI) : socketCANBus(socketCANBus), socketAPI(socketAPI) , generateFrame(socketCANBus) {}

ReceiveFrames::~ReceiveFrames()
{
    stopListenAPI();
    stopListenCANBus();
}

uint32_t ReceiveFrames::gethexValueId()
{
    return hexValueId;
}

/**
 * Function to read frames from the CAN bus and add them to a queue.
 * This function runs in a loop and continually reads frames from the CAN bus.
 */
bool ReceiveFrames::receiveFramesFromCANBus()
{
    struct can_frame frame;
    while(listenCANBus)
    {
        /* Read frames from the CAN socket */
        int nbytes = read(socketCANBus, &frame, sizeof(frame));
        if (nbytes < 0)
        {
            std::cerr << "Read Error" << std::endl;
            /* Return error if read fails */
            return false;
        }
        else
        {
            {
                /* Lock the queue before adding the frame to ensure thread safety */
                std::lock_guard<std::mutex> lock(queueMutex);
                
                /* Take receiver_id */
                uint8_t dest_id = frame.can_id & 0xFF;

                /* If frame is for MCU module, for API or test frame */
                if( dest_id == hexValueId || dest_id == 0xFF || dest_id == 0xFA)
                {
                    frameQueue.push(frame);
                }
            }
            /* Notify one waiting thread that a new frame has been added to the queue */
            queueCondVar.notify_one();
        }
    }
    return true;
}

bool ReceiveFrames::receiveFramesFromAPI()
{
    struct can_frame frame;
    while(listenAPI)
    {
        /* Read frames from the CAN socket */
        int nbytes = read(socketAPI, &frame, sizeof(frame));
        if (nbytes < 0)
        {
            std::cerr << "Read Error" << std::endl;
            /* Return error if read fails */
            return false;
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
    return true;
}

/*
 * Function to process frames from the queue.
 * This function runs in a loop and processes each frame from the queue.
 */
void ReceiveFrames::processQueue()
{
    std::cout << "Frame processing:..." << std::endl;
    can_frame frameParam;
    while (true)
    {
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

        /* Last byte: id_sender */
        uint8_t sender_id = (frame.can_id >> 8) & 0xFF;
        /* First byte: id_receiver or id_api */
        uint8_t dest_id = frame.can_id & 0xFF;

        /* Compare the CAN ID with the expected hexValueId */
        if (dest_id == hexValueId)
        {
            /* Check if the request is for MCU service or notification that one ECU is up or down */
            if(frame.data[1] == 0xff)
            {
                std::cout << "Notification from the ECU that it is up" << std::endl;
                /* Add the ECU ID to the list of ECUs that are up */
                if (std::find(ecusUp.begin(), ecusUp.end(), sender_id) == ecusUp.end())
                {
                    ecusUp.push_back(sender_id);
                }
            }
            else
            {
                std::cout << "Frame for MCU Service" << std::endl;
                handler.handleFrame(frame);
            }
        }
        else if (dest_id == 0xFA) {
            std::cout << "Frame for API Service" << std::endl;
            std::vector<uint8_t> data(frame.data, frame.data + frame.can_dlc);
            // generateFrame.SendFrame(frame.can_id, data, socketAPI);
            /* call function to send the frame to API */
        }
        else if(dest_id == 0xFF)
        {
            /* Test frame between MCU and ECU */
            std::cout << "Received the test frame " << std::endl;
        }

        if(sender_id == 0xFA && dest_id != hexValueId) {
            /* For ECUs services */
            std::vector<uint8_t> data(frame.data, frame.data + frame.can_dlc);
            std::cout << "Frame for ECU Service" << std::endl;
            generateFrame.SendFrame(frame.can_id, data);
        }
        if(!listenAPI && !listenCANBus)
        {
            break;
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
        for (uint8_t itr = 0; itr < frame.can_dlc; ++itr)
        {
            std::cout << std::hex << int(frame.data[itr]) << " ";
        }
        std::cout << std::endl;
}

/*
 * Function to send test frame 
*/
void ReceiveFrames::sendTestFrame()
{
    if(listenCANBus)
    {
        /* Set the CAN ID to 0xFF */
        int can_id = 0xFF;
        /* Set the data to an empty vector*/
        std::vector<uint8_t> data;
        /* Call GenerateFrame and SendFrame with the test frame */
        generateFrame.SendFrame(can_id, data);
    }
}

void ReceiveFrames::stopListenAPI()
{
    listenAPI = false;
    queueCondVar.notify_all();
}

void ReceiveFrames::stopListenCANBus()
{
    listenCANBus = false;
    queueCondVar.notify_all();
}

void ReceiveFrames::startListenAPI()
{
    listenAPI = true;
    queueCondVar.notify_all();
}

void ReceiveFrames::startListenCANBus()
{
    listenCANBus = true;
    queueCondVar.notify_all();
}

bool ReceiveFrames::getListenAPI()
{
    return listenAPI;
}

bool ReceiveFrames::getListenCANBus()
{
    return listenCANBus;
}

const std::vector<uint8_t>& ReceiveFrames::getECUsUp() const
{
  return ecusUp;
}