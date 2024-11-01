#include <cstring>
#include <string>
#include <thread>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <gtest/gtest.h>
#include <net/if.h>

#include "../include/ReceiveFrames.h"
#include "../include/GenerateFrames.h"

int socket1;
int socket2;

int createSocket()
{
    /* Create socket */
    std::string name_interface = "vcan1";
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

struct ReceiveFramesTest : testing::Test
{
    ReceiveFrames* r;
    HandleFrames* h;
    Logger* logger;
    GenerateFrames* g;
    ReceiveFramesTest()
    {
        logger = new Logger();
        r = new ReceiveFrames(socket2,0x11, *logger);
        h = new HandleFrames(socket2, *logger);
        g = new GenerateFrames(socket1, *logger);
    }
    ~ReceiveFramesTest()
    {
        delete r;
        delete h;
        delete g;
        delete logger;
    }
};

// TEST_F(ReceiveFramesTest, WrongSocket)
// {
//     std::cerr << "Running WrongSocket" << std::endl;
//     testing::internal::CaptureStdout();
//     ReceiveFrames* local_receive_frames = new ReceiveFrames(-1, 0x11, *logger);

//     std::string output = testing::internal::GetCapturedStdout();
//     EXPECT_NE(output.find("Error: Pass a valid Socket\n"), std::string::npos);
//     delete local_receive_frames;
//     std::cerr << "Finished WrongSocket" << std::endl;
// }

// TEST_F(ReceiveFramesTest, WrongModuleId)
// {
//     std::cerr << "Running WrongModuleId" << std::endl;
//     testing::internal::CaptureStdout();
//     ReceiveFrames* local_receive_frames = new ReceiveFrames(socket2, 0x8FFFFFFF, *logger);

//     std::string output = testing::internal::GetCapturedStdout();
//     EXPECT_NE(output.find("Error: Pass a valid Module ID\n"), std::string::npos);
//     delete local_receive_frames;
//     std::cerr << "Finished WrongModuleId" << std::endl;
// }

TEST_F(ReceiveFramesTest, EcuState)
{
    std::cerr << "Running EcuState" << std::endl;
    r->setEcuState(true);
    EXPECT_EQ(r->getEcuState(), true);
    std::cerr << "Finished EcuState" << std::endl;
}

TEST_F(ReceiveFramesTest, InvalidCANId)
{
    std::cerr << "Running InvalidCANId" << std::endl;
    std::thread processor_thread([this] {
        r->receive(*h);
    });
    testing::internal::CaptureStdout();
    g->sendFrame(0x0011, {0x02,0x10,0x02}, DATA_FRAME);
    sleep(1);
    std::string output = testing::internal::GetCapturedStdout();
    EXPECT_NE(output.find("Invalid CAN ID: upper 8 bits are zero\n"), std::string::npos);
    r->stop();
    processor_thread.join();
    std::cerr << "Finished InvalidCANId" << std::endl;
}

TEST_F(ReceiveFramesTest, MCUUnlocked)
{
    std::cerr << "Running MCUUnlocked" << std::endl;
    std::thread processor_thread([this] {
        r->receive(*h);
    });
    testing::internal::CaptureStdout();
    g->sendFrame(0x1011, {0x01,0xCE}, DATA_FRAME);
    sleep(1);
    std::string output = testing::internal::GetCapturedStdout();
    EXPECT_NE(output.find("Notification from the MCU that the server is unlocked."), std::string::npos);
    r->stop();
    processor_thread.join();
    std::cerr << "Finished MCUUnlocked" << std::endl;
}

TEST_F(ReceiveFramesTest, MCULocked)
{
    std::cerr << "Running MCULocked" << std::endl;
    testing::internal::CaptureStdout();
    std::thread processor_thread([this] {
        r->receive(*h);
    });
    g->sendFrame(0x1011, {0x01,0xCF}, DATA_FRAME);
    sleep(1);
    std::string output = testing::internal::GetCapturedStdout();
    EXPECT_NE(output.find("Notification from the MCU that the server is locked."), std::string::npos);
    r->stop();
    processor_thread.join();
    std::cerr << "Finished MCULocked" << std::endl;
}

TEST_F(ReceiveFramesTest, NotificationUp)
{
    std::cerr << "Running NotificationUp" << std::endl;
    testing::internal::CaptureStdout();
    std::thread processor_thread([this] {
        r->receive(*h);
    });
    g->sendFrame(0x1011, {0x01,0x99}, DATA_FRAME);
    sleep(1);
    std::string output = testing::internal::GetCapturedStdout();
    EXPECT_NE(output.find("Request received from MCU"), std::string::npos);
    EXPECT_NE(output.find("Response sent to MCU"), std::string::npos);
    r->stop();
    processor_thread.join();
    std::cerr << "Finished NotificationUp" << std::endl;
}

TEST_F(ReceiveFramesTest, EngineExecuteService)
{
    std::cerr << "Running EngineCallService" << std::endl;
    r = new ReceiveFrames(socket2,0x12, *logger);
    testing::internal::CaptureStdout();
    std::thread processor_thread([this] {
        r->receive(*h);
    });
    g->sendFrame(0xfa12, {0x02,0x10, 0x02}, DATA_FRAME);
    sleep(1);
    std::string output = testing::internal::GetCapturedStdout();
    EXPECT_NE(output.find("Received CAN frame"), std::string::npos);
    EXPECT_NE(output.find("CAN ID: 0x8000fa12"), std::string::npos);
    EXPECT_NE(output.find("Data Length: 3"), std::string::npos);
    EXPECT_NE(output.find("Data: 0x2 0x10 0x2"), std::string::npos);
    EXPECT_NE(output.find("Calling HandleFrames module to parse the frame."), std::string::npos);
    r->stop();
    processor_thread.join();
    std::cerr << "Finished EngineExecuteService" << std::endl;
}

TEST_F(ReceiveFramesTest, DoorsExecuteService)
{
    std::cerr << "Running DoorsExecuteService" << std::endl;
    r = new ReceiveFrames(socket2,0x13, *logger);
    testing::internal::CaptureStdout();
    std::thread processor_thread([this] {
        r->receive(*h);
    });
    g->sendFrame(0xfa13, {0x02,0x10, 0x02}, DATA_FRAME);
    sleep(1);
    std::string output = testing::internal::GetCapturedStdout();
    EXPECT_NE(output.find("Received CAN frame"), std::string::npos);
    EXPECT_NE(output.find("CAN ID: 0x8000fa13"), std::string::npos);
    EXPECT_NE(output.find("Data Length: 3"), std::string::npos);
    EXPECT_NE(output.find("Data: 0x2 0x10 0x2"), std::string::npos);
    EXPECT_NE(output.find("Calling HandleFrames module to parse the frame."), std::string::npos);
    r->stop();
    processor_thread.join();
    std::cerr << "Finished DoorsExecuteService" << std::endl;
}

TEST_F(ReceiveFramesTest, HVACExecuteService)
{
    std::cerr << "Running HVACExecuteService" << std::endl;
    r = new ReceiveFrames(socket2,0x14, *logger);
    testing::internal::CaptureStdout();
    std::thread processor_thread([this] {
        r->receive(*h);
    });
    g->sendFrame(0xfa14, {0x02,0x31, 0x01}, DATA_FRAME);
    sleep(1);
    std::string output = testing::internal::GetCapturedStdout();
    EXPECT_NE(output.find("Received CAN frame"), std::string::npos);
    EXPECT_NE(output.find("CAN ID: 0x8000fa14"), std::string::npos);
    EXPECT_NE(output.find("Data Length: 3"), std::string::npos);
    EXPECT_NE(output.find("Data: 0x2 0x31 0x1"), std::string::npos);
    EXPECT_NE(output.find("Started frame processing timing for frame with SID 31 with max_time = 400 on ECU with id 20."), std::string::npos);
    EXPECT_NE(output.find("Calling HandleFrames module to parse the frame."), std::string::npos);
    r->stop();
    processor_thread.join();
    std::cerr << "Finished HVACExecuteService" << std::endl;
}

TEST_F(ReceiveFramesTest, StartTimerDefaultCase)
{
    std::cerr << "Running StartTimerDefaultCase" << std::endl;
    r = new ReceiveFrames(socket2,0x15, *logger);
    testing::internal::CaptureStdout();
    std::thread processor_thread([this] {
        r->receive(*h);
    });
    g->sendFrame(0xfa15, {0x02,0x10, 0x01}, DATA_FRAME);
    sleep(1);
    std::string output = testing::internal::GetCapturedStdout();
    EXPECT_NE(output.find("starTimer function called with an ecu id unknown 15."), std::string::npos);
    r->stop();
    processor_thread.join();
    std::cerr << "Finished StartTimerDefaultCase" << std::endl;
}

TEST_F(ReceiveFramesTest, StopEngine)
{
    std::cerr << "Running StopEngine" << std::endl;
    testing::internal::CaptureStdout();
    r->startTimer(0x12, 0x10);
    sleep(3);
    std::string output = testing::internal::GetCapturedStdout();
    EXPECT_NE(output.find("Service with SID 10 sent the response pending frame."), std::string::npos);
    std::cerr << "Finished StopEngine" << std::endl;
}

TEST_F(ReceiveFramesTest, StopDoors)
{
    std::cerr << "Running StopDoors" << std::endl;
    testing::internal::CaptureStdout();
    r->startTimer(0x13, 0x10);
    sleep(3);
    std::string output = testing::internal::GetCapturedStdout();
    EXPECT_NE(output.find("Service with SID 10 sent the response pending frame."), std::string::npos);
    std::cerr << "Finished StopDoors" << std::endl;
}

TEST_F(ReceiveFramesTest, StopHVAC)
{
    std::cerr << "Running StopHVAC" << std::endl;
    testing::internal::CaptureStdout();
    r->startTimer(0x14, 0x10);
    sleep(3);
    std::string output = testing::internal::GetCapturedStdout();
    EXPECT_NE(output.find("Service with SID 10 sent the response pending frame."), std::string::npos);
    std::cerr << "Finished StopHVAC" << std::endl;
}

TEST_F(ReceiveFramesTest, StopTimerDefaultCase)
{
    std::cerr << "Running StopTimerDefaultCase" << std::endl;
    testing::internal::CaptureStdout();
    r->stopTimer(0x15, 0x10);
    std::string output = testing::internal::GetCapturedStdout();
    EXPECT_NE(output.find("stopTimer function called with an ecu id unknown 15."), std::string::npos);
    std::cerr << "Finished StopTimerDefaultCase" << std::endl;
}

int main(int argc, char* argv[])
{
    socket1 = createSocket();
    socket2 = createSocket();
    testing::InitGoogleTest(&argc, argv);
    int result = RUN_ALL_TESTS();
    if (socket1 > 0)
    {
        close(socket1);
    }
    if (socket2 > 0)
    {
        close(socket2);
    }
    return result;
}