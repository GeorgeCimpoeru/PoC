#include "../include/GenerateFrame.h"
#include <gtest/gtest.h>
#include <fcntl.h>
#include <stdexcept>

/* Test class for GenerateFrame, which provides setup and teardown for CAN socket */
class GenerateFrameTest : public testing::Test {
protected:
	/* Set up a virtual CAN socket before each test */
    void SetUp() override {
        s = createSocket();
    }

	/* Close the CAN socket after each test */
    void TearDown() override {
        close(s);
    }

	/* Create and bind a CAN socket to the virtual CAN interface */
    int createSocket() {
        int s = socket(PF_CAN, SOCK_RAW, CAN_RAW);
        if (s < 0) {
            std::cerr << "Error trying to create the socket\n";
            return -1;
        }

        struct sockaddr_can addr;
        struct ifreq ifr;

        strcpy(ifr.ifr_name, "vcan0");
        ioctl(s, SIOCGIFINDEX, &ifr);

        addr.can_family = AF_CAN;
        addr.can_ifindex = ifr.ifr_ifindex;

        if (bind(s, (struct sockaddr*)&addr, sizeof(addr)) < 0) {
            std::cerr << "Error binding\n";
            close(s);
            return -1;
        }

        return s;
    }

    int s;
};

/* Test the construction of a data frame */
TEST_F(GenerateFrameTest, ConstructDataFrame) {
    const uint32_t can_id = 0x123;
    const uint8_t data[] = {0x01, 0x02, 0x03};
    const uint8_t dlc = sizeof(data) / sizeof(data[0]);

    GenerateFrame frame(FrameType::DATA_FRAME, can_id, data, dlc);

    EXPECT_EQ(frame.frame.can_id, can_id);
    EXPECT_EQ(frame.frame.can_dlc, dlc);
    for (int i = 0; i < dlc; ++i) {
        EXPECT_EQ(frame.frame.data[i], data[i]);
    }
}

/* Test the construction of a remote frame */
TEST_F(GenerateFrameTest, ConstructRemoteFrame) {
    const uint32_t can_id = 0x123;
    const uint8_t dlc = 3; 

    GenerateFrame frame(FrameType::REMOTE_FRAME, can_id, nullptr, dlc);

    EXPECT_EQ(frame.frame.can_id, can_id | CAN_RTR_FLAG);
    EXPECT_EQ(frame.frame.can_dlc, dlc);
}

/* Test the construction of an error frame */
TEST_F(GenerateFrameTest, ConstructErrorFrame) {
    const uint32_t can_id = 0x123;

    GenerateFrame frame(FrameType::ERROR_FRAME, can_id, nullptr, 0);

    EXPECT_EQ(frame.frame.can_id, CAN_ERR_FLAG);
    EXPECT_EQ(frame.frame.can_dlc, 0);
}

/* Test the construction of an overload frame */
TEST_F(GenerateFrameTest, ConstructOverloadFrame) {
    const uint32_t can_id = 0x123;

    GenerateFrame frame(FrameType::OVERLOAD_FRAME, can_id, nullptr, 0);

    EXPECT_EQ(frame.frame.can_id, CAN_ERR_FLAG);
    EXPECT_EQ(frame.frame.can_dlc, 0);
}

/* Test that an invalid frame type throws an exception */
TEST_F(GenerateFrameTest, InvalidFrameType) {
    const uint32_t can_id = 0x123;
    const uint8_t data[] = {0x01, 0x02, 0x03};
    const uint8_t dlc = sizeof(data) / sizeof(data[0]);

    EXPECT_THROW(GenerateFrame frame(static_cast<FrameType>(-1), can_id, data, dlc), std::invalid_argument);
}

/* Test the retrieval of a CAN frame */
TEST_F(GenerateFrameTest, GetFrame) {
    const uint32_t can_id = 0x123;
    const uint8_t data[] = {0x01, 0x02, 0x03};
    const uint8_t dlc = sizeof(data) / sizeof(data[0]);

    GenerateFrame frame(FrameType::DATA_FRAME, can_id, data, dlc);
    struct can_frame retrieved_frame = frame.getFrame();

    EXPECT_EQ(retrieved_frame.can_id, can_id);
    EXPECT_EQ(retrieved_frame.can_dlc, dlc);
    for (int i = 0; i < dlc; ++i) {
        EXPECT_EQ(retrieved_frame.data[i], data[i]);
    }
}

/* Test successful sending of a data frame */
TEST_F(GenerateFrameTest, SendDataFrame) {
    const uint32_t can_id = 0x123;
    const uint8_t data[] = {0x01, 0x02, 0x03};
    const uint8_t dlc = sizeof(data) / sizeof(data[0]);

    GenerateFrame frame(FrameType::DATA_FRAME, can_id, data, dlc);

    EXPECT_EQ(frame.SendFrame("vcan0", s), 0);
}

/* Test successful sending of a remote frame */
TEST_F(GenerateFrameTest, SendRemoteFrame) {
    const uint32_t can_id = 0x123;
	/* Remote frame has no data*/
    const uint8_t dlc = 0; 

    GenerateFrame frame(FrameType::REMOTE_FRAME, can_id, nullptr, dlc);

    EXPECT_EQ(frame.SendFrame("vcan0", s), 0);
}

/* Test successful sending of an error frame */
TEST_F(GenerateFrameTest, SendErrorFrame) {
    const uint32_t can_id = 0x123;
	/* Error frame has no data*/
    const uint8_t dlc = 0; 

    GenerateFrame frame(FrameType::ERROR_FRAME, can_id, nullptr, dlc);

    EXPECT_EQ(frame.SendFrame("vcan0", s), 0);
}

/* Test successful sending of an overload frame */
TEST_F(GenerateFrameTest, SendOverloadFrame) {
    const uint32_t can_id = 0x123;
	/* Overload frame has no data */
    const uint8_t dlc = 0; 

    GenerateFrame frame(FrameType::OVERLOAD_FRAME, can_id, nullptr, dlc);

    EXPECT_EQ(frame.SendFrame("vcan0", s), 0);
}

/* Test failure to send a frame due to an invalid socket descriptor */
TEST_F(GenerateFrameTest, SendFrameFailure) {
    const uint32_t can_id = 0x123;
    const uint8_t data[] = {0x01, 0x02, 0x03};
    const uint8_t dlc = sizeof(data) / sizeof(data[0]);

    GenerateFrame frame(FrameType::DATA_FRAME, can_id, data, dlc);

    /* Invalid socket descriptor */
    int invalid_socket = -1;
    EXPECT_EQ(frame.SendFrame("vcan0", invalid_socket), -1);
}

int main(int argc, char* argv[]) {
    testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
