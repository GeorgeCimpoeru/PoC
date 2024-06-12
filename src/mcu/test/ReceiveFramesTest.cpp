#include <gtest/gtest.h>
#include <unistd.h>
#include <linux/can.h>
#include <sys/socket.h>
#include <thread>
#include <sstream>

#include "../include/ReceiveFrames.h"

/* MockReceiveFrames class derived from ReceiveFrames to expose protected members for testing */
class MockReceiveFrames : public ReceiveFrames
{
public:
    MockReceiveFrames(int socketAPI, int socketCANBus) : ReceiveFrames(socketAPI, socketCANBus) {}
    using ReceiveFrames::frameQueue;
    using ReceiveFrames::queueMutex;
    using ReceiveFrames::queueCondVar;
    using ReceiveFrames::printFrames;
};

/* Test fixture class for ReceiveFrames tests */
class ReceiveFramesTest : public ::testing::Test
{
protected:
    int mockSocketCANBus;
    int mockSocketAPI;
    int mockSocketPairCANBus[2];
    int mockSocketPairAPI[2];
    MockReceiveFrames* receiveFrames;

    /* Setup method to initialize test environment */
    virtual void SetUp()
    {
        /* Create mock socket pairs for testing */
        socketpair(AF_UNIX, SOCK_STREAM, 0, mockSocketPairCANBus);
        socketpair(AF_UNIX, SOCK_STREAM, 0, mockSocketPairAPI);
        mockSocketCANBus = mockSocketPairCANBus[0];
        mockSocketAPI = mockSocketPairAPI[0];
        receiveFrames = new MockReceiveFrames(mockSocketCANBus, mockSocketAPI);
    }

    /* Teardown method to clean up after each test */
    virtual void TearDown()
    {
        delete receiveFrames;
        close(mockSocketPairCANBus[0]);
        close(mockSocketPairCANBus[1]);
        close(mockSocketPairAPI[0]);
        close(mockSocketPairAPI[1]);
    }
};

/* Test to simulate read error from API */
TEST_F(ReceiveFramesTest, TestReceiveFramesFromAPI_ReadError)
{
    std::cerr << "Running TestReceiveFramesFromAPI_ReadError" << std::endl;
    /* Close socket to simulate read error */
    close(mockSocketAPI);
    receiveFrames->startListenAPI();
    int result = receiveFrames->receiveFramesFromAPI();
    receiveFrames->stopListenAPI(); 
    EXPECT_EQ(result, false);
    std::cerr << "Finished TestReceiveFramesFromAPI_ReadError" << std::endl;
}


/* Test to simulate read error from CAN Bus */
TEST_F(ReceiveFramesTest, TestReceiveFramesFromCANBus_ReadError)
{
    std::cerr << "Running TestReceiveFramesFromCANBus_ReadError" << std::endl;
    /* Close socket to simulate read error */
    close(mockSocketCANBus);
    receiveFrames->startListenCANBus();
    int result = receiveFrames->receiveFramesFromCANBus();
    receiveFrames->stopListenCANBus(); 
    EXPECT_EQ(result, false);
    std::cerr << "Finished TestReceiveFramesFromCANBus_ReadError" << std::endl;
}

/* Test to simulate successful read from CAN bus */
TEST_F(ReceiveFramesTest, TestReceiveFramesFromCANBus_Success)
{
    std::cerr << "Running TestReceiveFramesFromCANBus_Success" << std::endl;
    std::thread readerThread([this] {
        /* Delay to ensure reader thread is ready */
        usleep(100);
        struct can_frame frame;
        frame.can_id = 0x123;
        frame.can_dlc = 4;
        for (int i = 0; i < 4; ++i)
        {
            frame.data[i] = i;
        }
        /* Write frame to mock socket */
        write(mockSocketPairCANBus[1], &frame, sizeof(frame));
    });

    receiveFrames->startListenCANBus(); 
    std::thread receiverThread([this]{
        int result = receiveFrames->receiveFramesFromCANBus();
        /* Expect successful read */
        EXPECT_EQ(result, true);
    });

    receiveFrames->stopListenCANBus();
    receiverThread.join();
    readerThread.join();
    std::cerr << "Finished TestReceiveFramesFromCANBus_Success" << std::endl;
}

/* Test to simulate successful read from API */
TEST_F(ReceiveFramesTest, TestReceiveFramesFromAPI_Success)
{
    std::cerr << "Running TestReceiveFramesFromAPI_Success" << std::endl;
    std::thread readerThread([this] {
        /* Delay to ensure reader thread is ready */
        usleep(100);
        struct can_frame frame;
        frame.can_id = 0x123;
        frame.can_dlc = 4;
        for (int i = 0; i < 4; ++i)
        {
            frame.data[i] = i;
        }
        /* Write frame to mock socket */
        write(mockSocketPairAPI[1], &frame, sizeof(frame));
    });

    receiveFrames->startListenAPI(); 
    std::thread receiverThread([this]{
        int result = receiveFrames->receiveFramesFromAPI();
        /* Expect successful read */
        EXPECT_EQ(result, true);
    });

    receiveFrames->stopListenAPI();
    receiverThread.join();
    readerThread.join();
    std::cerr << "Finished TestReceiveFramesFromAPI_Success" << std::endl;
}

