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

/* Test for processFrameData with a valid DiagnosticSessionControl frame */
TEST_F(HandleFramesTest, DiagnosticSessionControlValTest){
    /* Set an arbitrary CAN ID */
    testFrame.can_id= 0;
    /* Set the Data Length Code to at least 2 to access data[1] */
    testFrame.can_dlc = 2;
    testFrame.data[0] = 0x03;
    testFrame.data[1] = 0x10;
    int pci = testFrame.data[0];
    int sid = testFrame.data[1];
    /* simulated frame_data for valid single frame response */
    std::vector<uint8_t> frame_data = {0x03, 0x10};
    bool isMultiFrame = false;

    /* redirect stdout to capture the output */
    testing::internal::CaptureStdout();
    handler.processFrameData(testFrame.can_id, sid, frame_data, isMultiFrame);
    std::string output = testing::internal::GetCapturedStdout();
    /* check if the correct message is printed */
    EXPECT_EQ(output, "DiagnosticSessionControl called.\n");
}

/* Test for processFrameData with a negative response frame */
TEST_F(HandleFramesTest, DiagnosticSessionControlNegTest){
    /* Set an arbitrary CAN ID */
    testFrame.can_id= 0;
    /* Set the Data Length Code to at least 2 to access data[1] */
    testFrame.can_dlc = 2;
    testFrame.data[0] = 0x03;
    testFrame.data[1] = 0x7F;
    testFrame.data[2] = 0x10;
    int pci = testFrame.data[0];
    int sid = testFrame.data[1];
    std::vector<uint8_t> frame_data = {0x03, 0x7F, 0x10 /*or 0x22 rejected */};
    bool isMultiFrame = false;
    testing::internal::CaptureStdout();
    handler.processFrameData(testFrame.can_id, sid, frame_data, isMultiFrame);
    std::string output = testing::internal::GetCapturedStdout();
    EXPECT_EQ(output, "DiagnosticSessionControl called.\n");
}

/* Test for handleFrame with a valid single frame - DiagnosticSessionControl SID*/
TEST_F(HandleFramesTest, DiagnosticSessionControlSingTest){
    /* Set an arbitrary CAN ID */
    testFrame.can_id= 0;
    /* Set the Data Length Code to at least 2 to access data[1] */
    testFrame.can_dlc = 2;
    testFrame.data[0] = 0x03;
    testFrame.data[1] = 0x10;
    int pci = testFrame.data[0];
    int sid = testFrame.data[1];
    /* simulated frame_data for valid single frame response */
    std::vector<uint8_t> frame_data = {0x03, 0x10};
    bool isMultiFrame = false;

    /* redirect stdout to capture the output */
    testing::internal::CaptureStdout();
    handler.handleFrame(testFrame);
    std::string output = testing::internal::GetCapturedStdout();
    /* check if the correct message is printed */
    EXPECT_EQ(output, "Single Frame received:\n\nDiagnosticSessionControl called.\n");
}

/* Test for processFrameData with a valid EcuReset frame */
TEST_F(HandleFramesTest, EcuResetValTest){
    /* Set an arbitrary CAN ID */
    testFrame.can_id= 0;
    /* Set the Data Length Code to at least 2 to access data[1] */
    testFrame.can_dlc = 2;
    testFrame.data[0] = 0x03;
    testFrame.data[1] = 0x11;
    int pci = testFrame.data[0];
    int sid = testFrame.data[1];
    /* simulated frame_data for valid single frame response */
    std::vector<uint8_t> frame_data = {0x03, 0x11};
    bool isMultiFrame = false;

    /* redirect stdout to capture the output */
    testing::internal::CaptureStdout();
    handler.processFrameData(testFrame.can_id, sid, frame_data, isMultiFrame);
    std::string output = testing::internal::GetCapturedStdout();
    /* check if the correct message is printed */
    EXPECT_EQ(output, "EcuReset called.\n");
}

/* Test for handleFrame with a valid single frame - EcuReset SID*/
TEST_F(HandleFramesTest, EcuResetSingTest){
    /* Set an arbitrary CAN ID */
    testFrame.can_id= 0;
    /* Set the Data Length Code to at least 2 to access data[1] */
    testFrame.can_dlc = 2;
    testFrame.data[0] = 0x03;
    testFrame.data[1] = 0x11;
    int pci = testFrame.data[0];
    int sid = testFrame.data[1];
    /* simulated frame_data for valid single frame response */
    std::vector<uint8_t> frame_data = {0x03, 0x11};
    bool isMultiFrame = false;

    /* redirect stdout to capture the output */
    testing::internal::CaptureStdout();
    handler.handleFrame(testFrame);
    std::string output = testing::internal::GetCapturedStdout();
    /* check if the correct message is printed */
    EXPECT_EQ(output, "Single Frame received:\n\nEcuReset called.\n");
}

