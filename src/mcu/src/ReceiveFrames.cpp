#include "../include/ReceiveFrames.h"

ReceiveFrames::ReceiveFrames(int socket) : s(socket){}

/**
 * Function to read frames from the CAN bus and add them to a queue.
 * This function runs in a loop and continually reads frames from the CAN bus.
 */
int ReceiveFrames::ReceiveFramesFromCANBus() {
    struct can_frame frame;
    while (true) {
        // Read frames from the CAN socket
        int nbytes = read(s, &frame, sizeof(frame));
        if (nbytes < 0) {
            std::cerr << "Read Error" << std::endl;
            return 1; // Return error if read fails
        } else if (nbytes == 0) {
                    std::cerr << "No CAN frame received" << std::endl;
                } else if (nbytes < sizeof(struct can_frame)) {
                            std::cerr << "Incomplete frame read\n";
                            return 1;
                        } else {   
                            {
                                // Lock the queue before adding the frame to ensure thread safety
                                std::lock_guard<std::mutex> lock(queueMutex);
                                frameQueue.push(frame);
                            }
                            // Notify one waiting thread that a new frame has been added to the queue
                            queueCondVar.notify_one();
                        }

    }
    return 0;
}

/**
 * Function to process frames from the queue.
 * This function runs in a loop and processes each frame from the queue.
 */
void ReceiveFrames::ProcessQueue() {
    can_frame frameParam;
    while (true) {
        // Wait until the queue is not empty, then lock the queue
        std::unique_lock<std::mutex> lock(queueMutex);
        queueCondVar.wait(lock, [this]{ return !frameQueue.empty(); });

        // Extract the first element from the queue
        struct can_frame frame = frameQueue.front();
        frameQueue.pop();
        // Unlock the queue to allow other threads to add frames
        lock.unlock();

        // Print the received CAN frame details
        PrintFrames(frame);

        // Compare the CAN ID with the expected hexValueId
        if (frame.can_id == hexValueId) {
            // Compare the first data byte with hexValueId
            if (frame.data[0] == hexValueId) {
                    std::cout << "Frame for MCU Service" << std::endl;
                    handler.HandleFrame(frame);

            } else {
                    frameParam.can_id = frame.data[0];
                    frameParam.can_dlc = frame.can_dlc - 1;
                    std::copy(frameParam.data, frame.data + 1, frame.data + frame.can_dlc);
                    std::cout << "Frame for ECU Service" << std::endl;
                    handler.HandleFrame(frameParam);
                
            }   
        }
        else {
            frame_return.can_id = 0x00;
            frame_return.can_dlc = 0;
            std::fill(frame_return.data, frame_return.data + frame.can_dlc, 0);
        }
    return frame_return;
    }
}

/**
 * Function to print the frames.
 */
void ReceiveFrames::PrintFrames(const struct can_frame &frame) {
        std::cout << "-------------------\n";
        std::cout << "Processing CAN frame from queue:" << std::endl;
        std::cout << "CAN ID: 0x" << std::hex << frame.can_id << std::endl;
        std::cout << "Data Length: " << std::dec << int(frame.can_dlc) << std::endl;
        std::cout << "Data: ";
        for (int i = 0; i < frame.can_dlc; ++i) {
            std::cout << std::hex << int(frame.data[i]) << " ";
        }
        std::cout << std::endl;
}

