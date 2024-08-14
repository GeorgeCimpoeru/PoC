#include "../include/ReceiveFrames.h"
#include "../../../ecu_simulation/BatteryModule/include/BatteryModule.h"

ReceiveFrames::ReceiveFrames(int socket, int frame_id) : socket(socket), frame_id(frame_id), running(true) 
{
    if (socket < 0) 
    {
        /* std::cerr << "Error: Pass a valid Socket\n"; */
        LOG_WARN(batteryModuleLogger->GET_LOGGER(), "Error: Pass a valid Socket\n");
        exit(EXIT_FAILURE);
    }

    const int MIN_VALID_ID = 0x00000000;
    const int MAX_VALID_ID = 0x7FFFFFFF;

    if (frame_id < MIN_VALID_ID || frame_id > MAX_VALID_ID) 
    {
        /* std::cerr << "Error: Pass a valid Module ID\n"; */
        LOG_WARN(batteryModuleLogger->GET_LOGGER(), "Error: Pass a valid Module ID\n");
        exit(EXIT_FAILURE);
    }

    /* Print the frame_id for debugging */ 
    LOG_INFO(batteryModuleLogger->GET_LOGGER(), "Module ID: 0x{0:x}", this->frame_id);
}

ReceiveFrames::~ReceiveFrames() 
{
    stop();
}

void ReceiveFrames::receive(HandleFrames &handle_frame) 
{
    try 
    {
        bufferFrameInThread = std::thread(&ReceiveFrames::bufferFrameIn, this);
        this->bufferFrameOut(handle_frame);
    } 
    catch (const std::exception &e) 
    {
        LOG_ERROR(batteryModuleLogger->GET_LOGGER(), "Exception in starting threads: {}", e.what());
        stop();
    }
}

/* Set the socket to non-blocking mode. */
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
        /* Define a pollfd structure to monitor the socket */ 
        struct pollfd pfd;
        /* Set the socket file descriptor */ 
        pfd.fd = this->socket; 
        /* We are interested in read events -use POLLING */ 
        pfd.events = POLLIN;    

        while (running) 
        {
            /* Use poll to wait for data to be available with a timeout of 1000ms (1 second) */ 
            int poll_result = poll(&pfd, 1, 1000);

            if (poll_result > 0) 
            {
                if (pfd.revents & POLLIN) 
                {
                    struct can_frame frame = {};
                    int nbytes = read(this->socket, &frame, sizeof(struct can_frame));

                    if (nbytes > 0) 
                    {
                        std::unique_lock<std::mutex> lock(mtx);
                        frame_buffer.push_back(std::make_tuple(frame, nbytes));
                        cv.notify_one();
                    }
                }
            } 
            else if (poll_result == 0) 
            {
                /* Timeout occurred, no data available, but we just continue to check */ 
                continue;
            } 
            else 
            {
                /* An error occurred */ 
                LOG_ERROR(batteryModuleLogger->GET_LOGGER(), "poll error: {}", strerror(errno));
                break;
            }
        }
    }
    catch (const std::exception &e) 
    {
        LOG_ERROR(batteryModuleLogger->GET_LOGGER(), "Exception in bufferFrameIn: {}", e.what());
        stop();
    }
}

