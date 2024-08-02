#include "../include/ReceiveFrames.h"
namespace MCU
{
    ReceiveFrames::ReceiveFrames(int socket_canbus, int socket_api)
        : timeout_duration(120), running(true), socket_canbus(socket_canbus), 
        socket_api(socket_api), handler(socket_api, socket_canbus),
        generate_frames(socket_canbus, *MCULogger)
        
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
            LOG_INFO(MCULogger->GET_LOGGER(), "Captured a frame on the CANBus socket");
            if (nbytes < 0)
            {
                LOG_ERROR(MCULogger->GET_LOGGER(),"Read Error");
                /* Return error if read fails */
                return false;
            }
            else
            {
                {
                    /* Lock the queue before adding the frame to ensure thread safety */
                    std::lock_guard<std::mutex> lock(queue_mutex);
                    /* Take receiver_id */
                    uint8_t receiver_id = frame.can_id & 0xFF;

                    /* If frame is for MCU module, for API or test frame */
                    if( receiver_id == hex_value_id || receiver_id == 0xFF || receiver_id == 0xFA)
                    {
                        frame_queue.push(frame);
                        LOG_INFO(MCULogger->GET_LOGGER(), fmt::format("Pass a valid Module ID: 0x{:x} and frame added to the processing queue.", frame.can_id));

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
            LOG_INFO(MCULogger->GET_LOGGER(), "Captured a frame on the API socket");
            if (nbytes < 0)
            {
                LOG_ERROR(MCULogger->GET_LOGGER(),"Read Error");
                /* Return error if read fails */
                return false;
            }
            else
            {
                {
                    /* Lock the queue before adding the frame to ensure thread safety */
                    std::lock_guard<std::mutex> lock(queue_mutex);
                    frame_queue.push(frame);
                    LOG_INFO(MCULogger->GET_LOGGER(), fmt::format("Pass a valid Module ID: 0x{:x} and frame added to the processing queue.", frame.can_id));
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
        LOG_INFO(MCULogger->GET_LOGGER(),"Frame processing method invoked!");
        while (true)
        {
            /* Wait until the queue is not empty, then lock the queue */
            std::unique_lock<std::mutex> lock(queue_mutex);
            queue_cond_var.wait(lock, [this]{ return !frame_queue.empty(); });

            /* Extract the first element from the queue */
            struct can_frame frame = frame_queue.front();
            frame_queue.pop();
            LOG_INFO(MCULogger->GET_LOGGER(), fmt::format("Frame with ID: 0x{:x} is taken from processing queue", frame.can_id));
            /* Unlock the queue to allow other threads to add frames */
            lock.unlock();

            /* Print the received CAN frame details */
            printFrames(frame);

            /* Extracting the components from can_id */

            /* Last byte: id_sender */
            uint8_t sender_id = (frame.can_id >> 8) & 0xFF;
            /* First byte: id_receiver or id_api */
            uint8_t receiver_id = frame.can_id & 0xFF;
            /* Compare the CAN ID with the expected hexValueId */
            if (receiver_id == hex_value_id) 
            {
                if (frame.data[1] == 0xff) 
                {
                    LOG_INFO(MCULogger->GET_LOGGER(), fmt::format("Frame received to notify MCU that ECU with ID: 0x{:x} is up", sender_id));
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
                    LOG_INFO(MCULogger->GET_LOGGER(), fmt::format("Received frame for MCU to execute service with SID: 0x{:x}", frame.data[1]));
                    LOG_INFO(MCULogger->GET_LOGGER(), "Calling HandleFrames module to execute the service and parse the frame.");
                    handler.handleFrame(frame);
                }
            }
            else if (receiver_id == 0xFA) {
                LOG_INFO(MCULogger->GET_LOGGER(), fmt::format("Frame received from device with sender ID: 0x{:x} sent for API processing", sender_id));
                std::vector<uint8_t> data(frame.data, frame.data + frame.can_dlc);
                generate_frames.sendFrame(frame.can_id, data, socket_api, DATA_FRAME);
                LOG_INFO(MCULogger->GET_LOGGER(), fmt::format("Frame with ID: 0x{:x} sent on API socket", frame.can_id));
            } 

            if (sender_id == 0xFA && receiver_id != hex_value_id) 
            {
                if(frame.data[1] == 0x99)
                {
                    /** sends back to api a response with all ECUs IDs that are up 
                        response structure: 
                        id: MCU_id + API_id 
                        data: {PCI_L, SID(0xD9), MCU_id, BATTERY_id, DOORS_id, ENGINE_id, ECU4_id}
                    */
                    LOG_INFO(MCULogger->GET_LOGGER(), "Received frame to update status of ECUs still up.");
                    generate_frames.sendFrame(0x10FA,{0x07, 0xD9, (uint8_t)hex_value_id, ecus_up[0], ecus_up[1], ecus_up[2], ecus_up[3]}, socket_api, DATA_FRAME);
                    LOG_INFO(MCULogger->GET_LOGGER(), "Frame sent to API on API socket to update status of ECUs still up.");
                }
                else
                {
                    std::vector<uint8_t> data(frame.data, frame.data + frame.can_dlc);
                    LOG_INFO(MCULogger->GET_LOGGER(), fmt::format("Received frame for ECU to execute service with SID: 0x{:x}", frame.data[1]));
                    generate_frames.sendFrame(frame.can_id, data);
                    LOG_INFO(MCULogger->GET_LOGGER(), fmt::format("Frame with ID: 0x{:x} sent on CANBus socket", frame.can_id));
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
        LOG_INFO(MCULogger->GET_LOGGER(), "");
        LOG_INFO(MCULogger->GET_LOGGER(), "Received CAN frame");
        LOG_INFO(MCULogger->GET_LOGGER(), "Module ID: 0x{:x}", frame.can_id);
        LOG_INFO(MCULogger->GET_LOGGER(), "Data Length: {}", int(frame.can_dlc));
        std::ostringstream dataStream;
        dataStream << "Data:";
        for (int frame_byte = 0; frame_byte < frame.can_dlc; ++frame_byte) 
        {
            dataStream << " 0x" << std::hex << int(frame.data[frame_byte]);
        }
        LOG_INFO(MCULogger->GET_LOGGER(), "{}", dataStream.str());
        LOG_INFO(MCULogger->GET_LOGGER(), "");
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
        ReceiveFrames::listen_api = true;
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
                        /* Send request frame */
                        std::vector<uint8_t> data = {0x01};
                        ReceiveFrames::generate_frames.sendFrame(0x1011, data);
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
}