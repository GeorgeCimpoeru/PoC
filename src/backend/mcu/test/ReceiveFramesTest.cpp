#include <gtest/gtest.h>
#include <unistd.h>
#include <linux/can.h>
#include <sys/socket.h>
#include <thread>
#include <sstream>
#include "../include/ReceiveFrames.h"
#include "../../uds/diagnostic_session_control/include/DiagnosticSessionControl.h"
#include <vector>
#include <map>
#include <chrono>
#include <mutex>
#include <condition_variable>
#include <atomic>

std::vector<uint8_t> seed;
int socket_api;

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

class CaptureFrame
{
    public:
        struct can_frame frame;
        void capture()
        {
            read(socket_api, &frame, sizeof(struct can_frame));
        }
};
/* Test fixture class for ReceiveFrames tests */
class ReceiveFramesTest : public ::testing::Test
{
protected:
    int mock_socket_canbus;
    int mock_socket_api;
    int mock_socket_pair_canbus[2];
    int mock_socket_pair_api[2];
    int socket1;
    int socket2;
    int socket3;
    int socket4;
    Logger* logger;
    MockReceiveFrames* receive_frames;
    MockReceiveFrames* receive_frames_2;
    SecurityAccess* r;
    CaptureFrame* c1;
    /* Setup method to initialize test environment */
    virtual void SetUp()
    {
        /* Create mock socket pairs for testing */
        socketpair(AF_UNIX, SOCK_STREAM, 0, mock_socket_pair_canbus);
        socketpair(AF_UNIX, SOCK_STREAM, 0, mock_socket_pair_api);
        mock_socket_canbus = mock_socket_pair_canbus[0];
        mock_socket_api = mock_socket_pair_api[0];
        socket1 = createSocket("vcan1");
        socket2 = createSocket("vcan0");
        socket3 = createSocket("vcan0");
        socket4 = createSocket("vcan1");
        receive_frames = new MockReceiveFrames(mock_socket_canbus, mock_socket_api);
        receive_frames_2 = new MockReceiveFrames(socket2, socket1);
        r = new SecurityAccess(socket4, *logger);
        c1 = new CaptureFrame();
    }
    /* Teardown method to clean up after each test */
    virtual void TearDown()
    {
        delete receive_frames;
        delete receive_frames_2;
        delete r;
        delete c1;
        close(mock_socket_pair_canbus[0]);
        close(mock_socket_pair_canbus[1]);
        close(mock_socket_pair_api[0]);
        close(mock_socket_pair_api[1]);
    }

public:
    static int createSocket(std::string name_interface)
    {
        struct sockaddr_can addr;
        struct ifreq ifr;
        int s;

        s = socket(PF_CAN, SOCK_RAW, CAN_RAW);
        if (s < 0)
        {
            std::cout<<"Error trying to create the socket\n";
            return 1;
        }
        /* Giving name and index to the interface created */
        strcpy(ifr.ifr_name, name_interface.c_str() );
        ioctl(s, SIOCGIFINDEX, &ifr);
        /* Set addr structure with info. of the CAN interface */
        addr.can_family = AF_CAN;
        addr.can_ifindex = ifr.ifr_ifindex;
        /* Bind the socket to the CAN interface */
        int b = bind(s, (struct sockaddr*)&addr, sizeof(addr));
        if( b < 0 )
        {
            std::cout<<"Error binding\n";
            return 1;
        }
        int flags = fcntl(s, F_GETFL, 0);
        if (flags == -1)
        {
            return 1;
        }
        /* Set the O_NONBLOCK flag to make the socket non-blocking */
        flags |= O_NONBLOCK;
        if (fcntl(s, F_SETFL, flags) == -1)
        {
            return -1;
        }
        return s;
    }  
};

uint8_t computeKey(uint8_t& seed)
{
    return ~seed + 1;
}

