#include <iostream>
#include <vector>
#include <cstdint>
#include <string>
#include <gtest/gtest.h>
#include <linux/can.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <net/if.h>
#include "../include/EcuReset.h"
#include "../../../utils/include/Logger.h"
#include "../../../ecu_simulation/BatteryModule/include/BatteryModule.h"
#include "../../../mcu/include/MCUModule.h"

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


class EcuResetTest : public ::testing::Test {
protected:
    canid_t frame_id;
    std::vector<uint8_t> frame_data;
    Logger* mockLogger;
    EcuReset* ecu_reset;
    CaptureFrame* captured_frame;
    EcuResetTest()
    {
        batteryModuleLogger = new Logger;
        MCULogger = new Logger;
        mockLogger = new Logger;
        battery = new BatteryModule(0x00, 0x11);
        MCU::mcu = new MCU::MCUModule(0x01);
        ecu_reset = new EcuReset(0x1011, 0x11, socket_, *mockLogger);
        captured_frame = new CaptureFrame();
    }
    ~EcuResetTest()
    {
        delete batteryModuleLogger;
        delete MCULogger;
        delete captured_frame;
        delete mockLogger;
        delete ecu_reset;
        MCU::mcu = nullptr;
        battery = nullptr;
    }
};

/* Test for HardReset */
TEST_F(EcuResetTest, HardResetTestEcu) {

    // std::vector<uint8_t> invalid_frame_data = {0x02, 0x36};
    // std::vector<uint8_t> expected_frame_data = {0x03, 0x7F, 0x36, 0x13};
    ecu_reset = new EcuReset(0x1011, 0x01, socket_, *mockLogger);
    testing::internal::CaptureStdout();
    ecu_reset->ecuResetRequest();

    std::string output = testing::internal::GetCapturedStdout();
    // delete ecu_reset;
    EXPECT_NE(output.find("Reset Mode: Hard Reset"), std::string::npos);
    // captured_frame->capture();
    // for (int i = 0; i < captured_frame->frame.can_dlc; ++i) {
    //     EXPECT_EQ(expected_frame_data[i], captured_frame->frame.data[i]);
    // }
}

TEST_F(EcuResetTest, HardResetTestMcu) {

    // std::vector<uint8_t> invalid_frame_data = {0x02, 0x36};
    // std::vector<uint8_t> expected_frame_data = {0x03, 0x7F, 0x36, 0x13};
    ecu_reset = new EcuReset(0xFA10, 0x01, socket_, *mockLogger);
    testing::internal::CaptureStdout();
    ecu_reset->ecuResetRequest();

    std::string output = testing::internal::GetCapturedStdout();
    // delete ecu_reset;
    EXPECT_NE(output.find("Reset Mode: Hard Reset"), std::string::npos);
    // captured_frame->capture();
    // for (int i = 0; i < captured_frame->frame.can_dlc; ++i) {
    //     EXPECT_EQ(expected_frame_data[i], captured_frame->frame.data[i]);
    // }
}

/* Test for KeyOffReset */
TEST_F(EcuResetTest, KeyOffResetTestEcu) {

    // std::vector<uint8_t> invalid_frame_data = {0x02, 0x36};
    // std::vector<uint8_t> expected_frame_data = {0x03, 0x7F, 0x36, 0x13};
    ecu_reset = new EcuReset(0x1011, 0x02, socket_, *mockLogger);
    testing::internal::CaptureStdout();
    ecu_reset->ecuResetRequest();

    std::string output = testing::internal::GetCapturedStdout();
    EXPECT_NE(output.find("Reset Mode: Key Off Reset"), std::string::npos);
    // captured_frame->capture();
    // for (int i = 0; i < captured_frame->frame.can_dlc; ++i) {
    //     EXPECT_EQ(expected_frame_data[i], captured_frame->frame.data[i]);
    // }
}

/* Test for KeyOffReset */
TEST_F(EcuResetTest, KeyOffResetTestMcu) {

    // std::vector<uint8_t> invalid_frame_data = {0x02, 0x36};
    // std::vector<uint8_t> expected_frame_data = {0x03, 0x7F, 0x36, 0x13};
    ecu_reset = new EcuReset(0xFA10, 0x02, socket_, *mockLogger);
    testing::internal::CaptureStdout();
    ecu_reset->ecuResetRequest();

    std::string output = testing::internal::GetCapturedStdout();
    EXPECT_NE(output.find("Reset Mode: Key Off Reset"), std::string::npos);
    // captured_frame->capture();
    // for (int i = 0; i < captured_frame->frame.can_dlc; ++i) {
    //     EXPECT_EQ(expected_frame_data[i], captured_frame->frame.data[i]);
    // }
}

/* Test for PositiveResponse */
TEST_F(EcuResetTest, PositiveResponseTest) {

    // std::vector<uint8_t> frame_data = {0x02, 0x36, 0x01, 0x02, 0x33};
    std::vector<uint8_t> expected_frame_data = {0x00, 0xFF, 0x11, 0x03};

    ecu_reset->ecuResetRequest();
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