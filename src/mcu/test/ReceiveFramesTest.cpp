#include <gtest/gtest.h>
#include <unistd.h>
#include <linux/can.h>
#include <sys/socket.h>
#include <thread>
#include <sstream>
#include "../include/ReceiveFrames.h"
#include <vector>
#include <map>
#include <chrono>
#include <mutex>
#include <condition_variable>
#include <atomic>

/* MockReceiveFrames class derived from ReceiveFrames to expose protected members for testing */
class MockReceiveFrames : public MCU::ReceiveFrames
{
public:
    MockReceiveFrames(int socket_api, int socket_canbus) : ReceiveFrames(socket_api, socket_canbus) {}
    using ReceiveFrames::frame_queue;
    using ReceiveFrames::queue_mutex;
    using ReceiveFrames::queue_cond_var;
    using ReceiveFrames::printFrames;
    using ReceiveFrames::resetTimer;
    using ReceiveFrames::startTimerThread;
    using ReceiveFrames::stopTimerThread;
    using ReceiveFrames::ecu_timers;
    using ReceiveFrames::timeout_duration;
    using ReceiveFrames::running;
};
/* Test fixture class for ReceiveFrames tests */
class ReceiveFramesTest : public ::testing::Test
{
protected:
    int mock_socket_canbus;
    int mock_socket_api;
    int mock_socket_pair_canbus[2];
    int mock_socket_pair_api[2];
    MockReceiveFrames* receive_frames;
    /* Setup method to initialize test environment */
    virtual void SetUp()
    {
        /* Create mock socket pairs for testing */
        socketpair(AF_UNIX, SOCK_STREAM, 0, mock_socket_pair_canbus);
        socketpair(AF_UNIX, SOCK_STREAM, 0, mock_socket_pair_api);
        mock_socket_canbus = mock_socket_pair_canbus[0];
        mock_socket_api = mock_socket_pair_api[0];
        receive_frames = new MockReceiveFrames(mock_socket_canbus, mock_socket_api);
    }
    /* Teardown method to clean up after each test */
    virtual void TearDown()
    {
        delete receive_frames;
        close(mock_socket_pair_canbus[0]);
        close(mock_socket_pair_canbus[1]);
        close(mock_socket_pair_api[0]);
        close(mock_socket_pair_api[1]);
    }
};
/* Test to simulate read error from API */
TEST_F(ReceiveFramesTest, TestReceiveFramesFromAPI_ReadError)
{
    std::cerr << "Running TestReceiveFramesFromAPI_ReadError" << std::endl;
    /* Close socket to simulate read error */
    close(mock_socket_api);
    receive_frames->startListenAPI();
    int result = receive_frames->receiveFramesFromAPI();
    receive_frames->stopListenAPI(); 
    EXPECT_EQ(result, false);
    std::cerr << "Finished TestReceiveFramesFromAPI_ReadError" << std::endl;
}
/* Test to simulate read error from CAN Bus */
TEST_F(ReceiveFramesTest, TestReceiveFramesFromCANBus_ReadError)
{
    std::cerr << "Running TestReceiveFramesFromCANBus_ReadError" << std::endl;
    /* Close socket to simulate read error */
    close(mock_socket_canbus);
    receive_frames->startListenCANBus();
    int result = receive_frames->receiveFramesFromCANBus();
    receive_frames->stopListenCANBus(); 
    EXPECT_EQ(result, false);
    std::cerr << "Finished TestReceiveFramesFromCANBus_ReadError" << std::endl;
}
/* Test to simulate successful read from CAN bus */
TEST_F(ReceiveFramesTest, TestReceiveFramesFromCANBus_Success)
{
    std::cerr << "Running TestReceiveFramesFromCANBus_Success" << std::endl;
    std::thread reader_thread([this] {
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
        write(mock_socket_pair_canbus[1], &frame, sizeof(frame));
    });
    receive_frames->startListenCANBus(); 
    std::thread receiver_thread([this]{
        int result = receive_frames->receiveFramesFromCANBus();
        /* Expect successful read */
        EXPECT_EQ(result, true);
    });
    receive_frames->stopListenCANBus();
    receiver_thread.join();
    reader_thread.join();
    std::cerr << "Finished TestReceiveFramesFromCANBus_Success" << std::endl;
}
/* Test to simulate successful read from API */
TEST_F(ReceiveFramesTest, TestReceiveFramesFromAPI_Success)
{
    std::cerr << "Running TestReceiveFramesFromAPI_Success" << std::endl;
    std::thread reader_thread([this] {
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
        write(mock_socket_pair_api[1], &frame, sizeof(frame));
    });
    receive_frames->startListenAPI(); 
    std::thread receiver_thread([this]{
        int result = receive_frames->receiveFramesFromAPI();
        /* Expect successful read */
        EXPECT_EQ(result, true);
    });
    receive_frames->stopListenAPI();
    receiver_thread.join();
    reader_thread.join();
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
    receive_frames->startListenCANBus(); 
    {
        std::lock_guard<std::mutex> lock(receive_frames->queue_mutex);
        /* Push frame to queue */
        receive_frames->frame_queue.push(frame);
    }
    receive_frames->queue_cond_var.notify_one();
    usleep(100);
    testing::internal::CaptureStdout();
    std::thread processor_thread([this] {
        receive_frames->processQueue();
    });
    receive_frames->stopListenCANBus();
    processor_thread.join();

    std::string output = testing::internal::GetCapturedStdout();
    EXPECT_NE(output.find("Frame processing:...\n"), std::string::npos);
    EXPECT_NE(output.find("-------------------\n\n"), std::string::npos);
    EXPECT_NE(output.find("Processing CAN frame from queue:\n"), std::string::npos);
    EXPECT_NE(output.find("CAN ID: 0x2210\n"), std::string::npos);
    EXPECT_NE(output.find("Data Length: 4\n"), std::string::npos);
    EXPECT_NE(output.find("Data: 0 34 2 3 \n"), std::string::npos);
    EXPECT_NE(output.find("Frame for MCU Service\n"), std::string::npos);
    EXPECT_NE(output.find("Single Frame received:\n"), std::string::npos);
    std::cerr << "Finished TestProcessQueue_ForMCU" << std::endl;
    usleep(100);
}
  