/* Test to process queue with an MCU-specific CAN frame */
TEST_F(ReceiveFramesTest, TestProcessQueue_ForMCU)
{
    std::cerr << "Running TestProcessQueue_ForMCU" << std::endl;
    struct can_frame frame;
    uint32_t mcu_id = 0x2210;
    frame.can_id = mcu_id;
    frame.can_dlc = 4;
    for (uint8_t itr = 0; itr < 4; ++itr)
    {
        frame.data[itr] = itr;
    }
    frame.data[1] = 0x22;

    receiveFrames->startListenCANBus(); 
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
    receiveFrames->stopListenCANBus();
    processorThread.join();

    std::string output = testing::internal::GetCapturedStdout();
    std::string expectedOutput = 
        "Frame processing:...\n"
        "-------------------\n"
        "Processing CAN frame from queue:\n"
        "CAN ID: 0x2210\n"
        "Data Length: 4\n"
        "Data: 0 22 2 3 \n"
        "Frame for MCU Service\n"
        "Single Frame received:\n"
        "\nReadDataByIdentifier called.\n";
    EXPECT_EQ(output, expectedOutput);
    std::cerr << "Finished TestProcessQueue_ForMCU" << std::endl;
    usleep(100);
}

/* Test to process queue with an ECU-up-specific CAN frame */
TEST_F(ReceiveFramesTest, TestProcessQueue_ForECUUp)
{
    std::cerr << "Running TestProcessQueue_ForECU-up" << std::endl;
    struct can_frame frame;
    uint32_t mcu_id = 0x2210;
    frame.can_id = mcu_id;
    frame.can_dlc = 4;
    for (uint8_t itr = 0; itr < 4; ++itr)
    {
        frame.data[itr] = itr;
    }
    frame.data[1] = 0xff;
    frame.data[2] = 0x11;

    receiveFrames->startListenCANBus(); 
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
    receiveFrames->stopListenCANBus();
    processorThread.join();

    std::string output = testing::internal::GetCapturedStdout();
    std::string expectedOutput = 
        "Frame processing:...\n"
        "-------------------\n"
        "Processing CAN frame from queue:\n"
        "CAN ID: 0x2210\n"
        "Data Length: 4\n"
        "Data: 0 ff 11 3 \n"
        "Notification from the ECU that it is up\n";
    EXPECT_EQ(output, expectedOutput);
    std::cerr << "Finished TestProcessQueue_ForECU-up" << std::endl;
    usleep(100);
}

/* Test to process queue with an API-specific CAN frame */
TEST_F(ReceiveFramesTest, TestProcessQueue_ForAPI)
{
    std::cerr << "Running TestProcessQueue_ForAPI" << std::endl;
    struct can_frame frame;
    uint32_t api_id = 0x11FA;
    frame.can_id = api_id;
    frame.can_dlc = 4;
    for (uint8_t itr = 0; itr < 4; ++itr)
    {
        frame.data[itr] = itr;
    }

    receiveFrames->startListenAPI(); 
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
    receiveFrames->stopListenAPI();
    processorThread.join();

    std::string output = testing::internal::GetCapturedStdout();
    std::string expectedOutput = 
        "Frame processing:...\n"
        "-------------------\n"
        "Processing CAN frame from queue:\n"
        "CAN ID: 0x11fa\n"
        "Data Length: 4\n"
        "Data: 0 1 2 3 \n"
        "Frame for API Service\n";
    EXPECT_EQ(output, expectedOutput);
    std::cerr << "Finished TestProcessQueue_ForAPI" << std::endl;
    usleep(100);
}

/* Test to process queue with an ECU-specific CAN frame */
TEST_F(ReceiveFramesTest, TestProcessQueue_ForECU)
{
    std::cerr << "Running TestProcessQueue_ForECU" << std::endl;
    struct can_frame frame;
    uint32_t ecu_id = 0xfa11;
    frame.can_id = ecu_id;
    frame.can_dlc = 4;
    for (uint8_t itr = 0; itr < 4; ++itr)
    {
        frame.data[itr] = itr;
    }

    receiveFrames->startListenCANBus(); 
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
    receiveFrames->stopListenCANBus();
    processorThread.join();

    std::string output = testing::internal::GetCapturedStdout();
    std::string expectedOutput = 
        "Frame processing:...\n"
        "-------------------\n"
        "Processing CAN frame from queue:\n"
        "CAN ID: 0xfa11\n"
        "Data Length: 4\n"
        "Data: 0 1 2 3 \n"
        "Frame for ECU Service\n";
    EXPECT_EQ(output, expectedOutput);
    std::cerr << "Finished TestProcessQueue_ForECU" << std::endl;
    usleep(100);
}

