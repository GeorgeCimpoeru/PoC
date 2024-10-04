/**
 * @file TesterPresentTest.cpp
 * @author Theodor Stoica
 * @brief Unit test for TesterPresent service
 * @version 0.1
 * @date 2024-07-03
 * 
 */

#include "../include/TesterPresent.h"
#include <cstring>
#include <string>
#include <thread>

#include <fcntl.h>
#include <sys/ioctl.h>
#include <gtest/gtest.h>
#include <net/if.h>

int socket_;
int socket2_;
/*ID frame for response */
const int id = 0x10FA;

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

struct can_frame createFrame(std::vector<uint8_t> test_data)
{
    struct can_frame result_frame;
    result_frame.can_id = id;
    int i = 0;
    for (auto d : test_data)
    {
        result_frame.data[i++] = d;
    }
    result_frame.can_dlc = test_data.size();
    return result_frame;
}

bool containsLine(const std::string& output, const std::string& line)
{
    return output.find(line) != std::string::npos;
}

class CaptureFrame
{
    public:
        struct can_frame frame;
        void capture()
        {
            read(socket_, &frame, sizeof(struct can_frame));
        }
};

void testFrames(struct can_frame expected_frame, CaptureFrame &c1 )
{
    EXPECT_EQ(expected_frame.can_id && 0xFFFF, c1.frame.can_id && 0xFFFF);
    EXPECT_EQ(expected_frame.can_dlc, c1.frame.can_dlc);
    for (int i = 0; i < expected_frame.can_dlc; ++i)
    {
        EXPECT_EQ(expected_frame.data[i], c1.frame.data[i]);
    }
}

struct TesterPresentTest : testing::Test
{
    TesterPresent* tp;
    CaptureFrame* c1;
    Logger* logger;
    DiagnosticSessionControl* session;
    TesterPresentTest()
    {
        logger = new Logger();
        session = new DiagnosticSessionControl(*logger, socket2_);
        tp = new TesterPresent(socket2_, *logger,*session);
        c1 = new CaptureFrame();
        /* Set session to default for each test */
        session->sessionControl(0xFA10,0x01);
        /* Read the frame to be ready to read the next frame */
        c1->capture();
    }
    ~TesterPresentTest()
    {
        delete tp;
        delete c1;
        delete session;
        delete logger;
    }
};

TEST_F(TesterPresentTest, SetEndTimeDefaultSession)
{
    tp->setEndTimeProgrammingSession();
    auto expected_programming_end_time = std::chrono::steady_clock::now();
    auto end_time = tp->getEndTimeProgrammingSession();
    EXPECT_NEAR(std::chrono::duration_cast<std::chrono::hours>
        (
            end_time - expected_programming_end_time
        ).count(),24 * 365, 1);
}

TEST_F(TesterPresentTest, SetEndTimeProgrammingSession)
{
    tp->setEndTimeProgrammingSession(true);
    auto expected_programming_end_time = std::chrono::steady_clock::now();
    auto end_time = tp->getEndTimeProgrammingSession();
    EXPECT_NEAR(std::chrono::duration_cast<std::chrono::seconds>
        (
            end_time - expected_programming_end_time
        ).count(),TesterPresent::S3_TIMER, 1);
}

TEST_F(TesterPresentTest, IncorrectMesssageLength)
{
    struct can_frame result_frame = createFrame({0x03, 0x7F, 
                    TesterPresent::TESTER_PRESENT_SID, NegativeResponse::IMLOIF});

    tp->handleTesterPresent(0xFA10, {0x04, 0x2E, 0x00});
    c1->capture();
    testFrames(result_frame, *c1);
}

TEST_F(TesterPresentTest, SubFunctionNotSupported)
{
    struct can_frame result_frame = createFrame({0x03, 0x7F, 
                    TesterPresent::TESTER_PRESENT_SID, NegativeResponse::SFNS});

    tp->handleTesterPresent(0xFA10, {0x02, 0x3E, 0x01});
    c1->capture();
    testFrames(result_frame, *c1);
}

TEST_F(TesterPresentTest, ProgrammingToProgrammingSession)
{
    struct can_frame result_frame = createFrame({0x02, 0x7E, 0x00});
    session->sessionControl(0xFA10, 0x02);
    c1->capture();
    tp->handleTesterPresent(0xFA10, {0x02, 0x3E, 0x00});
    c1->capture();
    testFrames(result_frame, *c1);
}

TEST_F(TesterPresentTest, DefaultToProgrammingSession)
{
    struct can_frame result_frame = createFrame({0x02, 0x7E, 0x00});
    tp->handleTesterPresent(0xFA10, {0x02, 0x3E, 0x00});
    c1->capture();
    testFrames(result_frame, *c1);
}

int main(int argc, char* argv[])
{
    socket_ = createSocket();
    socket2_ = createSocket();
    testing::InitGoogleTest(&argc, argv);
    int result = RUN_ALL_TESTS();
    if (socket_ > 0)
    {
        close(socket_); 
    }
    if (socket2_ > 0)
    {
        close(socket2_);
    }
    return result;
}