/* Test for processFrameData with a valid SecurityAccess frame */
TEST_F(HandleFramesTest, SecurityAccessValTest){
    /* Set an arbitrary CAN ID */
    testFrame.can_id= 0;
    /* Set the Data Length Code to at least 2 to access data[1] */
    testFrame.can_dlc = 2;
    testFrame.data[0] = 0x03;
    testFrame.data[1] = 0x27;
    int pci = testFrame.data[0];
    int sid = testFrame.data[1];
    /* simulated frame_data for valid single frame response */
    std::vector<uint8_t> frame_data = {0x03, 0x27};
    bool isMultiFrame = false;

    /* redirect stdout to capture the output */
    testing::internal::CaptureStdout();
    handler.processFrameData(testFrame.can_id, sid, frame_data, isMultiFrame);
    std::string output = testing::internal::GetCapturedStdout();
    /* check if the correct message is printed */
    EXPECT_EQ(output, "SecurityAccess called.\n");
}

/* Test for handleFrame with a valid single frame - SecurityAccess SID*/
TEST_F(HandleFramesTest, SecurityAccessSingTest){
    /* Set an arbitrary CAN ID */
    testFrame.can_id= 0;
    /* Set the Data Length Code to at least 2 to access data[1] */
    testFrame.can_dlc = 2;
    testFrame.data[0] = 0x03;
    testFrame.data[1] = 0x27;
    int pci = testFrame.data[0];
    int sid = testFrame.data[1];
    /* simulated frame_data for valid single frame response */
    std::vector<uint8_t> frame_data = {0x03, 0x27};
    bool isMultiFrame = false;

    /* redirect stdout to capture the output */
    testing::internal::CaptureStdout();
    handler.handleFrame(testFrame);
    std::string output = testing::internal::GetCapturedStdout();
    /* check if the correct message is printed */
    EXPECT_EQ(output, "Single Frame received:\n\nSecurityAccess called.\n");
}

/* Test for processFrameData with a valid Authentication frame */
TEST_F(HandleFramesTest, AuthenticationValTest){
    /* Set an arbitrary CAN ID */
    testFrame.can_id= 0;
    /* Set the Data Length Code to at least 2 to access data[1] */
    testFrame.can_dlc = 2;
    testFrame.data[0] = 0x03;
    testFrame.data[1] = 0x29;
    int pci = testFrame.data[0];
    int sid = testFrame.data[1];
    /* simulated frame_data for valid single frame response */
    std::vector<uint8_t> frame_data = {0x03, 0x29};
    bool isMultiFrame = false;

    /* redirect stdout to capture the output */
    testing::internal::CaptureStdout();
    handler.processFrameData(testFrame.can_id, sid, frame_data, isMultiFrame);
    std::string output = testing::internal::GetCapturedStdout();
    /* check if the correct message is printed */
    EXPECT_EQ(output, "Authentication called.\n");
}

/* Test for handleFrame with a valid single frame - Authentication SID*/
TEST_F(HandleFramesTest, AuthenticationSingTest){
    /* Set an arbitrary CAN ID */
    testFrame.can_id= 0;
    /* Set the Data Length Code to at least 2 to access data[1] */
    testFrame.can_dlc = 2;
    testFrame.data[0] = 0x03;
    testFrame.data[1] = 0x29;
    int pci = testFrame.data[0];
    int sid = testFrame.data[1];
    /* simulated frame_data for valid single frame response */
    std::vector<uint8_t> frame_data = {0x03, 0x29};
    bool isMultiFrame = false;

    /* redirect stdout to capture the output */
    testing::internal::CaptureStdout();
    handler.handleFrame(testFrame);
    std::string output = testing::internal::GetCapturedStdout();
    /* check if the correct message is printed */
    EXPECT_EQ(output, "Single Frame received:\n\nAuthentication called.\n");
}

