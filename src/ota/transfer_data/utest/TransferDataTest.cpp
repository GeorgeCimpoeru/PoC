#include <iostream>
#include <vector>
#include <cstdint>
#include <string>
#include <gtest/gtest.h>
#include <linux/can.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <net/if.h>
#include "../include/TransferData.h"
#include "../../../utils/include/Logger.h"

int socket_;
int socket2_;
const int id = 0x1011;


class CaptureFrame
{
    public:
        struct can_frame frame;
        void capture()
        {
            int nbytes = read(socket_, &frame, sizeof(struct can_frame));
            if(nbytes < 0)
            {
                return;
            }
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


class TransferDataTest : public ::testing::Test {
protected:
    canid_t frame_id;
    std::vector<uint8_t> frame_data;
    Logger mockLogger;
    TransferData* transfer_data;
    CaptureFrame* captured_frame;
    TransferDataTest()
    {
        transfer_data = new TransferData(socket_, mockLogger);
        captured_frame = new CaptureFrame();
    }
    ~TransferDataTest()
    {
        delete captured_frame;
    }
};


/* Test for Incorrect Message Length */
TEST_F(TransferDataTest, IncorrectMessageLengthTest) {

    std::vector<uint8_t> invalid_frame_data = {0x02, 0x36};
    std::vector<uint8_t> expected_frame_data = {0x03, 0x7F, 0x36, 0x13};

    transfer_data->transferData(0x1011, invalid_frame_data);
    captured_frame->capture();
    for (int i = 0; i < captured_frame->frame.can_dlc; ++i) {
        EXPECT_EQ(expected_frame_data[i], captured_frame->frame.data[i]);
    }
}

/* Test for Wrong block sequence number */
TEST_F(TransferDataTest, WrongBlockSequenceNumberTest) {

    std::vector<uint8_t> invalid_frame_data = {0x02, 0x36, 0x03, 0x02};
    std::vector<uint8_t> expected_frame_data = {0x03, 0x7F, 0x36, 0x73};

    transfer_data->transferData(0x1011, invalid_frame_data);
    captured_frame->capture();
    for (int i = 0; i < captured_frame->frame.can_dlc; ++i) {
        EXPECT_EQ(expected_frame_data[i], captured_frame->frame.data[i]);
    }
}

/* Test for PositiveResponse */
TEST_F(TransferDataTest, PositiveResponseTest) {

    std::vector<uint8_t> frame_data = {0x02, 0x36, 0x01, 0x02, 0x33};
    std::vector<uint8_t> expected_frame_data = {0x02, 0x76, 0x01};

    transfer_data->transferData(0x1011, frame_data);
    captured_frame->capture();
    for (int i = 0; i < captured_frame->frame.can_dlc; ++i) {
        EXPECT_EQ(expected_frame_data[i], captured_frame->frame.data[i]);
    }
}

int main(int argc, char **argv) {
    socket_ = createSocket();
    socket2_ = createSocket();
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}