/* Test to process queue with an ECU-up-specific CAN frame */
TEST_F(ReceiveFramesTest, TestProcessQueue_ForECUUp)
{
    std::cerr << "Running TestProcessQueue_ForECUUp" << std::endl;
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
    receive_frames->startListenCANBus(); 
    {
        std::lock_guard<std::mutex> lock(receive_frames->queue_mutex);
        /* Push frame to queue */
        receive_frames->frame_queue.push(frame);
    }
    receive_frames->queue_cond_var.notify_one();
    usleep(100);
    testing::internal::CaptureStdout();
    std::thread processor_thread([this] {
        receive_frames->processQueue();
    });
    receive_frames->stopListenCANBus();
    processor_thread.join();

    std::string output = testing::internal::GetCapturedStdout();
    EXPECT_NE(output.find("Frame processing:...\n"), std::string::npos);
    EXPECT_NE(output.find("-------------------\n\n"), std::string::npos);
    EXPECT_NE(output.find("Processing CAN frame from queue:\n"), std::string::npos);
    EXPECT_NE(output.find("CAN ID: 0x2210\n"), std::string::npos);
    EXPECT_NE(output.find("Data Length: 4\n"), std::string::npos);
    EXPECT_NE(output.find("Data: 0 255 17 3 \n"), std::string::npos);
    EXPECT_NE(output.find("Notification from the ECU that it is up\n"), std::string::npos);
    std::cerr << "Finished TestProcessQueue_ForECUUp" << std::endl;
    usleep(100);
}

