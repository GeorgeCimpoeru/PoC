#include "../include/ReceiveFrame.h"

ReceiveFrame::ReceiveFrame(HandleFrames& handler, int socket) : s(socket), handler(handler) {}

/**
 * Function to read frames from the CAN bus and add them to a queue.
 * This function runs in a loop and continually reads frames from the CAN bus.
 */
int ReceiveFrame::ReceiveFrameFromCANBus() {
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
void ReceiveFrame::ProcessQueue() {
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
        PrintFrame(frame);

        // Compare the CAN ID with the expected hexValueId
        if (frame.can_id == hexValueId) {
            // Compare the first data byte with hexValueId
            if (frame.data[0] == hexValueId) {
                if(frame.data[1] < 0x10){  
                    std::cout << "Frame with PCI" << std::endl;
                    // handler.HandleFrame(dataSize, frameData);
                    std::cout << "HandleFrames function call" << std::endl;
                } else {
                    std::cout << "Frame without PCI" << std::endl;
                    // handler.HandleFrame(dataSize, frameData);
                    std::cout << "HandleFrames function call" << std::endl;
                } 

            } else {
                if(frame.data[1] < 0x10){  
                    std::cout << "Frame with PCI" << std::endl;
                    uint8_t id = frame.data[0];
                    uint8_t dataSize = frame.can_dlc - 1;
                    std::vector<uint8_t> frameData(frame.data + 1, frame.data + frame.can_dlc);
                    std::cout << "generateFrame function call" << std::endl;
                    // generateFrame(id, dataSize, frameData);
                } else {
                    std::cout << "Frame without PCI" << std::endl;
                    uint8_t id = frame.data[0];
                    uint8_t dataSize = frame.can_dlc - 1;
                    std::vector<uint8_t> frameData(frame.data + 1, frame.data + frame.can_dlc);
                    std::cout << "generateFrame function call" << std::endl;
                    // generateFrame(id, dataSize, frameData);
                } 
            }   
        }
    }
}

/**
 * Function to print the frame.
 */
void ReceiveFrame::PrintFrame(const struct can_frame &frame) {
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