/* Test for processFrameData with a valid TesterPresent frame */
TEST_F(HandleFramesTest, TesterPresentValTest){
    /* Set an arbitrary CAN ID */
    testFrame.can_id= 0;
    /* Set the Data Length Code to at least 2 to access data[1] */
    testFrame.can_dlc = 2;
    testFrame.data[0] = 0x03;
    testFrame.data[1] = 0x3E;
    int pci = testFrame.data[0];
    int sid = testFrame.data[1];
    /* simulated frame_data for valid single frame response */
    std::vector<uint8_t> frame_data = {0x03, 0x3E};
    bool isMultiFrame = false;

    /* redirect stdout to capture the output */
    testing::internal::CaptureStdout();
    handler.processFrameData(testFrame.can_id, sid, frame_data, isMultiFrame);
    std::string output = testing::internal::GetCapturedStdout();
    /* check if the correct message is printed */
    EXPECT_EQ(output, "TesterPresent called.\n");
}

/* Test for handleFrame with a valid single frame - TesterPresent SID*/
TEST_F(HandleFramesTest, TesterPresentSingTest){
    /* Set an arbitrary CAN ID */
    testFrame.can_id= 0;
    /* Set the Data Length Code to at least 2 to access data[1] */
    testFrame.can_dlc = 2;
    testFrame.data[0] = 0x03;
    testFrame.data[1] = 0x3E;
    int pci = testFrame.data[0];
    int sid = testFrame.data[1];
    /* simulated frame_data for valid single frame response */
    std::vector<uint8_t> frame_data = {0x03, 0x3E};
    bool isMultiFrame = false;

    /* redirect stdout to capture the output */
    testing::internal::CaptureStdout();
    handler.handleFrame(testFrame);
    std::string output = testing::internal::GetCapturedStdout();
    /* check if the correct message is printed */
    EXPECT_EQ(output, "Single Frame received:\n\nTesterPresent called.\n");
}

/* Test for processFrameData with a valid AccessTimingParameters frame */
TEST_F(HandleFramesTest, AccessTimingParametersValTest){
    /* Set an arbitrary CAN ID */
    testFrame.can_id= 0;
    /* Set the Data Length Code to at least 2 to access data[1] */
    testFrame.can_dlc = 2;
    testFrame.data[0] = 0x03;
    testFrame.data[1] = 0x83;
    int pci = testFrame.data[0];
    int sid = testFrame.data[1];
    /* simulated frame_data for valid single frame response */
    std::vector<uint8_t> frame_data = {0x03, 0x83};
    bool isMultiFrame = false;

    /* redirect stdout to capture the output */
    testing::internal::CaptureStdout();
    handler.processFrameData(testFrame.can_id, sid, frame_data, isMultiFrame);
    std::string output = testing::internal::GetCapturedStdout();
    /* check if the correct message is printed */
    EXPECT_EQ(output, "AccessTimingParameters called.\n");
}

/* Test for handleFrame with a valid single frame - AccessTimingParameters SID*/
TEST_F(HandleFramesTest, AccessTimingParametersSingTest){
    /* Set an arbitrary CAN ID */
    testFrame.can_id= 0;
    /* Set the Data Length Code to at least 2 to access data[1] */
    testFrame.can_dlc = 2;
    testFrame.data[0] = 0x03;
    testFrame.data[1] = 0x83;
    int pci = testFrame.data[0];
    int sid = testFrame.data[1];
    /* simulated frame_data for valid single frame response */
    std::vector<uint8_t> frame_data = {0x03, 0x83};
    bool isMultiFrame = false;

    /* redirect stdout to capture the output */
    testing::internal::CaptureStdout();
    handler.handleFrame(testFrame);
    std::string output = testing::internal::GetCapturedStdout();
    /* check if the correct message is printed */
    EXPECT_EQ(output, "Single Frame received:\n\nAccessTimingParameters called.\n");
}

/* Test for processFrameData with a valid AccessTimingParameters frame */
TEST_F(HandleFramesTest, ReadDataByIdentifierValTest){
    /* Set an arbitrary CAN ID */
    testFrame.can_id= 0;
    /* Set the Data Length Code to at least 2 to access data[1] */
    testFrame.can_dlc = 2;
    testFrame.data[0] = 0x03;
    testFrame.data[1] = 0x22;
    int pci = testFrame.data[0];
    int sid = testFrame.data[1];
    /* simulated frame_data for valid single frame response */
    std::vector<uint8_t> frame_data = {0x03, 0x22};
    bool isMultiFrame = false;

    /* redirect stdout to capture the output */
    testing::internal::CaptureStdout();
    handler.processFrameData(testFrame.can_id, sid, frame_data, isMultiFrame);
    std::string output = testing::internal::GetCapturedStdout();
    /* check if the correct message is printed */
    EXPECT_EQ(output, "ReadDataByIdentifier called.\n");
}

