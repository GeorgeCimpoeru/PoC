/**
 * @file ClearDtcTest.cpp
 * @author Mujdei Ruben
 * @brief 
 * @version 0.1
 * @date 2024-07-24
 * 
 * @copyright Copyright (c) 2024
 * 
 * PS: Test work with the next data in the file './dtcs.txt':
 *          P0A9B-17 24
 *          P0805-11 2F
 *          C0805-11 2F
 *          B1234-00 00
 *          P0B12-01 10
 *          C0B12-01 10
 * The file is created automatically!!
 */
#include "../include/ClearDtc.h"

#include <cstring>
#include <string>
#include <thread>

#include <fcntl.h>
#include <sys/ioctl.h>
#include <gtest/gtest.h>
#include <net/if.h>

int socket_;
int socket2_;
/* Const */
const int id = 0x3412;
std::string path_to_dtc = "./dtcs.txt";
Logger* logger = new Logger("log_test_read_dtc","./log_test_read_dtc.log");

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
/* Create object for all tests */
struct ReadDtcTest : testing::Test
{
    ClearDtc* clear;
    CaptureFrame* c1;
    ReadDtcTest()
    {
        clear = new ClearDtc(path_to_dtc,*logger,socket2_);
        c1 = new CaptureFrame();
    }
    ~ReadDtcTest()
    {
        delete clear;
        delete c1;
    }
};

TEST_F(ReadDtcTest, TestsSendFrame)
{
    struct can_frame result_frame = createFrame({0x01, 0x54});

    clear->clearDtc(0x1234, {0x4,0x14,0x01,0x0A,0xAA});
    c1->capture();
    testFrames(result_frame, *c1);
}

TEST_F(ReadDtcTest, TestClearDtc)
{
    clear->clearDtc(0x1234, {0x4,0x14,0x01,0x0A,0xAA});
    std::ifstream file_dtc;
    file_dtc.open(path_to_dtc);
    std::string line;
    int k=0;
    while (std::getline(file_dtc,line))
    {
        k++;
    }
    EXPECT_EQ(k,4);
}

TEST_F(ReadDtcTest, TestClearDtc2)
{
    clear->clearDtc(0x1234, {0x4,0x14,0xFF,0xFF,0xFF});
    std::ifstream file_dtc;
    file_dtc.open(path_to_dtc);
    std::string line;
    int k=0;
    while (std::getline(file_dtc,line))
    {
        k++;
    }
    EXPECT_EQ(k,0);
}

TEST_F(ReadDtcTest, NRC0x13Test)
{
    testing::internal::CaptureStdout();
    clear->clearDtc(0x1234, {0x4,0x14,0x01,0x0A,0xAA,0xBB});
    std::string output = testing::internal::GetCapturedStdout();
    EXPECT_NE(output.find("Incorrect message length or invalid format"), std::string::npos);
}

TEST_F(ReadDtcTest, NRC0x31Test)
{
    testing::internal::CaptureStdout();
    clear->clearDtc(0x1234, {0x4,0x14,0x01,0x0A,0xAB});
    std::string output = testing::internal::GetCapturedStdout();
    EXPECT_NE(output.find("RequestOutOfRange NRC:Specified Group of DTC parameter is not supported"), std::string::npos);
}

int main(int argc, char* argv[])
{
    std::ofstream outfile(path_to_dtc);
    outfile << "P0A9B-17 24"<<std::endl;
    outfile << "P0805-11 2F"<<std::endl;
    outfile << "C0805-11 2F"<<std::endl;
    outfile << "B1234-00 00"<<std::endl;
    outfile << "P0B12-01 10"<<std::endl;
    outfile << "C0B12-01 10"<<std::endl;
    socket_ = createSocket();
    socket2_ = createSocket();
    testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
    outfile.close();
    const char * p = path_to_dtc.c_str();
    remove(p);
}