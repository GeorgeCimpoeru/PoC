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
    MockReceiveFrames(int socket_api, int socket_canbus) : ReceiveFrames(socket_api, socket_canbus) {}
    using ReceiveFrames::frame_queue;
    using ReceiveFrames::queue_mutex;
    using ReceiveFrames::queue_cond_var;
    using ReceiveFrames::printFrames;
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
        receive_frames->frame_queue.push(frame);  /* Push frame to queue */
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
    std::string expected_output = 
        "Frame processing:...\n"
        "-------------------\n"
        "Processing CAN frame from queue:\n"
        "CAN ID: 0x2210\n"
        "Data Length: 4\n"
        "Data: 0 22 2 3 \n"
        "Frame for MCU Service\n"
        "Single Frame received:\n"
        "\nReadDataByIdentifier called.\n";
    EXPECT_EQ(output, expected_output);
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
        receive_frames->frame_queue.push(frame);  /* Push frame to queue */
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
    std::string expected_output = 
        "Frame processing:...\n"
        "-------------------\n"
        "Processing CAN frame from queue:\n"
        "CAN ID: 0x2210\n"
        "Data Length: 4\n"
        "Data: 0 ff 11 3 \n"
        "Notification from the ECU that it is up\n";
    EXPECT_EQ(output, expected_output);
    std::cerr << "Finished TestProcessQueue_ForECUUp" << std::endl;
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
        receive_frames->frame_queue.push(frame);  /* Push frame to queue */
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
    std::string expected_output = 
        "Frame processing:...\n"
        "-------------------\n"
        "Processing CAN frame from queue:\n"
        "CAN ID: 0x11fa\n"
        "Data Length: 4\n"
        "Data: 0 1 2 3 \n"
        "Frame for API Service\n";
        // "Request for download received.\n"
        // "Frame to be sent on CAN Bus...\n";
    EXPECT_EQ(output, expected_output);
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
        receive_frames->frame_queue.push(frame);  /* Push frame to queue */
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
    std::string expected_output = "Frame processing:...\n"
        "-------------------\n"
        "Processing CAN frame from queue:\n"
        "CAN ID: 0xff\n"
        "Data Length: 4\n"
        "Data: 0 1 2 3 \n"
        "Received the test frame \n";
    EXPECT_EQ(output, expected_output);
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

    std::string expected_output = 
        "-------------------\n"
        "Processing CAN frame from queue:\n"
        "CAN ID: 0x123\n"
        "Data Length: 4\n"
        "Data: 0 1 2 3 \n";
    EXPECT_EQ(output, expected_output);
    std::cerr << "Finished TestPrintFrames" << std::endl;
}

//* Test to verify the send_test_frame function */
TEST_F(ReceiveFramesTest, TestSendTestFrame)
{
    std::cerr << "Running TestSendTestFrame" << std::endl;
    testing::internal::CaptureStdout();
    receive_frames->startListenCANBus(); 
    receive_frames->sendTestFrame();
    std::string output = testing::internal::GetCapturedStdout();
    
    EXPECT_TRUE(output.empty());
    std::cerr << "Finished TestSendTestFrame" << std::endl;
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
    const std::vector<uint8_t>& ecus_up = receive_frames->getECUsUp();
    EXPECT_TRUE(ecus_up.empty()); // Assuming initial state is empty
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

/* Main function to run all tests */
int main(int argc, char **argv)
{
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}