/* Test to process queue with an ECU-specific CAN frame */
TEST_F(ReceiveFramesTest, TestProcessQueue_ForECU)
{
    std::cerr << "Running TestProcessQueue_ForECU" << std::endl;
    struct can_frame frame;
    frame.can_id = 0xfa11;
    frame.can_dlc = 4;
    for (int i = 0; i < 4; ++i)
    {
        frame.data[i] = i;
    }
    receive_frames->startListenAPI(); 
    {
        std::lock_guard<std::mutex> lock(receive_frames->queue_mutex);
        /* Push frame to queue */
        receive_frames->frame_queue.push(frame);
    }
    receive_frames->queue_cond_var.notify_one();
    usleep(100);
    testing::internal::CaptureStdout();
    std::thread processor_thread([this] {
        receive_frames->processQueue();
    });
    receive_frames->stopListenAPI();
    processor_thread.join();

    std::string output = testing::internal::GetCapturedStdout();
    EXPECT_NE(output.find("Frame processing:...\n"), std::string::npos);
    EXPECT_NE(output.find("-------------------\n\n"), std::string::npos);
    EXPECT_NE(output.find("Processing CAN frame from queue:\n"), std::string::npos);
    EXPECT_NE(output.find("CAN ID: 0xfa11\n"), std::string::npos);
    EXPECT_NE(output.find("Data Length: 4\n"), std::string::npos);
    EXPECT_NE(output.find("Data: 0 1 2 3 \n"), std::string::npos);
    EXPECT_NE(output.find("Frame for ECU Service\n"), std::string::npos);
    std::cerr << "Finished TestProcessQueue_ForECU" << std::endl;
    usleep(100);
}

/* Test to process queue with an response to api with all ECUs up */
TEST_F(ReceiveFramesTest, TestProcessQueue_ForResponseWithECUsUp)
{
    std::cerr << "Running TestProcessQueue_ForResponseWithECUsUp" << std::endl;
    struct can_frame frame;
    frame.can_id = 0xfa00;
    frame.can_dlc = 4;
    for (int i = 0; i < 4; ++i)
    {
        frame.data[i] = i;
    }
    frame.data[1] = 0x99;
    receive_frames->startListenAPI(); 
    {
        std::lock_guard<std::mutex> lock(receive_frames->queue_mutex);
        /* Push frame to queue */
        receive_frames->frame_queue.push(frame);
    }
    receive_frames->queue_cond_var.notify_one();
    usleep(100);
    testing::internal::CaptureStdout();
    std::thread processor_thread([this] {
        receive_frames->processQueue();
    });
    receive_frames->stopListenAPI();
    processor_thread.join();

    std::string output = testing::internal::GetCapturedStdout();
    EXPECT_NE(output.find("Frame processing:...\n"), std::string::npos);
    EXPECT_NE(output.find("-------------------\n\n"), std::string::npos);
    EXPECT_NE(output.find("Processing CAN frame from queue:\n"), std::string::npos);
    EXPECT_NE(output.find("CAN ID: 0xfa00\n"), std::string::npos);
    EXPECT_NE(output.find("Data Length: 4\n"), std::string::npos);
    EXPECT_NE(output.find("Data: 0 153 2 3 \n"), std::string::npos);
    EXPECT_NE(output.find("Response with all ECUs up.\n"), std::string::npos);
    std::cerr << "Finished TestProcessQueue_ForResponseWithECUsUp" << std::endl;
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
    receive_frames->startListenAPI(); 
    {
        std::lock_guard<std::mutex> lock(receive_frames->queue_mutex);
        /* Push frame to queue */
        receive_frames->frame_queue.push(frame);
    }
    receive_frames->queue_cond_var.notify_one();
    usleep(100);
    testing::internal::CaptureStdout();
    std::thread processor_thread([this] {
        receive_frames->processQueue();
    });
    receive_frames->stopListenAPI();
    processor_thread.join();

    std::string output = testing::internal::GetCapturedStdout();
    EXPECT_NE(output.find("Frame processing:...\n"), std::string::npos);
    EXPECT_NE(output.find("-------------------\n\n"), std::string::npos);
    EXPECT_NE(output.find("Processing CAN frame from queue:\n"), std::string::npos);
    EXPECT_NE(output.find("CAN ID: 0x11fa\n"), std::string::npos);
    EXPECT_NE(output.find("Data Length: 4\n"), std::string::npos);
    EXPECT_NE(output.find("Data: 0 1 2 3 \n"), std::string::npos);
    EXPECT_NE(output.find("Frame for API Service\n"), std::string::npos);
    std::cerr << "Finished TestProcessQueue_ForAPI" << std::endl;
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
    receive_frames->startListenCANBus(); 
    {
        std::lock_guard<std::mutex> lock(receive_frames->queue_mutex);
        /* Push frame to queue */
        receive_frames->frame_queue.push(frame);
    }
    receive_frames->queue_cond_var.notify_one();
    usleep(100);
    testing::internal::CaptureStdout();
    std::thread processor_thread([this] {
        receive_frames->processQueue();
    });
    receive_frames->stopListenCANBus();
    processor_thread.join();

    std::string output = testing::internal::GetCapturedStdout();
    EXPECT_NE(output.find("Frame processing:...\n"), std::string::npos);
    EXPECT_NE(output.find("-------------------\n\n"), std::string::npos);
    EXPECT_NE(output.find("Processing CAN frame from queue:\n"), std::string::npos);
    EXPECT_NE(output.find("CAN ID: 0xff\n"), std::string::npos);
    EXPECT_NE(output.find("Data Length: 4\n"), std::string::npos);
    EXPECT_NE(output.find("Data: 0 1 2 3 \n"), std::string::npos);
    EXPECT_NE(output.find("Received the test frame \n"), std::string::npos);
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
    receive_frames->printFrames(frame);
    std::string output = testing::internal::GetCapturedStdout();
    EXPECT_NE(output.find("-------------------\n\n"), std::string::npos);
    EXPECT_NE(output.find("Processing CAN frame from queue:\n"), std::string::npos);
    EXPECT_NE(output.find("CAN ID: 0x123\n"), std::string::npos);
    EXPECT_NE(output.find("Data Length: 4\n"), std::string::npos);
    EXPECT_NE(output.find("Data: 0 1 2 3 \n"), std::string::npos);
    std::cerr << "Finished TestPrintFrames" << std::endl;
}
  