TEST_F(ReceiveFramesTest, UnlockedSecurity)
{
    std::cerr << "Running UnlockedSecurity" << std::endl;
    /* Check the security */
    /* Request seed */
    r->securityAccess(0xFA10, {0x02, 0x27, 0x01});

    c1->capture();
    if (c1->frame.can_dlc >= 4)
    {
        seed.clear();
        /* from 3 to pci_length we have the seed generated in response */
        for (int i = 3; i <= c1->frame.data[0]; i++)
        {
            seed.push_back(c1->frame.data[i]);
        }
    }
    /* Compute key from seed */
    for (auto &elem : seed)
    {
        elem = computeKey(elem);
    }
    std::vector<uint8_t> data_frame = {static_cast<uint8_t>(seed.size() + 2), 0x27, 0x02};
    data_frame.insert(data_frame.end(), seed.begin(), seed.end());
    r->securityAccess(0xFA10, data_frame);
    c1->capture();

    struct can_frame frame;
    frame.can_id = 0xfa10;
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
    
    testing::internal::CaptureStdout();
    std::thread processor_thread([this] {
        receive_frames->processQueue();
    });
    receive_frames->stopProcessingQueue();
    processor_thread.join();

    std::string output = testing::internal::GetCapturedStdout();
    EXPECT_NE(output.find("Server is unlocked."), std::string::npos);
    std::cerr << "Finished UnlockedSecurity" << std::endl;
}

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
    testing::internal::CaptureStdout();
    std::thread processor_thread([this] {
        receive_frames->processQueue();
    });
    receive_frames->stopListenCANBus();
    processor_thread.join();

    std::string output = testing::internal::GetCapturedStdout();
    EXPECT_NE(output.find("Frame processing method invoked!"), std::string::npos);
    EXPECT_NE(output.find("Frame with ID: 0x2210 is taken from processing queue"), std::string::npos);
    EXPECT_NE(output.find("Received CAN frame"), std::string::npos);
    EXPECT_NE(output.find("Module ID: 0x2210"), std::string::npos);
    EXPECT_NE(output.find("Data Length: 4"), std::string::npos);
    EXPECT_NE(output.find("Data: 0x0 0x22 0x2 0x3"), std::string::npos);
    EXPECT_NE(output.find("Started frame processing timing for frame with SID 22 with max_time = 40."), std::string::npos);
    EXPECT_NE(output.find("Received frame for MCU to execute service with SID: 0x22"), std::string::npos);
    EXPECT_NE(output.find("Calling HandleFrames module to execute the service and parse the frame."), std::string::npos);
    EXPECT_NE(output.find("Server is locked."), std::string::npos);
    std::cerr << "Finished TestProcessQueue_ForMCU" << std::endl;
}
  