/* Test for handleFrame with a valid single frame - ReadDataByIdentifier SID*/
TEST_F(HandleFramesTest, ReadDataByIdentifierSingTest){
    /* Set an arbitrary CAN ID */
    testFrame.can_id= 0;
    /* Set the Data Length Code to at least 2 to access data[1] */
    testFrame.can_dlc = 2;
    testFrame.data[0] = 0x03;
    testFrame.data[1] = 0x22;
    int pci = testFrame.data[0];
    int sid = testFrame.data[1];
    /* simulated frame_data for valid single frame response */
    std::vector<uint8_t> frame_data = {0x03, 0x22};
    bool isMultiFrame = false;

    /* redirect stdout to capture the output */
    testing::internal::CaptureStdout();
    handler.handleFrame(testFrame);
    std::string output = testing::internal::GetCapturedStdout();
    /* check if the correct message is printed */
    EXPECT_EQ(output, "Single Frame received:\n\nReadDataByIdentifier called.\n");
}

/* Test for processFrameData with a valid ReadMemoryByAddress frame */
TEST_F(HandleFramesTest, ReadMemoryByAddressValTest){
    /* Set an arbitrary CAN ID */
    testFrame.can_id= 0;
    /* Set the Data Length Code to at least 2 to access data[1] */
    testFrame.can_dlc = 2;
    testFrame.data[0] = 0x03;
    testFrame.data[1] = 0x23;
    int pci = testFrame.data[0];
    int sid = testFrame.data[1];
    /* simulated frame_data for valid single frame response */
    std::vector<uint8_t> frame_data = {0x03, 0x23};
    bool isMultiFrame = false;

    /* redirect stdout to capture the output */
    testing::internal::CaptureStdout();
    handler.processFrameData(testFrame.can_id, sid, frame_data, isMultiFrame);
    std::string output = testing::internal::GetCapturedStdout();
    /* check if the correct message is printed */
    EXPECT_EQ(output, "ReadMemoryByAddress called.\n");
}

/* Test for handleFrame with a valid single frame - ReadMemoryByAddress SID*/
TEST_F(HandleFramesTest, ReadMemoryByAddressSingTest){
    /* Set an arbitrary CAN ID */
    testFrame.can_id= 0;
    /* Set the Data Length Code to at least 2 to access data[1] */
    testFrame.can_dlc = 2;
    testFrame.data[0] = 0x03;
    testFrame.data[1] = 0x23;
    int pci = testFrame.data[0];
    int sid = testFrame.data[1];
    /* simulated frame_data for valid single frame response */
    std::vector<uint8_t> frame_data = {0x03, 0x23};
    bool isMultiFrame = false;

    /* redirect stdout to capture the output */
    testing::internal::CaptureStdout();
    handler.handleFrame(testFrame);
    std::string output = testing::internal::GetCapturedStdout();
    /* check if the correct message is printed */
    EXPECT_EQ(output, "Single Frame received:\n\nReadMemoryByAddress called.\n");
}

/* Test for processFrameData with a valid WriteDataByIdentifier frame */
TEST_F(HandleFramesTest, WriteDataByIdentifierValTest){
    /* Set an arbitrary CAN ID */
    testFrame.can_id= 0;
    /* Set the Data Length Code to at least 2 to access data[1] */
    testFrame.can_dlc = 2;
    testFrame.data[0] = 0x03;
    testFrame.data[1] = 0x2E;
    int pci = testFrame.data[0];
    int sid = testFrame.data[1];
    /* simulated frame_data for valid single frame response */
    std::vector<uint8_t> frame_data = {0x03, 0x2E};
    bool isMultiFrame = false;

    /* redirect stdout to capture the output */
    testing::internal::CaptureStdout();
    handler.processFrameData(testFrame.can_id, sid, frame_data, isMultiFrame);
    std::string output = testing::internal::GetCapturedStdout();
    /* check if the correct message is printed */
    EXPECT_EQ(output, "WriteDataByIdentifier called with one frame.\n");
}

/* Test for handleFrame with a valid single frame - WriteDataByIdentifier SID*/
TEST_F(HandleFramesTest, WriteDataByIdentifierSingTest){
    /* Set an arbitrary CAN ID */
    testFrame.can_id= 0;
    /* Set the Data Length Code to at least 2 to access data[1] */
    testFrame.can_dlc = 2;
    testFrame.data[0] = 0x03;
    testFrame.data[1] = 0x2E;
    int pci = testFrame.data[0];
    int sid = testFrame.data[1];
    /* simulated frame_data for valid single frame response */
    std::vector<uint8_t> frame_data = {0x03, 0x2E};
    bool isMultiFrame = false;

    /* redirect stdout to capture the output */
    testing::internal::CaptureStdout();
    handler.handleFrame(testFrame);
    std::string output = testing::internal::GetCapturedStdout();
    /* check if the correct message is printed */
    EXPECT_EQ(output, "Single Frame received:\n\nWriteDataByIdentifier called with one frame.\n");
}