/* Test to verify stop_listen_api function */
TEST_F(ReceiveFramesTest, TestStopListenAPI)
{
    std::cerr << "Running TestStopListenAPI" << std::endl;
    receive_frames->startListenAPI();
    receive_frames->stopListenAPI();
    EXPECT_FALSE(receive_frames->getListenAPI());
    std::cerr << "Finished TestStopListenAPI" << std::endl;
}
/* Test to verify stop_listen_can_bus function */
TEST_F(ReceiveFramesTest, TestStopListenCANBus)
{
    std::cerr << "Running TestStopListenCANBus" << std::endl;
    receive_frames->startListenCANBus();
    receive_frames->stopListenCANBus();
    EXPECT_FALSE(receive_frames->getListenCANBus());
    std::cerr << "Finished TestStopListenCANBus" << std::endl;
}
/* Test to verify start_listen_api function */
TEST_F(ReceiveFramesTest, TestStartListenAPI)
{
    std::cerr << "Running TestStartListenAPI" << std::endl;
    receive_frames->startListenAPI();
    EXPECT_TRUE(receive_frames->getListenAPI());
    receive_frames->stopListenAPI();
    std::cerr << "Finished TestStartListenAPI" << std::endl;
}
/* Test to verify start_listen_can_bus function */
TEST_F(ReceiveFramesTest, TestStartListenCANBus)
{
    std::cerr << "Running TestStartListenCANBus" << std::endl;
    receive_frames->startListenCANBus();
    EXPECT_TRUE(receive_frames->getListenCANBus());
    receive_frames->stopListenCANBus();
    std::cerr << "Finished TestStartListenCANBus" << std::endl;
}
/* Test to verify get_listen_api function */
TEST_F(ReceiveFramesTest, TestGetListenAPI)
{
    std::cerr << "Running TestGetListenAPI" << std::endl;
    receive_frames->startListenAPI();
    EXPECT_TRUE(receive_frames->getListenAPI());
    receive_frames->stopListenAPI();
    EXPECT_FALSE(receive_frames->getListenAPI());
    std::cerr << "Finished TestGetListenAPI" << std::endl;
}
/* Test to verify get_listen_can_bus function */
TEST_F(ReceiveFramesTest, TestGetListenCANBus)
{
    std::cerr << "Running TestGetListenCANBus" << std::endl;
    receive_frames->startListenCANBus();
    EXPECT_TRUE(receive_frames->getListenCANBus());
    receive_frames->stopListenCANBus();
    EXPECT_FALSE(receive_frames->getListenCANBus());
    std::cerr << "Finished TestGetListenCANBus" << std::endl;
}
/* Test to verify get_ecus_up function */
TEST_F(ReceiveFramesTest, TestGetECUsUp)
{
    std::cerr << "Running TestGetECUsUp" << std::endl;
    const uint8_t* ecus_up = receive_frames->getECUsUp();
    // Assuming initial state is all zeros
    for (int i = 0; i < 4; ++i) {
        EXPECT_EQ(ecus_up[i], 0);
    }
    std::cerr << "Finished TestGetECUsUp" << std::endl;
}