/* Test to process queue with an ECU-up-specific CAN frame */
TEST_F(ReceiveFramesTest, TestProcessQueue_ForBatteryUp)
{
    std::cerr << "Running TestProcessQueue_ForBatteryUp" << std::endl;
    struct can_frame frame;
    frame.can_id = 0x1110;
    frame.can_dlc = 4;
    for (uint8_t itr = 0; itr < 4; ++itr)
    {
        frame.data[itr] = itr;
    }
    frame.data[1] = 0xd9;
    receive_frames->startListenCANBus(); 
    {
        std::lock_guard<std::mutex> lock(receive_frames->queue_mutex);
        /* Push frame to queue */
        receive_frames->frame_queue.push(frame);
    }
    receive_frames->queue_cond_var.notify_one();
    testing::internal::CaptureStdout();
    std::thread processor_thread([this] {
        receive_frames->processQueue();
    });
    receive_frames->stopListenCANBus();
    processor_thread.join();

    std::string output = testing::internal::GetCapturedStdout();
    EXPECT_NE(output.find("Frame processing method invoked!"), std::string::npos);
    EXPECT_NE(output.find("Frame with ID: 0x1110 is taken from processing queue"), std::string::npos);
    EXPECT_NE(output.find("Received CAN frame"), std::string::npos);
    EXPECT_NE(output.find("Module ID: 0x1110"), std::string::npos);
    EXPECT_NE(output.find("Data Length: 4"), std::string::npos);
    EXPECT_NE(output.find("Data: 0x0 0xd9 0x2 0x3"), std::string::npos);
    EXPECT_NE(output.find("Frame received to notify MCU that ECU with ID: 0x11 is up"), std::string::npos);
    std::cerr << "Finished TestProcessQueue_ForBatteryUp" << std::endl;
}
/* Test to process queue with an ECU-up-specific CAN frame */
TEST_F(ReceiveFramesTest, TestProcessQueue_ForEngineUp)
{
    std::cerr << "Running TestProcessQueue_ForEngineUp" << std::endl;
    struct can_frame frame;
    frame.can_id = 0x1210;
    frame.can_dlc = 4;
    for (uint8_t itr = 0; itr < 4; ++itr)
    {
        frame.data[itr] = itr;
    }
    frame.data[1] = 0xd9;
    receive_frames->startListenCANBus(); 
    {
        std::lock_guard<std::mutex> lock(receive_frames->queue_mutex);
        /* Push frame to queue */
        receive_frames->frame_queue.push(frame);
    }
    receive_frames->queue_cond_var.notify_one();
    testing::internal::CaptureStdout();
    std::thread processor_thread([this] {
        receive_frames->processQueue();
    });
    receive_frames->stopListenCANBus();
    processor_thread.join();

    std::string output = testing::internal::GetCapturedStdout();
    EXPECT_NE(output.find("Frame processing method invoked!"), std::string::npos);
    EXPECT_NE(output.find("Frame with ID: 0x1210 is taken from processing queue"), std::string::npos);
    EXPECT_NE(output.find("Received CAN frame"), std::string::npos);
    EXPECT_NE(output.find("Module ID: 0x1210"), std::string::npos);
    EXPECT_NE(output.find("Data Length: 4"), std::string::npos);
    EXPECT_NE(output.find("Data: 0x0 0xd9 0x2 0x3"), std::string::npos);
    EXPECT_NE(output.find("Frame received to notify MCU that ECU with ID: 0x12 is up"), std::string::npos);
    std::cerr << "Finished TestProcessQueue_ForEngineUp" << std::endl;
}

/* Test to process queue with an ECU-up-specific CAN frame */
TEST_F(ReceiveFramesTest, TestProcessQueue_ForDoorsUp)
{
    std::cerr << "Running TestProcessQueue_ForDoorsUp" << std::endl;
    struct can_frame frame;
    frame.can_id = 0x1310;
    frame.can_dlc = 4;
    for (uint8_t itr = 0; itr < 4; ++itr)
    {
        frame.data[itr] = itr;
    }
    frame.data[1] = 0xd9;
    receive_frames->startListenCANBus(); 
    {
        std::lock_guard<std::mutex> lock(receive_frames->queue_mutex);
        /* Push frame to queue */
        receive_frames->frame_queue.push(frame);
    }
    receive_frames->queue_cond_var.notify_one();
    testing::internal::CaptureStdout();
    std::thread processor_thread([this] {
        receive_frames->processQueue();
    });
    receive_frames->stopListenCANBus();
    processor_thread.join();

    std::string output = testing::internal::GetCapturedStdout();
    EXPECT_NE(output.find("Frame processing method invoked!"), std::string::npos);
    EXPECT_NE(output.find("Frame with ID: 0x1310 is taken from processing queue"), std::string::npos);
    EXPECT_NE(output.find("Received CAN frame"), std::string::npos);
    EXPECT_NE(output.find("Module ID: 0x1310"), std::string::npos);
    EXPECT_NE(output.find("Data Length: 4"), std::string::npos);
    EXPECT_NE(output.find("Data: 0x0 0xd9 0x2 0x3"), std::string::npos);
    EXPECT_NE(output.find("Frame received to notify MCU that ECU with ID: 0x13 is up"), std::string::npos);
    std::cerr << "Finished TestProcessQueue_ForDoorsUp" << std::endl;
}

