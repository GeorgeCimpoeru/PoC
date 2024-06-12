#include "../include/ReceiveFrames.h"

ReceiveFrames::ReceiveFrames(int socket_canbus, int socket_api) : socket_canbus(socket_canbus), socket_api(socket_api) , generate_frames(socket_canbus) {}

ReceiveFrames::~ReceiveFrames()
{
    stopListenAPI();
    stopListenCANBus();
}

uint32_t ReceiveFrames::gethexValueId()
{
    return hex_value_id;
}

/**
 * Function to read frames from the CAN bus and add them to a queue.
 * This function runs in a loop and continually reads frames from the CAN bus.
 */
bool ReceiveFrames::receiveFramesFromCANBus()
{
    struct can_frame frame;
    while(listen_canbus)
    {
        /* Read frames from the CAN socket */
        int nbytes = read(socket_canbus, &frame, sizeof(frame));
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
                std::lock_guard<std::mutex> lock(queue_mutex);
                
                /* Take receiver_id */
                uint8_t dest_id = frame.can_id & 0xFF;

                /* If frame is for MCU module, for API or test frame */
                if( dest_id == hex_value_id || dest_id == 0xFF || dest_id == 0xFA)
                {
                    frame_queue.push(frame);
                }
            }
            /* Notify one waiting thread that a new frame has been added to the queue */
            queue_cond_var.notify_one();
        }
    }
    return true;
}

bool ReceiveFrames::receiveFramesFromAPI()
{
    struct can_frame frame;
    while(listen_api)
    {
        /* Read frames from the CAN socket */
        int nbytes = read(socket_api, &frame, sizeof(frame));
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
                std::lock_guard<std::mutex> lock(queue_mutex);
                frame_queue.push(frame);  
            }
            /* Notify one waiting thread that a new frame has been added to the queue */
            queue_cond_var.notify_one();
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
    while (true)
    {
        /* Wait until the queue is not empty, then lock the queue */
        std::unique_lock<std::mutex> lock(queue_mutex);
        queue_cond_var.wait(lock, [this]{ return !frame_queue.empty(); });

        /* Extract the first element from the queue */
        struct can_frame frame = frame_queue.front();
        frame_queue.pop();
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
        if (dest_id == hex_value_id)
        {
            /* Check if the request is for MCU service or notification that one ECU is up or down */
            if(frame.data[1] == 0xff)
            {
                std::cout << "Notification from the ECU that it is up" << std::endl;
                /* Add the ECU ID to the list of ECUs that are up */
                if (std::find(ecus_up.begin(), ecus_up.end(), sender_id) == ecus_up.end())
                {
                    ecus_up.push_back(sender_id);
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
            /* generateFrames.sendFrame(frame.can_id, data, socketAPI); */
        }
        else if(dest_id == 0xFF)
        {
            /* Test frame between MCU and ECU */
            std::cout << "Received the test frame " << std::endl;
        }

        if(sender_id == 0xFA && dest_id != hex_value_id) {
            /* For ECUs services */
            std::vector<uint8_t> data(frame.data, frame.data + frame.can_dlc);
            std::cout << "Frame for ECU Service" << std::endl;
            generate_frames.sendFrame(frame.can_id, data);
        }
        if(!listen_canbus && !listen_api)
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
    if(listen_canbus)
    {
        /* Set the CAN ID to 0xFF */
        int can_id = 0xFF;
        /* Set the data to an empty vector*/
        std::vector<uint8_t> data;
        /* Call GenerateFrame and SendFrame with the test frame */
        generate_frames.sendFrame(can_id, data);
    }
}

void ReceiveFrames::stopListenAPI()
{
    listen_api = false;
    queue_cond_var.notify_all();
}

void ReceiveFrames::stopListenCANBus()
{
    listen_canbus = false;
    queue_cond_var.notify_all();
}

void ReceiveFrames::startListenAPI()
{
    listen_api = true;
    queue_cond_var.notify_all();
}

void ReceiveFrames::startListenCANBus()
{
    listen_canbus = true;
    queue_cond_var.notify_all();
}

bool ReceiveFrames::getListenAPI()
{
    return listen_api;
}

bool ReceiveFrames::getListenCANBus()
{
    return listen_canbus;
}

const std::vector<uint8_t>& ReceiveFrames::getECUsUp() const
{
  return ecus_up;
}


