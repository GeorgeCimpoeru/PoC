/**
 * @file read_dtc_information_test.cpp
 * @author Mujdei Ruben
 * @brief Unit test for Read_dtc service
 * @version 0.1
 * @date 2024-07-03
 * 
 * @copyright Copyright (c) 2024
 * 
 * PS: Test work with the next data in the file 'dtcs.txt':
 *          P0A9B-17 24
 *          P0805-11 2F
 *          B1234-00 00
 *          P0B12-01 10
 * 
 */
#include "../include/ReadDtcInformation.h"

#include <cstring>
#include <string>
#include <thread>

#include <fcntl.h>
#include <sys/ioctl.h>
#include <gtest/gtest.h>
#include <net/if.h>

int socket_;
/* Const */
const int id = 0x3412;

/* Class to capture the frame sin the can-bus */
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
    struct can_frame frame;
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
    EXPECT_EQ(expected_frame.can_id, c1.frame.can_id);
    EXPECT_EQ(expected_frame.can_dlc, c1.frame.can_dlc);
    for (int i = 0; i < expected_frame.can_dlc; ++i) {
        EXPECT_EQ(expected_frame.data[i], c1.frame.data[i]);
    }
}
/* Create object for all tests */
struct ReadDtcTest : testing::Test
{
    ReadDTC* r;
    CaptureFrame* c1;
    Logger* logger;
    ReadDtcTest()
    {
        logger = new Logger("log_test_read_dtc","./log_test_read_dtc.log");
        r = new ReadDTC(*logger, "./dtcs.txt");
        c1 = new CaptureFrame();
    }
    ~ReadDtcTest()
    {
        delete r;
        delete c1;
    }
};

TEST_F(ReadDtcTest, SubFunction1)
{
    struct can_frame result_frame = createFrame({0x06, 0x59, 0x01, 0x3F, 0x01, 0x00, 0x02});

    r->read_dtc(0x1234, {0x4,0x19,0x01, 0x84});
    c1->capture();
    testFrames(result_frame, *c1);
}

TEST_F(ReadDtcTest, SubFunction2)
{
    struct can_frame result_frame = createFrame({0x06, 0x59, 0x02, 0x3F, 0x0B, 0x12, 0x01, 0x10});
    r->read_dtc(0x1234, {0x4,0x19,0x02, 0x10});
    c1->capture();
    testFrames(result_frame, *c1);
}

TEST_F(ReadDtcTest, SubFunction2_Test2)
{
    struct can_frame result_frame = createFrame({0x10, 0x0B, 0x59, 0x02, 0x3F, 0x0A, 0x9B, 0x17});
    Logger logger;
    GenerateFrames g = GenerateFrames(socket_,logger);
    g.flowControlFrame(0x1234);
    r->read_dtc(0x1234, {0x4,0x19,0x02, 0x84});
    c1->capture();
    
    testFrames(result_frame, *c1);
}

TEST_F(ReadDtcTest, SubFunction2_Test3)
{
    testing::internal::CaptureStdout();
    r->read_dtc(0x1234, {0x4,0x19,0x02, 0x84});
    c1->capture();
    std::string output = testing::internal::GetCapturedStdout();
    EXPECT_NE(output.find("Timeout. FLow control frame not received!"), std::string::npos);
}

int main(int argc, char* argv[])
{
    socket_ = createSocket();
    testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}