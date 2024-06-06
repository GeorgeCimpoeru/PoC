#include <gtest/gtest.h>
#include "../include/GenerateFrame.h"
#include <sys/socket.h>
#include <linux/can.h>
#include <linux/can/raw.h>
#include <net/if.h>
#include <unistd.h>

/* Mock class for testing */
class MockGenerateFrame : public GenerateFrame {
public:
    using GenerateFrame::GenerateFrame;
    using GenerateFrame::CreateFrame;
};

/* Test fixture for GenerateFrame tests */
class GenerateFrameTest : public ::testing::Test {
protected:
    int mockSocket;
    MockGenerateFrame* generateFrame;

    virtual void SetUp() {
        // Create a mock socket for testing
        mockSocket = socket(PF_CAN, SOCK_RAW, CAN_RAW);
        ASSERT_TRUE(mockSocket >= 0);

        // Bind the socket to the CAN interface
        struct sockaddr_can addr;
        struct ifreq ifr;

        strcpy(ifr.ifr_name, "vcan0");
        ioctl(mockSocket, SIOCGIFINDEX, &ifr);

        addr.can_family = AF_CAN;
        addr.can_ifindex = ifr.ifr_ifindex;

        ASSERT_EQ(bind(mockSocket, (struct sockaddr *)&addr, sizeof(addr)), 0);

        // Initialize the GenerateFrame object with the mock socket
        generateFrame = new MockGenerateFrame(mockSocket);
    }

    virtual void TearDown() {
        delete generateFrame;
        close(mockSocket);
    }
};

/* Test creating a data frame */
TEST_F(GenerateFrameTest, CreateDataFrame) {
    std::vector<int> data = {0x01, 0x02, 0x03, 0x04};
    can_frame frame = generateFrame->CreateFrame(0x123, data, DATA_FRAME);

    EXPECT_EQ(frame.can_id, 0x123);
    EXPECT_EQ(frame.can_dlc, 4);
    EXPECT_EQ(memcmp(frame.data, data.data(), data.size()), 0);
}

/* Test creating a remote frame */
TEST_F(GenerateFrameTest, CreateRemoteFrame) {
    std::vector<int> data = {0x01, 0x02, 0x03, 0x04};
    can_frame frame = generateFrame->CreateFrame(0x123, data, REMOTE_FRAME);

    EXPECT_EQ(frame.can_id, 0x123 | CAN_RTR_FLAG);
    EXPECT_EQ(frame.can_dlc, 4);
    EXPECT_EQ(memcmp(frame.data, data.data(), data.size()), 0);
}

/* Test creating an error frame */
TEST_F(GenerateFrameTest, CreateErrorFrame) {
    std::vector<int> data = {};
    can_frame frame = generateFrame->CreateFrame(0x123, data, ERROR_FRAME);

    EXPECT_EQ(frame.can_id, CAN_ERR_FLAG);
    EXPECT_EQ(frame.can_dlc, 0);
}

/* Test sending a data frame */
TEST_F(GenerateFrameTest, SendDataFrame) {
    std::vector<int> data = {0x01, 0x02, 0x03, 0x04};
    int result = generateFrame->SendFrame(0x123, data, DATA_FRAME);

    EXPECT_EQ(result, 0);
}

/* Test sending a remote frame */
TEST_F(GenerateFrameTest, SendRemoteFrame) {
    std::vector<int> data = {0x01, 0x02, 0x03, 0x04};
    int result = generateFrame->SendFrame(0x123, data, REMOTE_FRAME);

    EXPECT_EQ(result, 0);
}

/* Test sending an error frame */
TEST_F(GenerateFrameTest, SendErrorFrame) {
    std::vector<int> data = {};
    int result = generateFrame->SendFrame(0x123, data, ERROR_FRAME);

    EXPECT_EQ(result, 0);
}

int main(int argc, char **argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