/* Test to verify get_hex_value_id function */
TEST_F(ReceiveFramesTest, TestGetHexValueId)
{
    std::cerr << "Running TestGetHexValueId" << std::endl;
    /* Retrieve the hex_value_id using the getter function */
    uint32_t actual_hex_value_id = receive_frames->gethexValueId();
    /* Verify if the retrieved hex_value_id matches the expected value */
    EXPECT_EQ(actual_hex_value_id, 0x10);
    std::cerr << "Finished TestGetHexValueId" << std::endl;
}

/* Test to verify the startTimerThread function */
TEST_F(ReceiveFramesTest, TestStartTimerThread)
{
    std::cerr << "Running TestStartTimerThread" << std::endl;
    receive_frames->stopTimerThread(); // Stop the timer thread if it's already running
    receive_frames->startTimerThread(); // Start the timer thread
    EXPECT_TRUE(receive_frames->running);
    std::this_thread::sleep_for(std::chrono::seconds(2)); // Wait for 2 seconds
    receive_frames->stopTimerThread(); // Stop the timer thread
    EXPECT_FALSE(receive_frames->running);
    std::cerr << "Finished TestStartTimerThread" << std::endl;
}

/* Test to check if the timer resets */
TEST_F(ReceiveFramesTest, TimerReset)
{
    std::cerr << "Running TimerReset" << std::endl;
    uint8_t ecu_id = 0x11;
    receive_frames->resetTimer(ecu_id);
    {
        std::lock_guard<std::mutex> lock(receive_frames->queue_mutex);
        auto it = receive_frames->ecu_timers.find(ecu_id);
        EXPECT_NE(it, receive_frames->ecu_timers.end());
    }
    std::this_thread::sleep_for(std::chrono::seconds(1));
    auto oldTimePoint = receive_frames->ecu_timers[ecu_id];
    receive_frames->resetTimer(ecu_id);
    {
        std::lock_guard<std::mutex> lock(receive_frames->queue_mutex);
        auto newTimePoint = receive_frames->ecu_timers[ecu_id];
        EXPECT_GT(newTimePoint, oldTimePoint);
    }
    std::cerr << "Finished TimerReset" << std::endl;
}

TEST_F(ReceiveFramesTest, TimerExpiryAndFrameSending)
{
    std::cerr << "Running TimerExpiryAndFrameSending" << std::endl;
    uint8_t ecu_id = 0x11;
    receive_frames->timeout_duration = std::chrono::seconds(2);
    receive_frames->resetTimer(ecu_id);
    std::this_thread::sleep_for(std::chrono::seconds(3));
    {
        std::lock_guard<std::mutex> lock(receive_frames->queue_mutex);
        auto it = receive_frames->ecu_timers.find(ecu_id);
        EXPECT_EQ(it, receive_frames->ecu_timers.end());
    }
    std::cerr << "Finished TimerExpiryAndFrameSending" << std::endl;
}
  
/* Main function to run all tests */
int main(int argc, char **argv)
{
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}