/**
 * @file SecurityAccessTest.cpp
 * @author Theodor Stoica
 * @brief Unit test for SecurityAccess Service
 * @version 0.1
 * @date 2024-07-16
 */
#include "../include/SecurityAccess.h"

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
        result_frame.data[i++] = d;
    result_frame.can_dlc = test_data.size();
    return result_frame;
}

int createSocket()
{
    /* Create socket */
    std::string name_interface = "vcan0";
    struct sockaddr_can addr;
    struct ifreq ifr;
    int s;

    s = socket(PF_CAN, SOCK_RAW, CAN_RAW);
    if( s<0)
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
    int b = bind( s, (struct sockaddr*)&addr, sizeof(addr));
    if( b < 0 )
    {
        std::cout<<"Error binding\n";
        return 1;
    }
    int flags = fcntl(s, F_GETFL, 0);
    if (flags == -1) {
        return 1;
    }
    // Set the O_NONBLOCK flag to make the socket non-blocking
    flags |= O_NONBLOCK;
    if (fcntl(s, F_SETFL, flags) == -1) {
        return -1;
    }
    return s;
}

void testFrames(struct can_frame expected_frame, CaptureFrame &c1 )
{
    EXPECT_EQ(expected_frame.can_id && 0xFFFF, c1.frame.can_id && 0xFFFF);
    EXPECT_EQ(expected_frame.can_dlc, c1.frame.can_dlc);
    for (int i = 0; i < expected_frame.can_dlc; ++i) {
        EXPECT_EQ(expected_frame.data[i], c1.frame.data[i]);
    }
}

struct SecurityTest : testing::Test
{
    SecurityAccess* r;
    CaptureFrame* c1;
    Logger* logger;
    SecurityTest()
    {
        logger = new Logger();
        r = new SecurityAccess(socket2_, *logger);
        c1 = new CaptureFrame();
    }
    ~SecurityTest()
    {
        delete r;
        delete c1;
    }
};
/* MCU is locked by default */
TEST_F(SecurityTest, DefaultMCUStateTest)
{
    EXPECT_FALSE(SecurityAccess::getMcuState());
}

TEST_F(SecurityTest, IncorrectMesssageLength)
{
    struct can_frame result_frame = createFrame({0x03, 0x7F, 
                    SecurityAccess::SECURITY_ACCESS_SID, SecurityAccess::IMLOIF});

    r->securityAccess(0xFA10, {0x03, 0x27, 0x03, 0x3F, 0x01, 0x00, 0x02});
    c1->capture();
    testFrames(result_frame, *c1);
}

/* We support only sf 0x01 and 0x02*/
TEST_F(SecurityTest, SubFunctionNotSupported)
{
    struct can_frame result_frame = createFrame({0x03, 0x7F, 
                    SecurityAccess::SECURITY_ACCESS_SID, SecurityAccess::SFNS});

    r->securityAccess(0xFA10, {0x06, SecurityAccess::SECURITY_ACCESS_SID, 
                    0x03, 0x3F, 0x01, 0x00, 0x02});
    c1->capture();
    testFrames(result_frame, *c1);
}

TEST_F(SecurityTest, RequestSeed)
{
    struct can_frame result_frame = createFrame({0x03, 0x7F, 
                    SecurityAccess::SECURITY_ACCESS_SID, SecurityAccess::SFNS});

    r->securityAccess(0xFA10, {0x02, 0x27, 0x01});
    c1->capture();
}

int main(int argc, char* argv[])
{
    socket_ = createSocket();
    socket2_ = createSocket();
    testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