/* Test for processFrameData with a valid ClearDiagnosticInformation frame */
TEST_F(HandleFramesTest, ClearDiagnosticInformationValTest){
    /* Set an arbitrary CAN ID */
    testFrame.can_id= 0;
    /* Set the Data Length Code to at least 2 to access data[1] */
    testFrame.can_dlc = 2;
    testFrame.data[0] = 0x03;
    testFrame.data[1] = 0x14;
    int pci = testFrame.data[0];
    int sid = testFrame.data[1];
    /* simulated frame_data for valid single frame response */
    std::vector<uint8_t> frame_data = {0x03, 0x14};
    bool isMultiFrame = false;

    /* redirect stdout to capture the output */
    testing::internal::CaptureStdout();
    handler.processFrameData(testFrame.can_id, sid, frame_data, isMultiFrame);
    std::string output = testing::internal::GetCapturedStdout();
    /* check if the correct message is printed */
    EXPECT_EQ(output, "ClearDiagnosticInformation called.\n");
}

/* Test for handleFrame with a valid single frame - ClearDiagnosticInformation SID*/
TEST_F(HandleFramesTest, ClearDiagnosticInformationSingTest){
    /* Set an arbitrary CAN ID */
    testFrame.can_id= 0;
    /* Set the Data Length Code to at least 2 to access data[1] */
    testFrame.can_dlc = 2;
    testFrame.data[0] = 0x03;
    testFrame.data[1] = 0x14;
    int pci = testFrame.data[0];
    int sid = testFrame.data[1];
    /* simulated frame_data for valid single frame response */
    std::vector<uint8_t> frame_data = {0x03, 0x14};
    bool isMultiFrame = false;

    /* redirect stdout to capture the output */
    testing::internal::CaptureStdout();
    handler.handleFrame(testFrame);
    std::string output = testing::internal::GetCapturedStdout();
    /* check if the correct message is printed */
    EXPECT_EQ(output, "Single Frame received:\n\nClearDiagnosticInformation called.\n");
}

/* Test for processFrameData with a valid ReadDtcInformation frame */
TEST_F(HandleFramesTest, ReadDtcInformationValTest){
    /* Set an arbitrary CAN ID */
    testFrame.can_id= 0;
    /* Set the Data Length Code to at least 2 to access data[1] */
    testFrame.can_dlc = 2;
    testFrame.data[0] = 0x03;
    testFrame.data[1] = 0x19;
    int pci = testFrame.data[0];
    int sid = testFrame.data[1];
    /* simulated frame_data for valid single frame response */
    std::vector<uint8_t> frame_data = {0x03, 0x19};
    bool isMultiFrame = false;

    /* redirect stdout to capture the output */
    testing::internal::CaptureStdout();
    handler.processFrameData(testFrame.can_id, sid, frame_data, isMultiFrame);
    std::string output = testing::internal::GetCapturedStdout();
    /* check if the correct message is printed */
    EXPECT_EQ(output, "ReadDtcInformation called.\n");
}

/* Test for handleFrame with a valid single frame - ReadDtcInformation SID*/
TEST_F(HandleFramesTest, ReadDtcInformationSingTest){
    /* Set an arbitrary CAN ID */
    testFrame.can_id= 0;
    /* Set the Data Length Code to at least 2 to access data[1] */
    testFrame.can_dlc = 2;
    testFrame.data[0] = 0x03;
    testFrame.data[1] = 0x19;
    int pci = testFrame.data[0];
    int sid = testFrame.data[1];
    /* simulated frame_data for valid single frame response */
    std::vector<uint8_t> frame_data = {0x03, 0x19};
    bool isMultiFrame = false;

    /* redirect stdout to capture the output */
    testing::internal::CaptureStdout();
    handler.handleFrame(testFrame);
    std::string output = testing::internal::GetCapturedStdout();
    /* check if the correct message is printed */
    EXPECT_EQ(output, "Single Frame received:\n\nReadDtcInformation called.\n");
}

