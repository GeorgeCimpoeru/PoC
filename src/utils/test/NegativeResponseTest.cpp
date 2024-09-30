/**
 * @file SecurityAccessTest.cpp
 * @author Theodor Stoica
 * @brief Unit test for NegativeResponse Service
 * @version 0.1
 * @date 2024-07-16
 */
#include "../include/NegativeResponse.h"

#include <cstring>
#include <string>
#include <thread>

#include <fcntl.h>
#include <sys/ioctl.h>
#include <gtest/gtest.h>
#include <net/if.h>

int socket_;
int socket2_;
const int id = 0x10FA;

std::vector<uint8_t> seed;

class CaptureFrame
{
    public:
        struct can_frame frame;
        void capture()
        {
            read(socket_, &frame, sizeof(struct can_frame));
        }
};

struct can_frame createFrame(std::vector<uint8_t> test_data)
{
    struct can_frame result_frame;
    result_frame.can_id = id;
    int i=0;
    for (auto d : test_data)
    {
        result_frame.data[i++] = d;
    }
    result_frame.can_dlc = test_data.size();
    return result_frame;
}

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

void testFrames(struct can_frame expected_frame, CaptureFrame &c1 )
{
    EXPECT_EQ(expected_frame.can_id && 0xFFFF, c1.frame.can_id && 0xFFFF);
    EXPECT_EQ(expected_frame.can_dlc, c1.frame.can_dlc);
    for (int i = 0; i < expected_frame.can_dlc; ++i)
    {
        EXPECT_EQ(expected_frame.data[i], c1.frame.data[i]);
    }
}

bool containsLine(const std::string& output, const std::string& line)
{
    return output.find(line) != std::string::npos;
}

struct NegativeResponseTest : testing::Test
{
    NegativeResponse* r;
    CaptureFrame* c1;
    Logger* logger;
    NegativeResponseTest()
    {
        logger = new Logger();
        r = new NegativeResponse(socket2_, *logger);
        c1 = new CaptureFrame();
    }
    ~NegativeResponseTest()
    {
        delete r;
        delete c1;
        delete logger;
    }
};

TEST_F(NegativeResponseTest, SendNRC)
{
    struct can_frame result_frame = createFrame({0x03, 0x7F, 0x27, 0x13});
    r->sendNRC(0xFA10, 0x27, 0x13);
    c1->capture();
    testFrames(result_frame, *c1);
}

TEST_F(NegativeResponseTest, InvalidNRC)
{
    testing::internal::CaptureStdout();
    r->sendNRC(0xFA10, 0x27, 0x99);
    c1->capture();
    std::string output_nrc = "";
    std::string searchLine = "Negative Response not supported";
    output_nrc = testing::internal::GetCapturedStdout();
    EXPECT_TRUE(containsLine(output_nrc, searchLine));
}

TEST_F(NegativeResponseTest, AccessTimingNRC)
{
    struct can_frame result_frame = createFrame({0x03, 0x7F, 0x27, 0x78});
    r->sendNRC(0xFA10, 0x27, 0x78);
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
