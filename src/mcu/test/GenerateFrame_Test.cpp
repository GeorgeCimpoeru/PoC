#include "../include/GenerateFrame.h"
#include <gtest/gtest.h>
#include <fcntl.h>
#include <stdexcept>

class CANFrameTest : public testing::Test {
protected:
    void SetUp() override {
        s = createSocket();
    }

    void TearDown() override {
        close(s);
    }

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

TEST_F(CANFrameTest, ConstructDataFrame) {
    const uint32_t can_id = 0x123;
    const uint8_t data[] = {0x01, 0x02, 0x03};
    const uint8_t dlc = sizeof(data) / sizeof(data[0]);

    CANFrame frame(FrameType::DATA_FRAME, can_id, data, dlc);

    EXPECT_EQ(frame.frame.can_id, can_id);
    EXPECT_EQ(frame.frame.can_dlc, dlc);
    for (int i = 0; i < dlc; ++i) {
        EXPECT_EQ(frame.frame.data[i], data[i]);
    }
}

TEST_F(CANFrameTest, ConstructRemoteFrame) {
    const uint32_t can_id = 0x123;
    const uint8_t dlc = 3; 

    CANFrame frame(FrameType::REMOTE_FRAME, can_id, nullptr, dlc);

    EXPECT_EQ(frame.frame.can_id, can_id | CAN_RTR_FLAG);
    EXPECT_EQ(frame.frame.can_dlc, dlc);
}

TEST_F(CANFrameTest, ConstructErrorFrame) {
    const uint32_t can_id = 0x123;

    CANFrame frame(FrameType::ERROR_FRAME, can_id, nullptr, 0);

    EXPECT_EQ(frame.frame.can_id, CAN_ERR_FLAG);
    EXPECT_EQ(frame.frame.can_dlc, 0);
}

TEST_F(CANFrameTest, ConstructOverloadFrame) {
    const uint32_t can_id = 0x123;

    CANFrame frame(FrameType::OVERLOAD_FRAME, can_id, nullptr, 0);

    EXPECT_EQ(frame.frame.can_id, CAN_ERR_FLAG);
    EXPECT_EQ(frame.frame.can_dlc, 0);
}

TEST_F(CANFrameTest, InvalidFrameType) {
    const uint32_t can_id = 0x123;
    const uint8_t data[] = {0x01, 0x02, 0x03};
    const uint8_t dlc = sizeof(data) / sizeof(data[0]);

    EXPECT_THROW(CANFrame frame(static_cast<FrameType>(-1), can_id, data, dlc), std::invalid_argument);
}

TEST_F(CANFrameTest, GetFrame) {
    const uint32_t can_id = 0x123;
    const uint8_t data[] = {0x01, 0x02, 0x03};
    const uint8_t dlc = sizeof(data) / sizeof(data[0]);

    CANFrame frame(FrameType::DATA_FRAME, can_id, data, dlc);
    struct can_frame retrieved_frame = frame.getFrame();

    EXPECT_EQ(retrieved_frame.can_id, can_id);
    EXPECT_EQ(retrieved_frame.can_dlc, dlc);
    for (int i = 0; i < dlc; ++i) {
        EXPECT_EQ(retrieved_frame.data[i], data[i]);
    }
}

TEST_F(CANFrameTest, SendDataFrame) {
    const uint32_t can_id = 0x123;
    const uint8_t data[] = {0x01, 0x02, 0x03};
    const uint8_t dlc = sizeof(data) / sizeof(data[0]);

    CANFrame frame(FrameType::DATA_FRAME, can_id, data, dlc);

    EXPECT_EQ(frame.SendFrame("vcan0", s), 0);
}

TEST_F(CANFrameTest, SendRemoteFrame) {
    const uint32_t can_id = 0x123;
    const uint8_t dlc = 0; // Remote frame has no data

    CANFrame frame(FrameType::REMOTE_FRAME, can_id, nullptr, dlc);

    EXPECT_EQ(frame.SendFrame("vcan0", s), 0);
}

TEST_F(CANFrameTest, SendErrorFrame) {
    const uint32_t can_id = 0x123;
    const uint8_t dlc = 0; // Error frame has no data

    CANFrame frame(FrameType::ERROR_FRAME, can_id, nullptr, dlc);

    EXPECT_EQ(frame.SendFrame("vcan0", s), 0);
}

TEST_F(CANFrameTest, SendOverloadFrame) {
    const uint32_t can_id = 0x123;
    const uint8_t dlc = 0; // Overload frame has no data

    CANFrame frame(FrameType::OVERLOAD_FRAME, can_id, nullptr, dlc);

    EXPECT_EQ(frame.SendFrame("vcan0", s), 0);
}

TEST_F(CANFrameTest, SendFrameFailure) {
    const uint32_t can_id = 0x123;
    const uint8_t data[] = {0x01, 0x02, 0x03};
    const uint8_t dlc = sizeof(data) / sizeof(data[0]);

    CANFrame frame(FrameType::DATA_FRAME, can_id, data, dlc);

    // Invalid socket descriptor
    int invalid_socket = -1;
    EXPECT_EQ(frame.SendFrame("vcan0", invalid_socket), -1);
}

int main(int argc, char* argv[]) {
    testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