/* Test for processFrameData with a valid RoutineControl frame */
TEST_F(HandleFramesTest, RoutineControlValTest){
    /* Set an arbitrary CAN ID */
    testFrame.can_id= 0;
    /* Set the Data Length Code to at least 2 to access data[1] */
    testFrame.can_dlc = 2;
    testFrame.data[0] = 0x03;
    testFrame.data[1] = 0x31;
    int pci = testFrame.data[0];
    int sid = testFrame.data[1];
    /* simulated frame_data for valid single frame response */
    std::vector<uint8_t> frame_data = {0x03, 0x31};
    bool isMultiFrame = false;

    /* redirect stdout to capture the output */
    testing::internal::CaptureStdout();
    handler.processFrameData(testFrame.can_id, sid, frame_data, isMultiFrame);
    std::string output = testing::internal::GetCapturedStdout();
    /* check if the correct message is printed */
    EXPECT_EQ(output, "RoutineControl called.\n");
}

/* Test for handleFrame with a valid single frame - RoutineControl SID*/
TEST_F(HandleFramesTest, RoutineControlSingTest){
    /* Set an arbitrary CAN ID */
    testFrame.can_id= 0;
    /* Set the Data Length Code to at least 2 to access data[1] */
    testFrame.can_dlc = 2;
    testFrame.data[0] = 0x03;
    testFrame.data[1] = 0x31;
    int pci = testFrame.data[0];
    int sid = testFrame.data[1];
    /* simulated frame_data for valid single frame response */
    std::vector<uint8_t> frame_data = {0x03, 0x31};
    bool isMultiFrame = false;

    /* redirect stdout to capture the output */
    testing::internal::CaptureStdout();
    handler.handleFrame(testFrame);
    std::string output = testing::internal::GetCapturedStdout();
    /* check if the correct message is printed */
    EXPECT_EQ(output, "Single Frame received:\n\nRoutineControl called.\n");
}

/* Test for processFrameData with a valid ResponseDiagnosticSessionControl frame */
TEST_F(HandleFramesTest, ResponseDiagnosticSessionControlValTest){
    /* Set an arbitrary CAN ID */
    testFrame.can_id= 0;
    /* Set the Data Length Code to at least 2 to access data[1] */
    testFrame.can_dlc = 2;
    testFrame.data[0] = 0x03;
    testFrame.data[1] = 0x50;
    int pci = testFrame.data[0];
    int sid = testFrame.data[1];
    /* simulated frame_data for valid single frame response */
    std::vector<uint8_t> frame_data = {0x03, 0x31};
    bool isMultiFrame = false;

    /* redirect stdout to capture the output */
    testing::internal::CaptureStdout();
    handler.processFrameData(testFrame.can_id, sid, frame_data, isMultiFrame);
    std::string output = testing::internal::GetCapturedStdout();
    /* check if the correct message is printed */
    EXPECT_EQ(output, "Response for DiagnosticSessionControl received.\n");
}

/* Test for handleFrame with a valid single frame - ResponseDiagnosticSessionControl SID*/
TEST_F(HandleFramesTest, ResponseDiagnosticSessionControlSingTest){
    /* Set an arbitrary CAN ID */
    testFrame.can_id= 0;
    /* Set the Data Length Code to at least 2 to access data[1] */
    testFrame.can_dlc = 2;
    testFrame.data[0] = 0x03;
    testFrame.data[1] = 0x50;
    int pci = testFrame.data[0];
    int sid = testFrame.data[1];
    /* simulated frame_data for valid single frame response */
    std::vector<uint8_t> frame_data = {0x03, 0x50};
    bool isMultiFrame = false;

    /* redirect stdout to capture the output */
    testing::internal::CaptureStdout();
    handler.handleFrame(testFrame);
    std::string output = testing::internal::GetCapturedStdout();
    /* check if the correct message is printed */
    EXPECT_EQ(output, "Single Frame received:\n\nResponse for DiagnosticSessionControl received.\n");
}

/* Test for processFrameData with a valid ResponseEcuReset frame */
TEST_F(HandleFramesTest, ResponseEcuResetValTest){
    /* Set an arbitrary CAN ID */
    testFrame.can_id= 0;
    /* Set the Data Length Code to at least 2 to access data[1] */
    testFrame.can_dlc = 2;
    testFrame.data[0] = 0x03;
    testFrame.data[1] = 0x51;
    int pci = testFrame.data[0];
    int sid = testFrame.data[1];
    /* simulated frame_data for valid single frame response */
    std::vector<uint8_t> frame_data = {0x03, 0x51};
    bool isMultiFrame = false;

    /* redirect stdout to capture the output */
    testing::internal::CaptureStdout();
    handler.processFrameData(testFrame.can_id, sid, frame_data, isMultiFrame);
    std::string output = testing::internal::GetCapturedStdout();
    /* check if the correct message is printed */
    EXPECT_EQ(output, "Response from EcuReset received.\n");
}