TEST_F(ReceiveFramesTest, TestProcessQueue_ForHVACUp)
{
    std::cerr << "Running TestProcessQueue_ForHVACUp" << std::endl;
    struct can_frame frame;
    frame.can_id = 0x1410;
    frame.can_dlc = 4;
    for (uint8_t itr = 0; itr < 4; ++itr)
    {
        frame.data[itr] = itr;
    }
    frame.data[1] = 0xd9;
    receive_frames->startListenCANBus(); 
    {
        std::lock_guard<std::mutex> lock(receive_frames->queue_mutex);
        /* Push frame to queue */
        receive_frames->frame_queue.push(frame);
    }
    receive_frames->queue_cond_var.notify_one();
    testing::internal::CaptureStdout();
    std::thread processor_thread([this] {
        receive_frames->processQueue();
    });
    receive_frames->stopListenCANBus();
    processor_thread.join();

    std::string output = testing::internal::GetCapturedStdout();
    EXPECT_NE(output.find("Frame processing method invoked!"), std::string::npos);
    EXPECT_NE(output.find("Frame with ID: 0x1410 is taken from processing queue"), std::string::npos);
    EXPECT_NE(output.find("Received CAN frame"), std::string::npos);
    EXPECT_NE(output.find("Module ID: 0x1410"), std::string::npos);
    EXPECT_NE(output.find("Data Length: 4"), std::string::npos);
    EXPECT_NE(output.find("Data: 0x0 0xd9 0x2 0x3"), std::string::npos);
    EXPECT_NE(output.find("Frame received to notify MCU that ECU with ID: 0x14 is up"), std::string::npos);
    std::cerr << "Finished TestProcessQueue_ForHVACUp" << std::endl;
}

TEST_F(ReceiveFramesTest, TestProcessQueue_ForDefaultUp)
{
    std::cerr << "Running TestProcessQueue_ForDefaultUp" << std::endl;
    struct can_frame frame;
    frame.can_id = 0x1510;
    frame.can_dlc = 4;
    for (uint8_t itr = 0; itr < 4; ++itr)
    {
        frame.data[itr] = itr;
    }
    frame.data[1] = 0xd9;
    receive_frames->startListenCANBus(); 
    {
        std::lock_guard<std::mutex> lock(receive_frames->queue_mutex);
        /* Push frame to queue */
        receive_frames->frame_queue.push(frame);
    }
    receive_frames->queue_cond_var.notify_one();
    testing::internal::CaptureStdout();
    std::thread processor_thread([this] {
        receive_frames->processQueue();
    });
    receive_frames->stopListenCANBus();
    processor_thread.join();

    std::string output = testing::internal::GetCapturedStdout();
    EXPECT_NE(output.find("Frame processing method invoked!"), std::string::npos);
    EXPECT_NE(output.find("Frame with ID: 0x1510 is taken from processing queue"), std::string::npos);
    EXPECT_NE(output.find("Received CAN frame"), std::string::npos);
    EXPECT_NE(output.find("Module ID: 0x1510"), std::string::npos);
    EXPECT_NE(output.find("Data Length: 4"), std::string::npos);
    EXPECT_NE(output.find("Data: 0x0 0xd9 0x2 0x3"), std::string::npos);
    EXPECT_NE(output.find("Frame received to notify MCU that ECU with ID: 0x15 is up"), std::string::npos);
    std::cerr << "Finished TestProcessQueue_ForDefaultUp" << std::endl;
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
    frame.data[1] = 0x22;
    receive_frames->startListenAPI(); 
    {
        std::lock_guard<std::mutex> lock(receive_frames->queue_mutex);
        /* Push frame to queue */
        receive_frames->frame_queue.push(frame);
    }
    receive_frames->queue_cond_var.notify_one();
    testing::internal::CaptureStdout();
    std::thread processor_thread([this] {
        receive_frames->processQueue();
    });
    receive_frames->stopListenAPI();
    processor_thread.join();

    std::string output = testing::internal::GetCapturedStdout();
    EXPECT_NE(output.find("Frame processing method invoked!"), std::string::npos);
    EXPECT_NE(output.find("Frame with ID: 0xfa11 is taken from processing queue"), std::string::npos);
    EXPECT_NE(output.find("Received CAN frame"), std::string::npos);
    EXPECT_NE(output.find("Module ID: 0xfa11"), std::string::npos);
    EXPECT_NE(output.find("Data Length: 4"), std::string::npos);
    EXPECT_NE(output.find("Data: 0x0 0x22 0x2 0x3"), std::string::npos);
    EXPECT_NE(output.find("Received frame for ECU to execute service with SID: 0x22"), std::string::npos);
    EXPECT_NE(output.find("Frame with ID: 0xfa11 sent on CANBus socket"), std::string::npos);
    std::cerr << "Finished TestProcessQueue_ForECU" << std::endl;
}

