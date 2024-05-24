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
    testFrame.data[1] = 0x10;
    testing::internal::CaptureStdout();
    handler.HandleFrame(testFrame);
    std::string output = testing::internal::GetCapturedStdout();
    EXPECT_EQ(output, "DiagnosticSessionControl called.\n");
}
TEST_F(HandleFramesTest, DiagnosticSessionControlResponseTest){
    testFrame.data[1] = 0x50;
    testing::internal::CaptureStdout();
    handler.HandleFrame(testFrame);
    std::string output = testing::internal::GetCapturedStdout();
    EXPECT_EQ(output, "DiagnosticSessionControl processed.\n");
}

TEST_F(HandleFramesTest, OtaRequestDownloadTest){
    testFrame.data[1] = 0x34;
    testing::internal::CaptureStdout();
    handler.HandleFrame(testFrame);
    std::string output = testing::internal::GetCapturedStdout();
    EXPECT_EQ(output, "Request download called.\n");
}
TEST_F(HandleFramesTest, OtaRequestDownloadResponseTest){
    testFrame.data[1] = 0x74;
    testing::internal::CaptureStdout();
    handler.HandleFrame(testFrame);
    std::string output = testing::internal::GetCapturedStdout();
    EXPECT_EQ(output, "Request download processed.\n");
}


TEST_F(HandleFramesTest, UnknownFrameTest){
    testFrame.data[1] = 0x99;
    testing::internal::CaptureStderr();
    handler.HandleFrame(testFrame);
    std::string output = testing::internal::GetCapturedStderr();
    EXPECT_EQ(output, "Unknown service.\n");
}


int main(int argc,char**argv)
{
testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}