/* Test to process queue with a test-specific CAN frame */
TEST_F(ReceiveFramesTest, TestProcessQueue_TestFrame)
{
    std::cerr << "Running TestProcessQueue_TestFrame" << std::endl;
    struct can_frame frame;
    uint32_t test_id = 0x00FF;
    frame.can_id = test_id;
    frame.can_dlc = 4;
    for (uint8_t itr = 0; itr < 4; ++itr)
    {
        frame.data[itr] = itr;
    }

    receiveFrames->startListenCANBus(); 
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
    receiveFrames->stopListenCANBus();
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
TEST_F(ReceiveFramesTest, TestPrintFrames)
{
    std::cerr << "Running TestPrintFrames" << std::endl;
    struct can_frame frame;
    frame.can_id = 0x123;
    frame.can_dlc = 4;
    for (uint8_t itr = 0; itr < 4; ++itr)
    {
        frame.data[itr] = itr;
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
TEST_F(ReceiveFramesTest, TestSendTestFrame)
{
    std::cerr << "Running TestSendTestFrame" << std::endl;
    testing::internal::CaptureStdout();
    receiveFrames->startListenCANBus(); 
    receiveFrames->sendTestFrame();
    std::string output = testing::internal::GetCapturedStdout();
    
    EXPECT_TRUE(output.empty());
    std::cerr << "Finished TestSendTestFrame" << std::endl;
}

/* Test to verify stopListenAPI function */
TEST_F(ReceiveFramesTest, TestStopListenAPI)
{
    std::cerr << "Running TestStopListenAPI" << std::endl;
    receiveFrames->startListenAPI();
    receiveFrames->stopListenAPI();
    EXPECT_FALSE(receiveFrames->getListenAPI());
    std::cerr << "Finished TestStopListenAPI" << std::endl;
}

/* Test to verify stopListenCANBus function */
TEST_F(ReceiveFramesTest, TestStopListenCANBus)
{
    std::cerr << "Running TestStopListenCANBus" << std::endl;
    receiveFrames->startListenCANBus();
    receiveFrames->stopListenCANBus();
    EXPECT_FALSE(receiveFrames->getListenCANBus());
    std::cerr << "Finished TestStopListenCANBus" << std::endl;
}

/* Test to verify startListenAPI function */
TEST_F(ReceiveFramesTest, TestStartListenAPI)
{
    std::cerr << "Running TestStartListenAPI" << std::endl;
    receiveFrames->startListenAPI();
    EXPECT_TRUE(receiveFrames->getListenAPI());
    receiveFrames->stopListenAPI();
    std::cerr << "Finished TestStartListenAPI" << std::endl;
}

/* Test to verify startListenCANBus function */
TEST_F(ReceiveFramesTest, TestStartListenCANBus)
{
    std::cerr << "Running TestStartListenCANBus" << std::endl;
    receiveFrames->startListenCANBus();
    EXPECT_TRUE(receiveFrames->getListenCANBus());
    receiveFrames->stopListenCANBus();
    std::cerr << "Finished TestStartListenCANBus" << std::endl;
}

/* Test to verify getListenAPI function */
TEST_F(ReceiveFramesTest, TestGetListenAPI)
{
    std::cerr << "Running TestGetListenAPI" << std::endl;
    receiveFrames->startListenAPI();
    EXPECT_TRUE(receiveFrames->getListenAPI());
    receiveFrames->stopListenAPI();
    EXPECT_FALSE(receiveFrames->getListenAPI());
    std::cerr << "Finished TestGetListenAPI" << std::endl;
}

/* Test to verify getListenCANBus function */
TEST_F(ReceiveFramesTest, TestGetListenCANBus)
{
    std::cerr << "Running TestGetListenCANBus" << std::endl;
    receiveFrames->startListenCANBus();
    EXPECT_TRUE(receiveFrames->getListenCANBus());
    receiveFrames->stopListenCANBus();
    EXPECT_FALSE(receiveFrames->getListenCANBus());
    std::cerr << "Finished TestGetListenCANBus" << std::endl;
}

/* Test to verify getECUsUp function */
TEST_F(ReceiveFramesTest, TestGetECUsUp)
{
    std::cerr << "Running TestGetECUsUp" << std::endl;
    const std::vector<uint8_t>& ecusUp = receiveFrames->getECUsUp();
    EXPECT_TRUE(ecusUp.empty()); // Assuming initial state is empty
    std::cerr << "Finished TestGetECUsUp" << std::endl;
}

/* Test to verify gethexValueId function */
TEST_F(ReceiveFramesTest, TestGetHexValueId)
{
    std::cerr << "Running TestGetHexValueId" << std::endl;

    /* Retrieve the hexValueId using the getter function */
    uint32_t actualHexValueId = receiveFrames->gethexValueId();
    /* Verify if the retrieved hexValueId matches the expected value */
    EXPECT_EQ(actualHexValueId, 0x10);

    std::cerr << "Finished TestGetHexValueId" << std::endl;
}


/* Main function to run all tests */
int main(int argc, char **argv)
{
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}