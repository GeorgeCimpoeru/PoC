#include <gtest/gtest.h>
#include <unistd.h>
#include <linux/can.h>
#include <sys/socket.h>
#include <thread>
#include <sstream>

#include "../include/ReceiveFrames.h"

/* MockReceiveFrames class derived from ReceiveFrames to expose protected members for testing */
class MockReceiveFrames : public ReceiveFrames {
public:
    MockReceiveFrames(int socket) : ReceiveFrames(socket) {}
    using ReceiveFrames::frameQueue;
    using ReceiveFrames::queueMutex;
    using ReceiveFrames::queueCondVar;
    using ReceiveFrames::printFrames;
};

/* Test fixture class for ReceiveFrames tests */
class ReceiveFramesTest : public ::testing::Test {
protected:
    int mockSocket;
    int mockSocketPair[2];
    MockReceiveFrames* receiveFrames;

    /* Setup method to initialize test environment */
    virtual void SetUp() {
        /* Create a mock socket pair for testing */
        socketpair(AF_UNIX, SOCK_STREAM, 0, mockSocketPair);
        mockSocket = mockSocketPair[0];
        receiveFrames = new MockReceiveFrames(mockSocket);
    }

    /* Teardown method to clean up after each test */
    virtual void TearDown() {
        delete receiveFrames;
        close(mockSocketPair[0]);
        close(mockSocketPair[1]);
    }
};

/* Test to simulate read error from CAN bus */
TEST_F(ReceiveFramesTest, TestReceiveFramesFromCANBus_ReadError) {
    std::cerr << "Running TestReceiveFramesFromCANBus_ReadError" << std::endl;
    /* Close socket to simulate read error */
    close(mockSocket);
    receiveFrames->startRunning(); 
    int result = receiveFrames->receiveFramesFromCANBus();
    receiveFrames->stopRunning(); 
    EXPECT_EQ(result, 1);
    std::cerr << "Finished TestReceiveFramesFromCANBus_ReadError" << std::endl;
}

/* Test to simulate successful read from CAN bus */
TEST_F(ReceiveFramesTest, TestReceiveFramesFromCANBus_Success) {
    std::cerr << "Running TestReceiveFramesFromCANBus_Success" << std::endl;
    std::thread readerThread([this] {
        usleep(100);  /* Delay to ensure reader thread is ready */
        struct can_frame frame;
        frame.can_id = 0x123;
        frame.can_dlc = 4;
        for (int i = 0; i < 4; ++i) {
            frame.data[i] = i;
        }
        write(mockSocketPair[1], &frame, sizeof(frame));  /* Write frame to mock socket */
    });

    receiveFrames->startRunning(); 
    std::thread receiverThread([this] {
        int result = receiveFrames->receiveFramesFromCANBus();
        EXPECT_EQ(result, 0);  /* Expect successful read */
    });

    receiveFrames->stopRunning();
    receiverThread.join();
    readerThread.join();
    std::cerr << "Finished TestReceiveFramesFromCANBus_Success" << std::endl;
}

/* Test to process queue with a single CAN frame */
TEST_F(ReceiveFramesTest, TestProcessQueue) {
    std::cerr << "Running TestProcessQueue" << std::endl;
    struct can_frame frame;
    frame.can_id = 0x123;
    frame.can_dlc = 4;
    for (int i = 0; i < 4; ++i) {
        frame.data[i] = i;
    }
    receiveFrames->startRunning(); 
    {
        std::lock_guard<std::mutex> lock(receiveFrames->queueMutex);
        receiveFrames->frameQueue.push(frame);  /* Push frame to queue */
    }
    receiveFrames->queueCondVar.notify_one();

    testing::internal::CaptureStdout();
    std::thread processorThread([this] {
        receiveFrames->processQueue();
    });

    receiveFrames->stopRunning();  /* Ensure the process loop exits */
    processorThread.join();

    std::string output = testing::internal::GetCapturedStdout();
    std::string expectedOutput = "Frame processing:...\n"
    "-------------------\n"
    "Processing CAN frame from queue:\n"
    "CAN ID: 0x123\nData Length: 4\n"
    "Data: 0 1 2 3 \n";
    EXPECT_EQ(output, expectedOutput);
    std::cerr << "Finished TestProcessQueue" << std::endl;
    usleep(100);
}

/* Test to process queue with an MCU-specific CAN frame */
TEST_F(ReceiveFramesTest, TestProcessQueue_ForMCU) {
    std::cerr << "Running TestProcessQueue_ForMCU" << std::endl;
    struct can_frame frame;
    uint32_t mcu_id = 0x22110;
    frame.can_id = mcu_id;
    frame.can_dlc = 4;
    for (int i = 0; i < 4; ++i) {
        frame.data[i] = i;
    }

    receiveFrames->startRunning(); 
    {
        std::lock_guard<std::mutex> lock(receiveFrames->queueMutex);
        receiveFrames->frameQueue.push(frame);  /* Push frame to queue */
    }
    receiveFrames->queueCondVar.notify_one();
    usleep(100);

    testing::internal::CaptureStdout();
    std::thread processorThread([this] {
        receiveFrames->processQueue();
    });
    receiveFrames->stopRunning();
    processorThread.join();

    std::string output = testing::internal::GetCapturedStdout();
    std::string expectedOutput = 
        "Frame processing:...\n"
        "-------------------\n"
        "Processing CAN frame from queue:\n"
        "CAN ID: 0x22110\n"
        "Data Length: 4\n"
        "Data: 0 1 2 3 \n"
        "Frame for MCU Service\n";
    EXPECT_EQ(output, expectedOutput);
    std::cerr << "Finished TestProcessQueue_ForMCU" << std::endl;
    usleep(100);
}

