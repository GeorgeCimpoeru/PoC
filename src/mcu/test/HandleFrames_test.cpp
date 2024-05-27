#include "gtest/gtest.h"
#include "../include/HandleFrames.h"
#include <linux/can.h>

class HandleFramesTest : public ::testing::Test{
    public:
    HandleFramesTest(){}
    protected:
    HandleFrames handler;
    struct can_frame testFrame;
};

TEST_F(HandleFramesTest, DiagnosticSessionControlTest){
    /* Set an arbitrary CAN ID */
    testFrame.can_id = 0;
    /* Set the Data Length Code to at least 2 to access data[1] */
    testFrame.can_dlc = 2;
    testFrame.data[1] = 0x10;
    testing::internal::CaptureStdout();
    handler.handleFrame(testFrame);
    std::string output = testing::internal::GetCapturedStdout();
    EXPECT_EQ(output, "DiagnosticSessionControl called.\n");
}

TEST_F(HandleFramesTest, OtaRequestDownloadTest){
    /* Set an arbitrary CAN ID */
    testFrame.can_id = 0;
    /* Set the Data Length Code to at least 2 to access data[1] */
    testFrame.can_dlc = 2;
    testFrame.data[1] = 0x34;
    testing::internal::CaptureStdout();
    handler.handleFrame(testFrame);
    std::string output = testing::internal::GetCapturedStdout();
    EXPECT_EQ(output, "Request download called.\n");
}

TEST_F(HandleFramesTest, UnknownFrameTest){
    /* Set an arbitrary CAN ID */
    testFrame.can_id = 0;
    /* Set the Data Length Code to at least 2 to access data[1] */
    testFrame.can_dlc = 2;
    testFrame.data[1] = 0x99;
    testing::internal::CaptureStderr();
    handler.handleFrame(testFrame);
    std::string output = testing::internal::GetCapturedStderr();
    EXPECT_EQ(output, "Unknown service.\n");
 }

int main(int argc, char **argv)
{
testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}