/* Test to process queue with an ECU-specific CAN frame */
TEST_F(ReceiveFramesTest, TestProcessQueue_ForECU2)
{
    std::cerr << "Running TestProcessQueue_ForECU2" << std::endl;
    struct can_frame frame;
    frame.can_id = 0xfa11;
    frame.can_dlc = 4;
    for (int i = 0; i < 4; ++i)
    {
        frame.data[i] = i;
    }
    frame.data[1] = 0x27;
    receive_frames->startListenAPI(); 
    {
        std::lock_guard<std::mutex> lock(receive_frames->queue_mutex);
        /* Push frame to queue */
        receive_frames->frame_queue.push(frame);
    }
    receive_frames->queue_cond_var.notify_one();
    testing::internal::CaptureStdout();
    std::thread processor_thread([this] {
        receive_frames->processQueue();
    });
    receive_frames->stopListenAPI();
    processor_thread.join();

    std::string output = testing::internal::GetCapturedStdout();
    EXPECT_NE(output.find("Frame processing method invoked!"), std::string::npos);
    EXPECT_NE(output.find("Frame with ID: 0xfa11 is taken from processing queue"), std::string::npos);
    EXPECT_NE(output.find("Received CAN frame"), std::string::npos);
    EXPECT_NE(output.find("Module ID: 0xfa11"), std::string::npos);
    EXPECT_NE(output.find("Data Length: 4"), std::string::npos);
    EXPECT_NE(output.find("Data: 0x0 0x27 0x2 0x3"), std::string::npos);
    EXPECT_NE(output.find("Received frame for ECU to execute service with SID: 0x27"), std::string::npos);
    EXPECT_NE(output.find("Frame with ID: 0xfa11 sent on CANBus socket"), std::string::npos);
    std::cerr << "Finished TestProcessQueue_ForECU" << std::endl;
}

