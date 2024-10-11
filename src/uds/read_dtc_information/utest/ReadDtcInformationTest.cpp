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
int socket2_;

/* Class to capture the frame sin the can-bus */
class CaptureFrame
{
    public:
        struct can_frame frame;
        void capture()
        {
            read(socket_, &frame, sizeof(struct can_frame));
        }
};

struct can_frame createFrame(uint16_t can_id ,std::vector<uint8_t> test_data)
{
    struct can_frame result_frame;
    result_frame.can_id = can_id;
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
/* Create object for all tests */
struct ReadDtcTest : testing::Test
{
    ReadDTC* r;
    CaptureFrame* c1;
    Logger* logger;
    ReadDtcTest()
    {
        std::string dtc_file_path = std::string(PROJECT_PATH) + "/src/ecu_simulation/EngineModule/dtcs.txt";
        logger = new Logger("log_test_read_dtc","./log_test_read_dtc.log");
        r = new ReadDTC(*logger, dtc_file_path,socket2_);
        c1 = new CaptureFrame();
    }
    ~ReadDtcTest()
    {
        delete r;
        delete c1;
    }
};

TEST_F(ReadDtcTest, IncorrectMessageLength)
{
    struct can_frame result_frame = createFrame(0x12fa, {0x03, 0x7F, 0x19, 0x13});

    r->read_dtc(0xfa12, {0x03, 0x19, 0x01});
    c1->capture();
    testFrames(result_frame, *c1);
}

TEST_F(ReadDtcTest, SubFunction1)
{
    struct can_frame result_frame = createFrame(0x12fa, {0x06, 0x59, 0x01, 0x24, 0x01, 0x00, 0x02});

    r->read_dtc(0xfa12, {0x4,0x19,0x01, 0xff});
    c1->capture();
    testFrames(result_frame, *c1);
}

TEST_F(ReadDtcTest, SubFunction2)
{
    struct can_frame result_frame = createFrame(0x10fa, {0x010, 0x09, 0x59, 0x02, 0x24, 0x01, 0x90, 0x24});
    r->read_dtc(0xfa10, {0x4,0x19,0x02, 0xff});
    /* First frame */
    c1->capture();
    testFrames(result_frame, *c1);

    result_frame = createFrame(0x10fa, {0x21, 0x01, 0x96, 0x24});
    c1->capture();
    testFrames(result_frame, *c1);
}

TEST_F(ReadDtcTest, SubfunctionNotSupported)
{
    struct can_frame result_frame = createFrame(0x12fa, {0x03, 0x7F, 0x19, 0x12});

    r->read_dtc(0xfa12, {0x4,0x19,0x03, 0xff});
    c1->capture();
    testFrames(result_frame, *c1);
}

TEST_F(ReadDtcTest, UnableToOpenFile1)
{
    std::string dtc_file_path = std::string(PROJECT_PATH) + "/src/ecu_simulation/WrongPath/dtcs.txt";
    r = new ReadDTC(*logger, dtc_file_path,socket2_);
    struct can_frame result_frame = createFrame(0x12fa, {0x03, 0x7F, 0x19, 0x94});

    r->read_dtc(0xfa12, {0x4,0x19,0x01, 0xff});
    c1->capture();
    testFrames(result_frame, *c1);
}

TEST_F(ReadDtcTest, UnableToOpenFile2)
{
    std::string dtc_file_path = std::string(PROJECT_PATH) + "/src/ecu_simulation/WrongPath/dtcs.txt";
    r = new ReadDTC(*logger, dtc_file_path,socket2_);
    struct can_frame result_frame = createFrame(0x12fa, {0x03, 0x7F, 0x19, 0x94});

    r->read_dtc(0xfa12, {0x4,0x19,0x02, 0xff});
    c1->capture();
    testFrames(result_frame, *c1);
}

TEST_F(ReadDtcTest, NoDtcFound)
{
    std::string dtc_file_path = std::string(PROJECT_PATH) + "/src/uds/read_dtc_information/dtcs.txt";
    r = new ReadDTC(*logger, dtc_file_path, socket2_);
    struct can_frame result_frame = createFrame(0x12fa, {0x03, 0x59, 0x02, 0x00});

    r->read_dtc(0xfa12, {0x4,0x19,0x02, 0xff});
    c1->capture();
    testFrames(result_frame, *c1);
}

int main(int argc, char* argv[])
{
    socket_ = createSocket();
    socket2_ = createSocket();
    testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}