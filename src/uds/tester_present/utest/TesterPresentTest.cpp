/**
 * @file tester_present_test.cpp
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
/* Const */
const int id = 0x1011;

/* Class to capture the frames on the CAN bus */
class CaptureFrame
{
public:
    struct can_frame frame;
    void capture()
    {
        int nbytes = read(socket_, &frame, sizeof(struct can_frame));
    }
};

struct can_frame createFrame(std::vector<uint8_t> test_data)
{
    struct can_frame result_frame;
    result_frame.can_id = id;
    int i = 0;
    for (auto d : test_data)
        result_frame.data[i++] = d;
    result_frame.can_dlc = test_data.size();
    return result_frame;
}

int createSocket()
{
    /* Create socket */
    std::string name_interface = "vcan0";
    struct can_frame frame;
    struct sockaddr_can addr;
    struct ifreq ifr;
    int s;

    s = socket(PF_CAN, SOCK_RAW, CAN_RAW);
    if (s < 0)
    {
        std::cout << "Error trying to create the socket\n";
        return 1;
    }
    /* Giving name and index to the interface created */
    strcpy(ifr.ifr_name, name_interface.c_str());
    ioctl(s, SIOCGIFINDEX, &ifr);
    /* Set addr structure with info. of the CAN interface */
    addr.can_family = AF_CAN;
    addr.can_ifindex = ifr.ifr_ifindex;
    /* Bind the socket to the CAN interface */
    int b = bind(s, (struct sockaddr *)&addr, sizeof(addr));
    if (b < 0)
    {
        std::cout << "Error binding\n";
        return 1;
    }
    int flags = fcntl(s, F_GETFL, 0);
    if (flags == -1)
    {
        return 1;
    }
    // Set the O_NONBLOCK flag to make the socket non-blocking
    flags |= O_NONBLOCK;
    if (fcntl(s, F_SETFL, flags) == -1)
    {
        return -1;
    }
    return s;
}

void testFrames(struct can_frame expected_frame, CaptureFrame &c1)
{
    EXPECT_EQ(expected_frame.can_id, c1.frame.can_id);
    EXPECT_EQ(expected_frame.can_dlc, c1.frame.can_dlc);
    for (int i = 0; i < expected_frame.can_dlc; ++i)
    {
        EXPECT_EQ(expected_frame.data[i], c1.frame.data[i]);
    }
}

/* Create object for all tests */
struct TesterPresentTest : testing::Test
{
    TesterPresent *tp;
    CaptureFrame *c1;
    Logger *logger;

    TesterPresentTest()
    {
        logger = new Logger("log_test_tester_present", "./log_test_tester_present.log");
        tp = new TesterPresent(*logger, socket_, 5);
        c1 = new CaptureFrame();
    }

    ~TesterPresentTest()
    {
        delete tp;
        delete c1;
        delete logger;
    }
};

TEST_F(TesterPresentTest, HandleTesterPresentValid)
{
    struct can_frame result_frame = createFrame({0x03, 0x7E, 0x00});
    tp->handleTesterPresent(0x1011, {0x2, 0x3E, 0x00});
    c1->capture();
    testFrames(result_frame, *c1);
}

TEST_F(TesterPresentTest, HandleTesterPresentInvalidLength)
{
    struct can_frame result_frame = createFrame({0x03, 0x7F, 0x3E, 0x13});
    tp->handleTesterPresent(0x1011, {0x2, 0x3E});
    c1->capture();
    testFrames(result_frame, *c1);
}

TEST_F(TesterPresentTest, HandleTesterPresentInvalidSubFunction)
{
    struct can_frame result_frame = createFrame({0x03, 0x7F, 0x3E, 0x12});
    tp->handleTesterPresent(0x1011, {0x2, 0x3E, 0x01});
    c1->capture();
    testFrames(result_frame, *c1);
}

int main(int argc, char *argv[])
{
    socket_ = createSocket();
    testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