/* Test for handleFrame with a valid single frame - ResponseEcuReset SID*/
TEST_F(HandleFramesTest, ResponseEcuResetSingTest){
    /* Set an arbitrary CAN ID */
    testFrame.can_id= 0;
    /* Set the Data Length Code to at least 2 to access data[1] */
    testFrame.can_dlc = 2;
    testFrame.data[0] = 0x03;
    testFrame.data[1] = 0x51;
    int pci = testFrame.data[0];
    int sid = testFrame.data[1];
    /* simulated frame_data for valid single frame response */
    std::vector<uint8_t> frame_data = {0x03, 0x51};
    bool isMultiFrame = false;

    /* redirect stdout to capture the output */
    testing::internal::CaptureStdout();
    handler.handleFrame(testFrame);
    std::string output = testing::internal::GetCapturedStdout();
    /* check if the correct message is printed */
    EXPECT_EQ(output, "Single Frame received:\n\nResponse from EcuReset received.\n");
}

/* Test for processFrameData with a valid ResponseSecurityAccess frame */
TEST_F(HandleFramesTest, ResponseSecurityAccessValTest){
    /* Set an arbitrary CAN ID */
    testFrame.can_id= 0;
    /* Set the Data Length Code to at least 2 to access data[1] */
    testFrame.can_dlc = 2;
    testFrame.data[0] = 0x03;
    testFrame.data[1] = 0x67;
    int pci = testFrame.data[0];
    int sid = testFrame.data[1];
    /* simulated frame_data for valid single frame response */
    std::vector<uint8_t> frame_data = {0x03, 0x67};
    bool isMultiFrame = false;

    /* redirect stdout to capture the output */
    testing::internal::CaptureStdout();
    handler.processFrameData(testFrame.can_id, sid, frame_data, isMultiFrame);
    std::string output = testing::internal::GetCapturedStdout();
    /* check if the correct message is printed */
    EXPECT_EQ(output, "Response from SecurityAccess received.\n");
}

/* Test for handleFrame with a valid single frame - ResponseSecurityAccess SID*/
TEST_F(HandleFramesTest, ResponseSecurityAccessSingTest){
    /* Set an arbitrary CAN ID */
    testFrame.can_id= 0;
    /* Set the Data Length Code to at least 2 to access data[1] */
    testFrame.can_dlc = 2;
    testFrame.data[0] = 0x03;
    testFrame.data[1] = 0x67;
    int pci = testFrame.data[0];
    int sid = testFrame.data[1];
    /* simulated frame_data for valid single frame response */
    std::vector<uint8_t> frame_data = {0x03, 0x67};
    bool isMultiFrame = false;

    /* redirect stdout to capture the output */
    testing::internal::CaptureStdout();
    handler.handleFrame(testFrame);
    std::string output = testing::internal::GetCapturedStdout();
    /* check if the correct message is printed */
    EXPECT_EQ(output, "Single Frame received:\n\nResponse from SecurityAccess received.\n");
}

/* Test for processFrameData with a valid ResponseAuthentication frame */
TEST_F(HandleFramesTest, ResponseAuthenticationValTest){
    /* Set an arbitrary CAN ID */
    testFrame.can_id= 0;
    /* Set the Data Length Code to at least 2 to access data[1] */
    testFrame.can_dlc = 2;
    testFrame.data[0] = 0x03;
    testFrame.data[1] = 0x69;
    int pci = testFrame.data[0];
    int sid = testFrame.data[1];
    /* simulated frame_data for valid single frame response */
    std::vector<uint8_t> frame_data = {0x03, 0x69};
    bool isMultiFrame = false;

    /* redirect stdout to capture the output */
    testing::internal::CaptureStdout();
    handler.processFrameData(testFrame.can_id, sid, frame_data, isMultiFrame);
    std::string output = testing::internal::GetCapturedStdout();
    /* check if the correct message is printed */
    EXPECT_EQ(output, "Response from Authentication received.\n");
}

