#include "../include/RoutineControl.h"

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

/* Class to capture the frame sin the can-bus */
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
    /* Set the O_NONBLOCK flag to make the socket non-blocking */
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
struct RoutineControlTest : testing::Test
{
    RoutineControl* routine_control;
    CaptureFrame* capture_frame;
    Logger logger;
    RoutineControlTest()
    {
        routine_control = new RoutineControl(socket2_,logger);
        capture_frame = new CaptureFrame();
    }
    ~RoutineControlTest()
    {
        delete routine_control;
        delete capture_frame;
    }
};

TEST_F(RoutineControlTest, IncorrectMessageLength)
{
    std::vector<uint8_t> invalid_frame_data = {0x03, 0x31, 0x01, 0x02};
    std::vector<uint8_t> expected_frame_data = {0x03, 0x7F, 0x31, 0x13};
    routine_control->routineControl(0x1110, invalid_frame_data);
    capture_frame->capture();
    for (int i = 0; i < capture_frame->frame.can_dlc; ++i) {
        EXPECT_EQ(expected_frame_data[i], capture_frame->frame.data[i]);
    }
}

TEST_F(RoutineControlTest, SubFunctionNotSupported)
{
    std::vector<uint8_t> invalid_frame_data = {0x03, 0x31, 0x00, 0x01, 0x03, 0x03};
    std::vector<uint8_t> expected_frame_data = {0x03, 0x7F, 0x31, 0x12};
    routine_control->routineControl(0x1110, invalid_frame_data);
    capture_frame->capture();
    for (int i = 0; i < capture_frame->frame.can_dlc; ++i) {
        EXPECT_EQ(expected_frame_data[i], capture_frame->frame.data[i]);
    }
}

TEST_F(RoutineControlTest, RequestOutOfRange)
{
    std::vector<uint8_t> invalid_frame_data = {0x03, 0x31, 0x01, 0x00, 0x00, 0x03};
    std::vector<uint8_t> expected_frame_data = {0x03, 0x7F, 0x31, 0x31};
    routine_control->routineControl(0x1110, invalid_frame_data);
    capture_frame->capture();
    for (int i = 0; i < capture_frame->frame.can_dlc; ++i) {
        EXPECT_EQ(expected_frame_data[i], capture_frame->frame.data[i]);
    }
}

TEST_F(RoutineControlTest, PositiveResponseCase1)
{
    std::vector<uint8_t> invalid_frame_data = {0x03, 0x31, 0x01, 0x01, 0x01, 0x03};
    std::vector<uint8_t> expected_frame_data = {0x04, 0x71, 0x01, 0x01, 0x01};
    routine_control->routineControl(0x1110, invalid_frame_data);
    capture_frame->capture();
    for (int i = 0; i < capture_frame->frame.can_dlc; ++i) {
        EXPECT_EQ(expected_frame_data[i], capture_frame->frame.data[i]);
    }
}

TEST_F(RoutineControlTest, PositiveResponseCase2)
{
    std::vector<uint8_t> invalid_frame_data = {0x03, 0x31, 0x01, 0x02, 0x01, 0x03};
    std::vector<uint8_t> expected_frame_data = {0x04, 0x71, 0x01, 0x02, 0x01};
    routine_control->routineControl(0x1110, invalid_frame_data);
    capture_frame->capture();
    for (int i = 0; i < capture_frame->frame.can_dlc; ++i) {
        EXPECT_EQ(expected_frame_data[i], capture_frame->frame.data[i]);
    }
}

TEST_F(RoutineControlTest, PositiveResponseDefaultCase)
{
    std::vector<uint8_t> invalid_frame_data = {0x03, 0x31, 0x01, 0x03, 0x01, 0x03};
    std::vector<uint8_t> expected_frame_data = {0x04, 0x71, 0x01, 0x03, 0x01};
    routine_control->routineControl(0x1110, invalid_frame_data);
    capture_frame->capture();
    for (int i = 0; i < capture_frame->frame.can_dlc; ++i) {
        EXPECT_EQ(expected_frame_data[i], capture_frame->frame.data[i]);
    }
}


int main(int argc, char* argv[])
{
    socket_ = createSocket();
    socket2_ = createSocket();
    testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}