/* Test to process queue with an API-specific CAN frame */
TEST_F(ReceiveFramesTest, TestProcessQueue_ForAPI) {
    std::cerr << "Running TestProcessQueue_ForAPI" << std::endl;
    struct can_frame frame;
    uint32_t api_id = 0xFA010;
    frame.can_id = api_id;
    frame.can_dlc = 4;
    for (int i = 0; i < 4; ++i) {
        frame.data[i] = i;
    }

    receiveFrames->startRunning(); 
    {
        std::lock_guard<std::mutex> lock(receiveFrames->queueMutex);
        receiveFrames->frameQueue.push(frame);  /* Push frame to queue */
    }
    receiveFrames->queueCondVar.notify_one();
    usleep(100);

    testing::internal::CaptureStdout();
    std::thread processorThread([this] {
        receiveFrames->processQueue();
    });
    receiveFrames->stopRunning();
    processorThread.join();

    std::string output = testing::internal::GetCapturedStdout();
    std::string expectedOutput = 
        "Frame processing:...\n"
        "-------------------\n"
        "Processing CAN frame from queue:\n"
        "CAN ID: 0xfa010\n"
        "Data Length: 4\n"
        "Data: 0 1 2 3 \n"
        "Frame for API Service\n";
    EXPECT_EQ(output, expectedOutput);
    std::cerr << "Finished TestProcessQueue_ForAPI" << std::endl;
    usleep(100);
}

/* Test to process queue with an ECU-specific CAN frame */
TEST_F(ReceiveFramesTest, TestProcessQueue_ForECU) {
    std::cerr << "Running TestProcessQueue_ForECU" << std::endl;
    struct can_frame frame;
    uint32_t ecu_id = 0x11010;
    frame.can_id = ecu_id;
    frame.can_dlc = 4;
    for (int i = 0; i < 4; ++i) {
        frame.data[i] = i;
    }

    receiveFrames->startRunning(); 
    {
        std::lock_guard<std::mutex> lock(receiveFrames->queueMutex);
        receiveFrames->frameQueue.push(frame);  /* Push frame to queue */
    }
    receiveFrames->queueCondVar.notify_one();
    usleep(100);

    testing::internal::CaptureStdout();
    std::thread processorThread([this] {
        receiveFrames->processQueue();
    });
    receiveFrames->stopRunning();
    processorThread.join();

    std::string output = testing::internal::GetCapturedStdout();
    std::string expectedOutput = 
        "Frame processing:...\n"
        "-------------------\n"
        "Processing CAN frame from queue:\n"
        "CAN ID: 0x11010\n"
        "Data Length: 4\n"
        "Data: 0 1 2 3 \n"
        "Frame for ECU Service\n";
    EXPECT_EQ(output, expectedOutput);
    std::cerr << "Finished TestProcessQueue_ForECU" << std::endl;
    usleep(100);
}

/* Test to process queue with a test-specific CAN frame */
TEST_F(ReceiveFramesTest, TestProcessQueue_TestFrame) {
    std::cerr << "Running TestProcessQueue_TestFrame" << std::endl;
    struct can_frame frame;
    uint32_t test_id = 0x000FF;
    frame.can_id = test_id;
    frame.can_dlc = 4;
    for (int i = 0; i < 4; ++i) {
        frame.data[i] = i;
    }

    receiveFrames->startRunning(); 
    {
        std::lock_guard<std::mutex> lock(receiveFrames->queueMutex);
        receiveFrames->frameQueue.push(frame);  /* Push frame to queue */
    }
    receiveFrames->queueCondVar.notify_one();
    usleep(100);

    testing::internal::CaptureStdout();
    std::thread processorThread([this] {
        receiveFrames->processQueue();
    });
    receiveFrames->stopRunning();
    processorThread.join();

    std::string output = testing::internal::GetCapturedStdout();
    std::string expectedOutput = "Frame processing:...\n"
        "-------------------\n"
        "Processing CAN frame from queue:\n"
        "CAN ID: 0xff\n"
        "Data Length: 4\n"
        "Data: 0 1 2 3 \n"
        "Received the test frame \n";
    EXPECT_EQ(output, expectedOutput);
    std::cerr << "Finished TestProcessQueue_TestFrame" << std::endl;
}

/* Test to verify the printFrames function */
TEST_F(ReceiveFramesTest, TestPrintFrames) {
    std::cerr << "Running TestPrintFrames" << std::endl;
    struct can_frame frame;
    frame.can_id = 0x123;
    frame.can_dlc = 4;
    for (int i = 0; i < 4; ++i) {
        frame.data[i] = i;
    }

    testing::internal::CaptureStdout();
    receiveFrames->printFrames(frame);
    std::string output = testing::internal::GetCapturedStdout();

    std::string expectedOutput = 
        "-------------------\n"
        "Processing CAN frame from queue:\n"
        "CAN ID: 0x123\n"
        "Data Length: 4\n"
        "Data: 0 1 2 3 \n";
    EXPECT_EQ(output, expectedOutput);
    std::cerr << "Finished TestPrintFrames" << std::endl;
}

/* Test to verify the sendTestFrame function */
TEST_F(ReceiveFramesTest, TestSendTestFrame) {
    std::cerr << "Running TestSendTestFrame" << std::endl;
    testing::internal::CaptureStdout();
    receiveFrames->startRunning(); 
    receiveFrames->sendTestFrame();
    std::string output = testing::internal::GetCapturedStdout();
    
    EXPECT_TRUE(output.empty());
    std::cerr << "Finished TestSendTestFrame" << std::endl;
}

/* Main function to run all tests */
int main(int argc, char **argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}