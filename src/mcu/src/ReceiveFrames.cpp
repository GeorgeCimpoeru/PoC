#include "../include/ReceiveFrames.h"

ReceiveFrames::ReceiveFrames(int socket_canbus, int socket_api)
    : socket_canbus(socket_canbus), socket_api(socket_api), generate_frames(socket_canbus),
      timeout_duration(120), running(true) 
{
    startTimerThread();
}

ReceiveFrames::~ReceiveFrames() 
{
    stopListenAPI();
    stopListenCANBus();
    stopTimerThread();
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
            LOG_ERROR(MCULogger.GET_LOGGER(),"Read Error");
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
            LOG_ERROR(MCULogger.GET_LOGGER(),"Read Error");
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
    LOG_INFO(MCULogger.GET_LOGGER(),"Frame processing:...");
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
            if (frame.data[1] == 0xff) 
            {
                LOG_INFO(MCULogger.GET_LOGGER(),"Notification from the ECU that it is up");
                /* Set the corresponding value from the array with the ECU id */
                switch(sender_id)
                {
                    case 0x11:
                        ecus_up[0] = sender_id;
                        break;
                    case 0x12:
                        ecus_up[1] = sender_id;
                        break;
                    case 0x13:
                        ecus_up[2] = sender_id;
                        break;
                    case 0x14:
                        ecus_up[3] = sender_id;
                        break;
                    default:
                        break;
                }

                resetTimer(sender_id);
            } 
            else 
            {
                LOG_INFO(MCULogger.GET_LOGGER(),"Frame for MCU Service");
                handler.handleFrame(frame);
            }
        }
        else if (dest_id == 0xFA) {
            LOG_INFO(MCULogger.GET_LOGGER(),"Frame for API Service");
            std::vector<uint8_t> data(frame.data, frame.data + frame.can_dlc);
        } 
        else if (dest_id == 0xFF) 
        {
            /* Test frame between MCU and ECU */
            LOG_INFO(MCULogger.GET_LOGGER(),"Received the test frame ");
        }

        if (sender_id == 0xFA && dest_id != hex_value_id) 
        {
            if(frame.data[1] == 0x99)
            {
                /** sends back to api a response with all ECUs IDs that are up 
                    response structure: 
                    id: MCU_id + API_id 
                    data: {PCI_L, SID(0xD9), MCU_id, BATTERY_id, DOORS_id, ENGINE_id, ECU4_id}
                */
               LOG_INFO(MCULogger.GET_LOGGER(),"Response with all ECUs up.");
                generate_frames.sendFrame(0x10FA,{0x07, 0xD9, (uint8_t)hex_value_id, ecus_up[0], ecus_up[1], ecus_up[2], ecus_up[3]}, socket_api);
            }
            else
            {
                std::vector<uint8_t> data(frame.data, frame.data + frame.can_dlc);
                LOG_INFO(MCULogger.GET_LOGGER(),"Frame for ECU Service");
                generate_frames.sendFrame(frame.can_id, data);
            }
        }

        if (!listen_api && !listen_canbus) 
        {
            break;
        }
    }
}

void ReceiveFrames::resetTimer(uint8_t ecu_id) {
    std::lock_guard<std::mutex> lock(queue_mutex);
    ecu_timers[ecu_id] = std::chrono::steady_clock::now();
}

/**
 * Function to print the frames.
 */
void ReceiveFrames::printFrames(const struct can_frame &frame)
{
    std::ostringstream oss;
    LOG_INFO(MCULogger.GET_LOGGER(),"-------------------\n");
    LOG_INFO(MCULogger.GET_LOGGER(),"Processing CAN frame from queue:");
    LOG_INFO(MCULogger.GET_LOGGER(), fmt::format("CAN ID: 0x{:x}", frame.can_id));
    LOG_INFO(MCULogger.GET_LOGGER(), fmt::format("Data Length: {}", int(frame.can_dlc)));
    oss << "Data: ";
    for (uint8_t itr = 0; itr < frame.can_dlc; ++itr)
    {
        oss << fmt::format("{} ", int(frame.data[itr]));
    }
    LOG_INFO(MCULogger.GET_LOGGER(), oss.str());
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
        /* Call Generate_frames and SendFrame with the test frame */
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

const uint8_t* ReceiveFrames::getECUsUp() const {
    return ecus_up;
}

void ReceiveFrames::startTimerThread() {
    running = true;
    timer_thread = std::thread(&ReceiveFrames::timerCheck, this);
}

void ReceiveFrames::stopTimerThread() {
    running = false;
    if (timer_thread.joinable()) {
        timer_thread.join();
    }
}

void ReceiveFrames::timerCheck()
{
    try {
        while (running) {
            std::this_thread::sleep_for(std::chrono::seconds(1));
            auto now = std::chrono::steady_clock::now();
            std::lock_guard<std::mutex> lock(queue_mutex);
            for (auto it = ecu_timers.begin(); it != ecu_timers.end();) {
                if (std::chrono::duration_cast<std::chrono::seconds>(now - it->second) >= timeout_duration) {
                    uint8_t ecu_id = it->first;
                    /* Send request frame */
                    std::vector<uint8_t> data = {0x01};
                    generate_frames.sendFrame(0x11, data);
                    it = ecu_timers.erase(it);
                } else {
                    ++it;
                }
            }
        }
    } catch (const std::exception& e) {
        std::cerr << "Exception in timerCheck: " << e.what() << std::endl;
    } catch (...) {
        std::cerr << "Unknown exception in timerCheck" << std::endl;
    }
}