/* Test for handleFrame with a valid single frame - ResponseAuthentication SID*/
TEST_F(HandleFramesTest, ResponseAuthenticationSingTest){
    /* Set an arbitrary CAN ID */
    testFrame.can_id= 0;
    /* Set the Data Length Code to at least 2 to access data[1] */
    testFrame.can_dlc = 2;
    testFrame.data[0] = 0x03;
    testFrame.data[1] = 0x69;
    int pci = testFrame.data[0];
    int sid = testFrame.data[1];
    /* simulated frame_data for valid single frame response */
    std::vector<uint8_t> frame_data = {0x03, 0x69};
    bool isMultiFrame = false;

    /* redirect stdout to capture the output */
    testing::internal::CaptureStdout();
    handler.handleFrame(testFrame);
    std::string output = testing::internal::GetCapturedStdout();
    /* check if the correct message is printed */
    EXPECT_EQ(output, "Single Frame received:\n\nResponse from Authentication received.\n");
}

/* Test for processFrameData with a valid ResponseTesterPresent frame */
TEST_F(HandleFramesTest, ResponseTesterPresentValTest){
    /* Set an arbitrary CAN ID */
    testFrame.can_id= 0;
    /* Set the Data Length Code to at least 2 to access data[1] */
    testFrame.can_dlc = 2;
    testFrame.data[0] = 0x03;
    testFrame.data[1] = 0x7E;
    int pci = testFrame.data[0];
    int sid = testFrame.data[1];
    /* simulated frame_data for valid single frame response */
    std::vector<uint8_t> frame_data = {0x03, 0x7E};
    bool isMultiFrame = false;

    /* redirect stdout to capture the output */
    testing::internal::CaptureStdout();
    handler.processFrameData(testFrame.can_id, sid, frame_data, isMultiFrame);
    std::string output = testing::internal::GetCapturedStdout();
    /* check if the correct message is printed */
    EXPECT_EQ(output, "Response from TesterPresent received.\n");
}

/* Test for handleFrame with a valid single frame - ResponseTesterPresent SID*/
TEST_F(HandleFramesTest, ResponseTesterPresentSingTest){
    /* Set an arbitrary CAN ID */
    testFrame.can_id= 0;
    /* Set the Data Length Code to at least 2 to access data[1] */
    testFrame.can_dlc = 2;
    testFrame.data[0] = 0x03;
    testFrame.data[1] = 0x7E;
    int pci = testFrame.data[0];
    int sid = testFrame.data[1];
    /* simulated frame_data for valid single frame response */
    std::vector<uint8_t> frame_data = {0x03, 0x7E};
    bool isMultiFrame = false;

    /* redirect stdout to capture the output */
    testing::internal::CaptureStdout();
    handler.handleFrame(testFrame);
    std::string output = testing::internal::GetCapturedStdout();
    /* check if the correct message is printed */
    EXPECT_EQ(output, "Single Frame received:\n\nResponse from TesterPresent received.\n");
}

/* Test for processFrameData with a valid ResponseAccessTimingParameters frame */
TEST_F(HandleFramesTest, ResponseAccessTimingParametersValTest){
    /* Set an arbitrary CAN ID */
    testFrame.can_id= 0;
    /* Set the Data Length Code to at least 2 to access data[1] */
    testFrame.can_dlc = 2;
    testFrame.data[0] = 0x03;
    testFrame.data[1] = 0xC3;
    int pci = testFrame.data[0];
    int sid = testFrame.data[1];
    /* simulated frame_data for valid single frame response */
    std::vector<uint8_t> frame_data = {0x03, 0xC3};
    bool isMultiFrame = false;

    /* redirect stdout to capture the output */
    testing::internal::CaptureStdout();
    handler.processFrameData(testFrame.can_id, sid, frame_data, isMultiFrame);
    std::string output = testing::internal::GetCapturedStdout();
    /* check if the correct message is printed */
    EXPECT_EQ(output, "Response from AccessTimingParameters received.\n");
}

/* Test for handleFrame with a valid single frame - ResponseAccessTimingParameters SID*/
TEST_F(HandleFramesTest, ResponseAccessTimingParametersSingTest){
    /* Set an arbitrary CAN ID */
    testFrame.can_id= 0;
    /* Set the Data Length Code to at least 2 to access data[1] */
    testFrame.can_dlc = 2;
    testFrame.data[0] = 0x03;
    testFrame.data[1] = 0xC3;
    int pci = testFrame.data[0];
    int sid = testFrame.data[1];
    /* simulated frame_data for valid single frame response */
    std::vector<uint8_t> frame_data = {0x03, 0xC3};
    bool isMultiFrame = false;

    /* redirect stdout to capture the output */
    testing::internal::CaptureStdout();
    handler.handleFrame(testFrame);
    std::string output = testing::internal::GetCapturedStdout();
    /* check if the correct message is printed */
    EXPECT_EQ(output, "Single Frame received:\n\nResponse from AccessTimingParameters received.\n");
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