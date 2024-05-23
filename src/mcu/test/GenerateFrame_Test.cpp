#include "../include/GenerateFrame.h"
#include <gtest/gtest.h>
#include <fcntl.h>

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

// TEST_F(CANFrameTest, SendDataFrame) {
//     const uint32_t can_id = 0x123;
//     const uint8_t data[] = {0x01, 0x02, 0x03};
//     const uint8_t dlc = sizeof(data) / sizeof(data[0]);

//     CANFrame frame(FrameType::DATA_FRAME, can_id, data, dlc);

//     EXPECT_EQ(frame.SendFrame("vcan0", s), 0);
//     // Need to wait for the frame to be sent before checking
//     usleep(1000); // Wait for 1ms

//     struct can_frame received_frame;
//     int nbytes = read(s, &received_frame, sizeof(struct can_frame));

//     EXPECT_EQ(nbytes, sizeof(struct can_frame));
//     EXPECT_EQ(received_frame.can_id, can_id);
//     EXPECT_EQ(received_frame.can_dlc, dlc);
//     for (int i = 0; i < dlc; ++i) {
//         EXPECT_EQ(received_frame.data[i], data[i]);
//     }
// }

TEST_F(CANFrameTest, SendFrameSuccess) {
    const uint32_t can_id = 0x123;
    const uint8_t data[] = {0x01, 0x02, 0x03};
    const uint8_t dlc = sizeof(data) / sizeof(data[0]);

    CANFrame frame(FrameType::DATA_FRAME, can_id, data, dlc);

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