void ReceiveFrames::bufferFrameOut(HandleFrames &handle_frame) 
{
    while (running) 
    {
        label1: 
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
        current_module_id = frame_id & 0xFF;

        /* Starting frame processing timing if is it a frame request for MCU */
        auto it = std::find(service_sids.begin(), service_sids.end(), frame.data[1]);

        if (it != service_sids.end() && frame_dest_id == 0x11) {
            startTimer(frame.data[1]);
        }

        /* Check if the received frame is for your module */ 
        if (static_cast<int>(frame_dest_id) != current_module_id)
        {
            LOG_WARN(batteryModuleLogger->GET_LOGGER(), "Received frame is not for this module\n");
            continue;
        }

        if (((frame.can_id >> 8) & 0xFF) == 0) 
        {
        LOG_WARN(batteryModuleLogger->GET_LOGGER(), "Invalid CAN ID: upper 8 bits are zero\n");
        return;
        }

        /* Check if the frame is a request of type 'Up-Notification' from MCU */
        if (frame_dest_id == 0x11 && frame.data[0] == 0x01)
        {
            LOG_INFO(batteryModuleLogger->GET_LOGGER(), "Request received from MCU");
            /* Create and instance of GenerateFrames with the CAN socket */
            GenerateFrames frame = GenerateFrames(this->socket, *batteryModuleLogger);

            /* Create a vector of uint8_t (bytes) containing the data to be sent */
            std::vector<uint8_t> data = {0x0, 0xff, 0x11, 0x3};

            /* Send the CAN frame with ID 0x2210 and the data vector*/
            frame.sendFrame(0x1110, data);
            LOG_INFO(batteryModuleLogger->GET_LOGGER(), "Response sent to MCU");

            std::this_thread::sleep_for(std::chrono::milliseconds(100));
            goto label1;
        }
        if (frame.data[1] == 0xAA)
        {
            current_module_id = frame_id & 0xFF;
            GenerateFrames test =GenerateFrames(this->socket, *batteryModuleLogger);
            std::vector<uint8_t> data = {0x00, 0xAA};
            test.sendFrame(0x1110,data);

        }
        /* Process the received frame */ 
        if (!handle_frame.checkReceivedFrame(nbytes, frame)) {
            LOG_WARN(batteryModuleLogger->GET_LOGGER(), "Failed to process frame\n");
        }
    }
}

void ReceiveFrames::startTimer(uint8_t sid) {
    LOG_INFO(batteryModuleLogger->GET_LOGGER(), "Started frame processing timing for frame with SID {:x}.", sid);
    auto start_time = std::chrono::steady_clock::now();
    battery->timing_parameters[sid] = start_time.time_since_epoch().count();

    // Initialize stop flag for this SID
    battery->stop_flags[sid] = true;

    battery->active_timers[sid] = std::async(std::launch::async, [sid, this, start_time]() {
        while (BatteryModule::stop_flags[sid]) {
            auto now = std::chrono::steady_clock::now();
            std::chrono::duration<double> elapsed = now - start_time;
            if (elapsed.count() > AccessTimingParameter::p2_max_time / 20) {
                stopTimer(sid);
            }
            std::this_thread::sleep_for(std::chrono::milliseconds(10));
        }
    });
}

void ReceiveFrames::stopTimer(uint8_t sid) {
    LOG_INFO(batteryModuleLogger->GET_LOGGER(), "stopTimer function called for frame with SID {:x}.", sid);
    auto end_time = std::chrono::steady_clock::now();
    auto start_time = std::chrono::steady_clock::time_point(std::chrono::duration_cast<std::chrono::steady_clock::duration>(std::chrono::nanoseconds((long long)battery->timing_parameters[sid])));
    std::chrono::duration<double> processing_time = end_time - start_time;

    battery->timing_parameters[sid] = processing_time.count();

    if (BatteryModule::active_timers.find(sid) != BatteryModule::active_timers.end()) {
        // Set stop flag to false for this SID
        battery->stop_flags[sid] = false;
        battery->stop_flags.erase(sid);
        battery->active_timers[sid].wait();
    }
}

void ReceiveFrames::printFrame(const struct can_frame &frame) 
{
    LOG_INFO(batteryModuleLogger->GET_LOGGER(), "");
    LOG_INFO(batteryModuleLogger->GET_LOGGER(), "Received CAN frame");
    LOG_INFO(batteryModuleLogger->GET_LOGGER(), fmt::format("CAN ID: 0x{:x}", frame.can_id));
    LOG_INFO(batteryModuleLogger->GET_LOGGER(), "Data Length: {}", int(frame.can_dlc));
    std::ostringstream dataStream;
    dataStream << "Data:";
    for (int frame_byte = 0; frame_byte < frame.can_dlc; ++frame_byte) 
    {
        dataStream << " 0x" << std::hex << int(frame.data[frame_byte]);
    }
    LOG_INFO(batteryModuleLogger->GET_LOGGER(), "{}", dataStream.str());
}