/* Test to process queue with an response to api with all ECUs up */
TEST_F(ReceiveFramesTest, TestProcessQueue_ForResponseWithECUsUp)
{
    std::cerr << "Running TestProcessQueue_ForResponseWithECUsUp" << std::endl;
    struct can_frame frame;
    frame.can_id = 0xfa99;
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
    testing::internal::CaptureStdout();
    std::thread processor_thread([this] {
        receive_frames->processQueue();
    });
    receive_frames->stopListenAPI();
    processor_thread.join();

    std::string output = testing::internal::GetCapturedStdout();
    EXPECT_NE(output.find("Frame processing method invoked!"), std::string::npos);
    EXPECT_NE(output.find("Frame with ID: 0xfa99 is taken from processing queue"), std::string::npos);
    EXPECT_NE(output.find("Received CAN frame"), std::string::npos);
    EXPECT_NE(output.find("Module ID: 0xfa99"), std::string::npos);
    EXPECT_NE(output.find("Data Length: 4"), std::string::npos);
    EXPECT_NE(output.find("Data: 0x0 0x99 0x2 0x3"), std::string::npos);
    EXPECT_NE(output.find("Received frame to update status of ECUs still up."), std::string::npos);
    EXPECT_NE(output.find("Frame sent to API on API socket to update status of ECUs still up."), std::string::npos);
    std::cerr << "Finished TestProcessQueue_ForResponseWithECUsUp" << std::endl;
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
    testing::internal::CaptureStdout();
    std::thread processor_thread([this] {
        receive_frames->processQueue();
    });
    receive_frames->stopListenAPI();
    processor_thread.join();

    std::string output = testing::internal::GetCapturedStdout();
    EXPECT_NE(output.find("Frame processing method invoked!"), std::string::npos);
    EXPECT_NE(output.find("Frame with ID: 0x11fa is taken from processing queue"), std::string::npos);
    EXPECT_NE(output.find("Received CAN frame"), std::string::npos);
    EXPECT_NE(output.find("Module ID: 0x11fa"), std::string::npos);
    EXPECT_NE(output.find("Data Length: 4"), std::string::npos);
    EXPECT_NE(output.find("Data: 0x0 0x1 0x2 0x3"), std::string::npos);
    EXPECT_NE(output.find("Frame received from device with sender ID: 0x11 sent for API processing"), std::string::npos);
    EXPECT_NE(output.find("Frame with ID: 0x11fa sent on API socket"), std::string::npos);
    std::cerr << "Finished TestProcessQueue_ForAPI" << std::endl;
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
    EXPECT_NE(output.find("Received CAN frame"), std::string::npos);
    EXPECT_NE(output.find("Module ID: 0x123"), std::string::npos);
    EXPECT_NE(output.find("Data Length: 4"), std::string::npos);
    EXPECT_NE(output.find("Data: 0x0 0x1 0x2 0x3"), std::string::npos);
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

/* Test for StopTimer */
TEST_F(ReceiveFramesTest, TestStopTimer)
{
    std::cerr << "Running TestStopTimer" << std::endl;
    struct can_frame frame;
    frame.can_id = 0xfa10;
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
    testing::internal::CaptureStdout();
    std::thread processor_thread([this] {
        receive_frames->processQueue();
    });
    receive_frames->stopTimer(0x22);
    receive_frames->stopListenCANBus();
    processor_thread.join();

    std::string output = testing::internal::GetCapturedStdout();
    EXPECT_NE(output.find("stopTimer function called for frame with SID 22."), std::string::npos);
    std::cerr << "Finished TestStopTimer" << std::endl;
}

TEST_F(ReceiveFramesTest, ListenOnCANSocket)
{
    std::cerr << "Running ListenOnCANSocket" << std::endl;
    DiagnosticSessionControl* dsc = new DiagnosticSessionControl(*logger, socket3);
    receive_frames_2->startListenCANBus();
    testing::internal::CaptureStdout();
    std::thread processor_thread([this] {
        receive_frames_2->receiveFramesFromCANBus();
    });
    dsc->sessionControl(0x1011, 0x02, false);
    receive_frames_2->stopListenCANBus();
    processor_thread.join();
    std::string output = testing::internal::GetCapturedStdout();
    EXPECT_NE(output.find("Captured a frame on the CANBus socket"), std::string::npos);
    // EXPECT_NE(output.find("Passed a valid Module ID: 0x1110 and frame added to the processing queue."), std::string::npos);
    std::cerr << "Finished ListenOnCANSocket" << std::endl;
}

TEST_F(ReceiveFramesTest, ListenOnAPISocket)
{
    std::cerr << "Running ListenOnAPISocket" << std::endl;
    DiagnosticSessionControl* dsc = new DiagnosticSessionControl(*logger, socket4);
    receive_frames_2->startListenAPI();
    testing::internal::CaptureStdout();
    std::thread processor_thread([this] {
        receive_frames_2->receiveFramesFromAPI();
    });
    dsc->sessionControl(0x1011, 0x02, false);
    receive_frames_2->stopListenAPI();
    processor_thread.join();

    std::string output = testing::internal::GetCapturedStdout();
    EXPECT_NE(output.find("Captured a frame on the API socket"), std::string::npos);
    // EXPECT_NE(output.find("Passed a valid Module ID: fa10 and frame added to the processing queue."), std::string::npos);
    std::cerr << "Finished ListenOnAPISocket" << std::endl;
}

TEST_F(ReceiveFramesTest, StopProccesingQueue)
{
    std::cerr << "Running StopProccesingQueue" << std::endl;
    testing::internal::CaptureStdout();
    std::thread processor_thread([this] {
        receive_frames->processQueue();
    });
    receive_frames->stopProcessingQueue();
    processor_thread.join();

    std::string output = testing::internal::GetCapturedStdout();
    EXPECT_NE(output.find("Frame processing method invoked!"), std::string::npos);
    std::cerr << "Finished StopProccesingQueue" << std::endl;
}
  
/* Main function to run all tests */
int main(int argc, char **argv)
{
    socket_api = ReceiveFramesTest::createSocket("vcan1");
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}