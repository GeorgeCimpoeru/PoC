#include <gtest/gtest.h>
#include <unistd.h>
#include <linux/can.h>
#include <sys/socket.h>
#include <thread>
#include <sstream>
#include "../include/ReceiveFrames.h"

class MockReceiveFrames : public ReceiveFrames {
public:
    MockReceiveFrames(int socket) : ReceiveFrames(socket) {}
};

class ReceiveFramesTest : public ::testing::Test {
protected:
    int mockSocket;
    int mockSocketPair[2];
    MockReceiveFrames* receiveFrames;

    virtual void SetUp() {
        /* Create a mock socket pair for testing */
        socketpair(AF_UNIX, SOCK_STREAM, 0, mockSocketPair);
        mockSocket = mockSocketPair[0];
        receiveFrames = new MockReceiveFrames(mockSocket);
    }

    virtual void TearDown() {
        delete receiveFrames;
        close(mockSocketPair[0]);
        close(mockSocketPair[1]);
    }
};

TEST_F(ReceiveFramesTest, TestReceiveFramesFromCANBus_ReadError) {
    /* Close socket to simulate read error */
    close(mockSocket);
    int result = receiveFrames->receiveFramesFromCANBus();
    EXPECT_EQ(result, 1);
}

TEST_F(ReceiveFramesTest, TestReceiveFramesFromCANBus_IncompleteFrameRead) {
    std::thread readerThread([this] {
        usleep(100);
        struct can_frame frame;
        frame.can_id = 0x123;
        frame.can_dlc = 4;
        write(mockSocketPair[1], &frame, sizeof(frame) - 1);
    });

    int result = receiveFrames->receiveFramesFromCANBus();
    EXPECT_EQ(result, 1);
    readerThread.join();
}

int main(int argc, char **argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}