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
    EXPECT_EQ(sid, testFrame.data[1]);
}

/* Test the first frame */
TEST_F(HandleFramesTest, firstFrameTest){
    /* Set an arbitrary CAN ID */
    testFrame.can_id= 0;
    /* Set the Data Length Code to at least 2 to access data[1] */
    testFrame.can_dlc = 2;
    testFrame.data[0] = 0x10;
    testFrame.data[1] = 0x3E;
    testFrame.data[2] = 0x22;
    int pci = testFrame.data[0];
    int sid = testFrame.data[2];
   
    int rest =  testFrame.data[1] % 7 > 0 ? 1 : 0;
    int expectedFrames = testFrame.data[1] / 7 + rest;
    
    bool isMultiFrame = false;

    /* redirect stdout to capture the output */
    testing::internal::CaptureStdout();    
    handler.handleFrame(testFrame);
    std::string output = testing::internal::GetCapturedStdout();
    /* check if the correct message is printed */
    EXPECT_EQ(output, "Multi-frame Sequence with 9 frames:\n");   
}

/* Test if the frame's sequence number matches the expected sequence number */
TEST_F(HandleFramesTest, MultipleFrameTest){
    /* Set an arbitrary CAN ID */
    testFrame.can_id= 0;
    /* Set the Data Length Code to at least 2 to access data[1] */
    testFrame.can_dlc = 1;
    testFrame.data[0] = 0x23;    

    int expectedSequenceNumber = 0x21;
    
    bool isMultiFrame = true;

    /* redirect stdout to capture the output */
    testing::internal::CaptureStderr();    
    handler.handleFrame(testFrame);
    std::string output = testing::internal::GetCapturedStderr();
    /* check if the correct message is printed */
    EXPECT_EQ(output, "Invalid consecutive frame sequence: expected 21 but received 23\n");
                         
}

/* Test for processFrameData with a negative response frame */
TEST_F(HandleFramesTest, DiagnosticSessionControlNegTest){
    /* Set an arbitrary CAN ID */
    testFrame.can_id= 0;
    /* Set the Data Length Code to at least 4 to access data[3] */
    testFrame.can_dlc = 4;
    testFrame.data[0] = 0x03;
    testFrame.data[1] = 0x7F;
    testFrame.data[2] = 0x10;
    testFrame.data[3] = 0x90;
    //testFrame.data[2] = 0x10;
    int pci = testFrame.data[0];
    int sid = testFrame.data[1];
    std::vector<uint8_t> frame_data = {0x03, 0x7F, 0x10, 0x90};
    bool isMultiFrame = false;
    testing::internal::CaptureStdout();
    handler.processFrameData(testFrame.can_id, sid, frame_data, isMultiFrame);
    std::string output = testing::internal::GetCapturedStdout();
    EXPECT_EQ(output, "Negative response received with SID: 127 and message: \x90\n");
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

/* Test for processFrameData with a valid ResponseAccessTimingParameters frame */
TEST_F(HandleFramesTest, ResponseReadDataByIdentifierValTest){
    /* Set an arbitrary CAN ID */
    testFrame.can_id= 0;
    /* Set the Data Length Code to at least 2 to access data[1] */
    testFrame.can_dlc = 2;
    testFrame.data[0] = 0x03;
    testFrame.data[1] = 0x62;
    int pci = testFrame.data[0];
    int sid = testFrame.data[1];
    /* simulated frame_data for valid single frame response */
    std::vector<uint8_t> frame_data = {0x03, 0x62};
    bool isMultiFrame = false;

    /* redirect stdout to capture the output */
    testing::internal::CaptureStdout();
    handler.processFrameData(testFrame.can_id, sid, frame_data, isMultiFrame);
    std::string output = testing::internal::GetCapturedStdout();
    /* check if the correct message is printed */
    EXPECT_EQ(output, "Response from ReadDataByIdentifier received in one frame.\n");
}

/* Test for handleFrame with a valid single frame - ResponseAccessTimingParameters SID*/
TEST_F(HandleFramesTest, ResponseReadDataByIdentifierSingTest){
    /* Set an arbitrary CAN ID */
    testFrame.can_id= 0;
    /* Set the Data Length Code to at least 2 to access data[1] */
    testFrame.can_dlc = 2;
    testFrame.data[0] = 0x03;
    testFrame.data[1] = 0x62;
    int pci = testFrame.data[0];
    int sid = testFrame.data[1];
    /* simulated frame_data for valid single frame response */
    std::vector<uint8_t> frame_data = {0x03, 0x62};
    bool isMultiFrame = false;

    /* redirect stdout to capture the output */
    testing::internal::CaptureStdout();
    handler.handleFrame(testFrame);
    std::string output = testing::internal::GetCapturedStdout();
    /* check if the correct message is printed */
    EXPECT_EQ(output, "Single Frame received:\n\nResponse from ReadDataByIdentifier received in one frame.\n");
}

/* Test for processFrameData with a valid ResponseReadMemoryByAdress frame */
TEST_F(HandleFramesTest, ResponseReadMemoryByAdressValTest){
    /* Set an arbitrary CAN ID */
    testFrame.can_id= 0;
    /* Set the Data Length Code to at least 2 to access data[1] */
    testFrame.can_dlc = 2;
    testFrame.data[0] = 0x03;
    testFrame.data[1] = 0x63;
    int pci = testFrame.data[0];
    int sid = testFrame.data[1];
    /* simulated frame_data for valid single frame response */
    std::vector<uint8_t> frame_data = {0x03, 0x63};
    bool isMultiFrame = false;

    /* redirect stdout to capture the output */
    testing::internal::CaptureStdout();
    handler.processFrameData(testFrame.can_id, sid, frame_data, isMultiFrame);
    std::string output = testing::internal::GetCapturedStdout();
    /* check if the correct message is printed */
    EXPECT_EQ(output, "Response from ReadMemoryByAdress received in one frame.\n");
}

/* Test for handleFrame with a valid single frame - ResponseReadMemoryByAdress SID*/
TEST_F(HandleFramesTest, ResponseReadMemoryByAdressSingTest){
    /* Set an arbitrary CAN ID */
    testFrame.can_id= 0;
    /* Set the Data Length Code to at least 2 to access data[1] */
    testFrame.can_dlc = 2;
    testFrame.data[0] = 0x03;
    testFrame.data[1] = 0x63;
    int pci = testFrame.data[0];
    int sid = testFrame.data[1];
    /* simulated frame_data for valid single frame response */
    std::vector<uint8_t> frame_data = {0x03, 0x63};
    bool isMultiFrame = false;

    /* redirect stdout to capture the output */
    testing::internal::CaptureStdout();
    handler.handleFrame(testFrame);
    std::string output = testing::internal::GetCapturedStdout();
    /* check if the correct message is printed */
    EXPECT_EQ(output, "Single Frame received:\n\nResponse from ReadMemoryByAdress received in one frame.\n");
}

/* Test for processFrameData with a valid ResponseWriteDataByIdentifier frame */
TEST_F(HandleFramesTest, ResponseWriteDataByIdentifierValTest){
    /* Set an arbitrary CAN ID */
    testFrame.can_id= 0;
    /* Set the Data Length Code to at least 2 to access data[1] */
    testFrame.can_dlc = 2;
    testFrame.data[0] = 0x03;
    testFrame.data[1] = 0x6E;
    int pci = testFrame.data[0];
    int sid = testFrame.data[1];
    /* simulated frame_data for valid single frame response */
    std::vector<uint8_t> frame_data = {0x03, 0x6E};
    bool isMultiFrame = false;

    /* redirect stdout to capture the output */
    testing::internal::CaptureStdout();
    handler.processFrameData(testFrame.can_id, sid, frame_data, isMultiFrame);
    std::string output = testing::internal::GetCapturedStdout();
    /* check if the correct message is printed */
    EXPECT_EQ(output, "Response from WriteDataByIdentifier received.\n");
}

/* Test for handleFrame with a valid single frame - ResponseWriteDataByIdentifier SID*/
TEST_F(HandleFramesTest, ResponseWriteDataByIdentifierSingTest){
    /* Set an arbitrary CAN ID */
    testFrame.can_id= 0;
    /* Set the Data Length Code to at least 2 to access data[1] */
    testFrame.can_dlc = 2;
    testFrame.data[0] = 0x03;
    testFrame.data[1] = 0x6E;
    int pci = testFrame.data[0];
    int sid = testFrame.data[1];
    /* simulated frame_data for valid single frame response */
    std::vector<uint8_t> frame_data = {0x03, 0x6E};
    bool isMultiFrame = false;

    /* redirect stdout to capture the output */
    testing::internal::CaptureStdout();
    handler.handleFrame(testFrame);
    std::string output = testing::internal::GetCapturedStdout();
    /* check if the correct message is printed */
    EXPECT_EQ(output, "Single Frame received:\n\nResponse from WriteDataByIdentifier received.\n");
}

/* Test for processFrameData with a valid ResponseClearDiagnosticInformation frame */
TEST_F(HandleFramesTest, ResponseClearDiagnosticInformationValTest){
    /* Set an arbitrary CAN ID */
    testFrame.can_id= 0;
    /* Set the Data Length Code to at least 2 to access data[1] */
    testFrame.can_dlc = 2;
    testFrame.data[0] = 0x03;
    testFrame.data[1] = 0x54;
    int pci = testFrame.data[0];
    int sid = testFrame.data[1];
    /* simulated frame_data for valid single frame response */
    std::vector<uint8_t> frame_data = {0x03, 0x54};
    bool isMultiFrame = false;

    /* redirect stdout to capture the output */
    testing::internal::CaptureStdout();
    handler.processFrameData(testFrame.can_id, sid, frame_data, isMultiFrame);
    std::string output = testing::internal::GetCapturedStdout();
    /* check if the correct message is printed */
    EXPECT_EQ(output, "Response from ClearDiagnosticInformation received.\n");
}

/* Test for handleFrame with a valid single frame - ResponseClearDiagnosticInformation SID*/
TEST_F(HandleFramesTest, ResponseClearDiagnosticInformationSingTest){
    /* Set an arbitrary CAN ID */
    testFrame.can_id= 0;
    /* Set the Data Length Code to at least 2 to access data[1] */
    testFrame.can_dlc = 2;
    testFrame.data[0] = 0x03;
    testFrame.data[1] = 0x54;
    int pci = testFrame.data[0];
    int sid = testFrame.data[1];
    /* simulated frame_data for valid single frame response */
    std::vector<uint8_t> frame_data = {0x03, 0x54};
    bool isMultiFrame = false;

    /* redirect stdout to capture the output */
    testing::internal::CaptureStdout();
    handler.handleFrame(testFrame);
    std::string output = testing::internal::GetCapturedStdout();
    /* check if the correct message is printed */
    EXPECT_EQ(output, "Single Frame received:\n\nResponse from ClearDiagnosticInformation received.\n");
}

/* Test for processFrameData with a valid ResponseReadDtcInformation frame */
TEST_F(HandleFramesTest, ResponseReadDtcInformationValTest){
    /* Set an arbitrary CAN ID */
    testFrame.can_id= 0;
    /* Set the Data Length Code to at least 2 to access data[1] */
    testFrame.can_dlc = 2;
    testFrame.data[0] = 0x03;
    testFrame.data[1] = 0x59;
    int pci = testFrame.data[0];
    int sid = testFrame.data[1];
    /* simulated frame_data for valid single frame response */
    std::vector<uint8_t> frame_data = {0x03, 0x59};
    bool isMultiFrame = false;

    /* redirect stdout to capture the output */
    testing::internal::CaptureStdout();
    handler.processFrameData(testFrame.can_id, sid, frame_data, isMultiFrame);
    std::string output = testing::internal::GetCapturedStdout();
    /* check if the correct message is printed */
    EXPECT_EQ(output, "Response from ReadDtcInformation received.\n");
}

/* Test for handleFrame with a valid single frame - ResponseReadDtcInformation SID*/
TEST_F(HandleFramesTest, ResponseReadDtcInformationSingTest){
    /* Set an arbitrary CAN ID */
    testFrame.can_id= 0;
    /* Set the Data Length Code to at least 2 to access data[1] */
    testFrame.can_dlc = 2;
    testFrame.data[0] = 0x03;
    testFrame.data[1] = 0x59;
    int pci = testFrame.data[0];
    int sid = testFrame.data[1];
    /* simulated frame_data for valid single frame response */
    std::vector<uint8_t> frame_data = {0x03, 0x59};
    bool isMultiFrame = false;

    /* redirect stdout to capture the output */
    testing::internal::CaptureStdout();
    handler.handleFrame(testFrame);
    std::string output = testing::internal::GetCapturedStdout();
    /* check if the correct message is printed */
    EXPECT_EQ(output, "Single Frame received:\n\nResponse from ReadDtcInformation received.\n");
}

/* Test for processFrameData with a valid ResponseRoutineControl frame */
TEST_F(HandleFramesTest, ResponseRoutineControlValTest){
    /* Set an arbitrary CAN ID */
    testFrame.can_id= 0;
    /* Set the Data Length Code to at least 2 to access data[1] */
    testFrame.can_dlc = 2;
    testFrame.data[0] = 0x03;
    testFrame.data[1] = 0x71;
    int pci = testFrame.data[0];
    int sid = testFrame.data[1];
    /* simulated frame_data for valid single frame response */
    std::vector<uint8_t> frame_data = {0x03, 0x71};
    bool isMultiFrame = false;

    /* redirect stdout to capture the output */
    testing::internal::CaptureStdout();
    handler.processFrameData(testFrame.can_id, sid, frame_data, isMultiFrame);
    std::string output = testing::internal::GetCapturedStdout();
    /* check if the correct message is printed */
    EXPECT_EQ(output, "Response from RoutineControl received.\n");
}

/* Test for handleFrame with a valid single frame - ResponseRoutineControl SID*/
TEST_F(HandleFramesTest, ResponseRoutineControlSingTest){
    /* Set an arbitrary CAN ID */
    testFrame.can_id= 0;
    /* Set the Data Length Code to at least 2 to access data[1] */
    testFrame.can_dlc = 2;
    testFrame.data[0] = 0x03;
    testFrame.data[1] = 0x71;
    int pci = testFrame.data[0];
    int sid = testFrame.data[1];
    /* simulated frame_data for valid single frame response */
    std::vector<uint8_t> frame_data = {0x03, 0x71};
    bool isMultiFrame = false;

    /* redirect stdout to capture the output */
    testing::internal::CaptureStdout();
    handler.handleFrame(testFrame);
    std::string output = testing::internal::GetCapturedStdout();
    /* check if the correct message is printed */
    EXPECT_EQ(output, "Single Frame received:\n\nResponse from RoutineControl received.\n");
}

/* Test for processFrameData with a valid ResponseRoutineControl frame */
TEST_F(HandleFramesTest, OtaRequestDownloadValTest){
    /* Set an arbitrary CAN ID */
    testFrame.can_id= 0;
    /* Set the Data Length Code to at least 2 to access data[1] */
    testFrame.can_dlc = 2;
    testFrame.data[0] = 0x03;
    testFrame.data[1] = 0x34;
    int pci = testFrame.data[0];
    int sid = testFrame.data[1];
    /* simulated frame_data for valid single frame response */
    std::vector<uint8_t> frame_data = {0x03, 0x34};
    bool isMultiFrame = false;

    /* redirect stdout to capture the output */
    testing::internal::CaptureStdout();
    handler.processFrameData(testFrame.can_id, sid, frame_data, isMultiFrame);
    std::string output = testing::internal::GetCapturedStdout();
    /* check if the correct message is printed */
    EXPECT_EQ(output, "RequestDownload called.\n");
}

/* Test for handleFrame with a valid single frame - RequestDownload SID*/
TEST_F(HandleFramesTest, OtaRequestDownloadSingTest){
    /* Set an arbitrary CAN ID */
    testFrame.can_id= 0;
    /* Set the Data Length Code to at least 2 to access data[1] */
    testFrame.can_dlc = 2;
    testFrame.data[0] = 0x03;
    testFrame.data[1] = 0x34;
    int pci = testFrame.data[0];
    int sid = testFrame.data[1];
    /* simulated frame_data for valid single frame response */
    std::vector<uint8_t> frame_data = {0x03, 0x34};
    bool isMultiFrame = false;

    /* redirect stdout to capture the output */
    testing::internal::CaptureStdout();
    handler.handleFrame(testFrame);
    std::string output = testing::internal::GetCapturedStdout();
    /* check if the correct message is printed */
    EXPECT_EQ(output, "Single Frame received:\n\nRequestDownload called.\n");
}

/* Test for processFrameData with a valid TransferData frame */
TEST_F(HandleFramesTest, TransferDataValTest){
    /* Set an arbitrary CAN ID */
    testFrame.can_id= 0;
    /* Set the Data Length Code to at least 2 to access data[1] */
    testFrame.can_dlc = 2;
    testFrame.data[0] = 0x03;
    testFrame.data[1] = 0x36;
    int pci = testFrame.data[0];
    int sid = testFrame.data[1];
    /* simulated frame_data for valid single frame response */
    std::vector<uint8_t> frame_data = {0x03, 0x36};
    bool isMultiFrame = false;

    /* redirect stdout to capture the output */
    testing::internal::CaptureStdout();
    handler.processFrameData(testFrame.can_id, sid, frame_data, isMultiFrame);
    std::string output = testing::internal::GetCapturedStdout();
    /* check if the correct message is printed */
    EXPECT_EQ(output, "TransferData called with one frame\n");
}

/* Test for handleFrame with a valid single frame - TransferData SID*/
TEST_F(HandleFramesTest, TransferDataSingTest){
    /* Set an arbitrary CAN ID */
    testFrame.can_id= 0;
    /* Set the Data Length Code to at least 2 to access data[1] */
    testFrame.can_dlc = 2;
    testFrame.data[0] = 0x03;
    testFrame.data[1] = 0x36;
    int pci = testFrame.data[0];
    int sid = testFrame.data[1];
    /* simulated frame_data for valid single frame response */
    std::vector<uint8_t> frame_data = {0x03, 0x36};
    bool isMultiFrame = false;

    /* redirect stdout to capture the output */
    testing::internal::CaptureStdout();
    handler.handleFrame(testFrame);
    std::string output = testing::internal::GetCapturedStdout();
    /* check if the correct message is printed */
    EXPECT_EQ(output, "Single Frame received:\n\nTransferData called with one frame\n");
}

/* Test for processFrameData with a valid TransferData frame */
TEST_F(HandleFramesTest, RequestTransferExitValTest){
    /* Set an arbitrary CAN ID */
    testFrame.can_id= 0;
    /* Set the Data Length Code to at least 2 to access data[1] */
    testFrame.can_dlc = 2;
    testFrame.data[0] = 0x03;
    testFrame.data[1] = 0x37;
    int pci = testFrame.data[0];
    int sid = testFrame.data[1];
    /* simulated frame_data for valid single frame response */
    std::vector<uint8_t> frame_data = {0x03, 0x37};
    bool isMultiFrame = false;

    /* redirect stdout to capture the output */
    testing::internal::CaptureStdout();
    handler.processFrameData(testFrame.can_id, sid, frame_data, isMultiFrame);
    std::string output = testing::internal::GetCapturedStdout();
    /* check if the correct message is printed */
    EXPECT_EQ(output, "RequestTransferExit called.\n");
}

/* Test for handleFrame with a valid single frame - RequestTransferExit SID*/
TEST_F(HandleFramesTest, RequestTransferExitSingTest){
    /* Set an arbitrary CAN ID */
    testFrame.can_id= 0;
    /* Set the Data Length Code to at least 2 to access data[1] */
    testFrame.can_dlc = 2;
    testFrame.data[0] = 0x03;
    testFrame.data[1] = 0x37;
    int pci = testFrame.data[0];
    int sid = testFrame.data[1];
    /* simulated frame_data for valid single frame response */
    std::vector<uint8_t> frame_data = {0x03, 0x37};
    bool isMultiFrame = false;

    /* redirect stdout to capture the output */
    testing::internal::CaptureStdout();
    handler.handleFrame(testFrame);
    std::string output = testing::internal::GetCapturedStdout();
    /* check if the correct message is printed */
    EXPECT_EQ(output, "Single Frame received:\n\nRequestTransferExit called.\n");
}

/* Test for processFrameData with a valid RequestUpdateStatus frame */
TEST_F(HandleFramesTest, RequestUpdateStatusValTest){
    /* Set an arbitrary CAN ID */
    testFrame.can_id= 0;
    /* Set the Data Length Code to at least 2 to access data[1] */
    testFrame.can_dlc = 2;
    testFrame.data[0] = 0x03;
    testFrame.data[1] = 0x32;
    int pci = testFrame.data[0];
    int sid = testFrame.data[1];
    /* simulated frame_data for valid single frame response */
    std::vector<uint8_t> frame_data = {0x03, 0x32};
    bool isMultiFrame = false;

    /* redirect stdout to capture the output */
    testing::internal::CaptureStdout();
    handler.processFrameData(testFrame.can_id, sid, frame_data, isMultiFrame);
    std::string output = testing::internal::GetCapturedStdout();
    /* check if the correct message is printed */
    EXPECT_EQ(output, "RequestUpdateStatus called.\n");
}

/* Test for handleFrame with a valid single frame - RequestUpdateStatus SID*/
TEST_F(HandleFramesTest, RequestUpdateStatusSingTest){
    /* Set an arbitrary CAN ID */
    testFrame.can_id= 0;
    /* Set the Data Length Code to at least 2 to access data[1] */
    testFrame.can_dlc = 2;
    testFrame.data[0] = 0x03;
    testFrame.data[1] = 0x32;
    int pci = testFrame.data[0];
    int sid = testFrame.data[1];
    /* simulated frame_data for valid single frame response */
    std::vector<uint8_t> frame_data = {0x03, 0x32};
    bool isMultiFrame = false;

    /* redirect stdout to capture the output */
    testing::internal::CaptureStdout();
    handler.handleFrame(testFrame);
    std::string output = testing::internal::GetCapturedStdout();
    /* check if the correct message is printed */
    EXPECT_EQ(output, "Single Frame received:\n\nRequestUpdateStatus called.\n");
}

/* Test for processFrameData with a valid ResponseRequestDownload frame */
TEST_F(HandleFramesTest, ResponseRequestDownloadValTest){
    /* Set an arbitrary CAN ID */
    testFrame.can_id= 0;
    /* Set the Data Length Code to at least 2 to access data[1] */
    testFrame.can_dlc = 2;
    testFrame.data[0] = 0x03;
    testFrame.data[1] = 0x74;
    int pci = testFrame.data[0];
    int sid = testFrame.data[1];
    /* simulated frame_data for valid single frame response */
    std::vector<uint8_t> frame_data = {0x03, 0x74};
    bool isMultiFrame = false;

    /* redirect stdout to capture the output */
    testing::internal::CaptureStdout();
    handler.processFrameData(testFrame.can_id, sid, frame_data, isMultiFrame);
    std::string output = testing::internal::GetCapturedStdout();
    /* check if the correct message is printed */
    EXPECT_EQ(output, "Response from RequestDownload received.\n");
}

/* Test for handleFrame with a valid single frame - ResponseRequestDownload SID*/
TEST_F(HandleFramesTest, ResponseRequestDownloadSingTest){
    /* Set an arbitrary CAN ID */
    testFrame.can_id= 0;
    /* Set the Data Length Code to at least 2 to access data[1] */
    testFrame.can_dlc = 2;
    testFrame.data[0] = 0x03;
    testFrame.data[1] = 0x74;
    int pci = testFrame.data[0];
    int sid = testFrame.data[1];
    /* simulated frame_data for valid single frame response */
    std::vector<uint8_t> frame_data = {0x03, 0x74};
    bool isMultiFrame = false;

    /* redirect stdout to capture the output */
    testing::internal::CaptureStdout();
    handler.handleFrame(testFrame);
    std::string output = testing::internal::GetCapturedStdout();
    /* check if the correct message is printed */
    EXPECT_EQ(output, "Single Frame received:\n\nResponse from RequestDownload received.\n");
}

/* Test for processFrameData with a valid ResponseTransferData frame */
TEST_F(HandleFramesTest, ResponseTransferDataValTest){
    /* Set an arbitrary CAN ID */
    testFrame.can_id= 0;
    /* Set the Data Length Code to at least 2 to access data[1] */
    testFrame.can_dlc = 2;
    testFrame.data[0] = 0x03;
    testFrame.data[1] = 0x76;
    int pci = testFrame.data[0];
    int sid = testFrame.data[1];
    /* simulated frame_data for valid single frame response */
    std::vector<uint8_t> frame_data = {0x03, 0x76};
    bool isMultiFrame = false;

    /* redirect stdout to capture the output */
    testing::internal::CaptureStdout();
    handler.processFrameData(testFrame.can_id, sid, frame_data, isMultiFrame);
    std::string output = testing::internal::GetCapturedStdout();
    /* check if the correct message is printed */
    EXPECT_EQ(output, "Response from TransferData received.\n");
}

/* Test for handleFrame with a valid single frame - ResponseTransferData SID*/
TEST_F(HandleFramesTest, ResponseTransferDataSingTest){
    /* Set an arbitrary CAN ID */
    testFrame.can_id= 0;
    /* Set the Data Length Code to at least 2 to access data[1] */
    testFrame.can_dlc = 2;
    testFrame.data[0] = 0x03;
    testFrame.data[1] = 0x76;
    int pci = testFrame.data[0];
    int sid = testFrame.data[1];
    /* simulated frame_data for valid single frame response */
    std::vector<uint8_t> frame_data = {0x03, 0x76};
    bool isMultiFrame = false;

    /* redirect stdout to capture the output */
    testing::internal::CaptureStdout();
    handler.handleFrame(testFrame);
    std::string output = testing::internal::GetCapturedStdout();
    /* check if the correct message is printed */
    EXPECT_EQ(output, "Single Frame received:\n\nResponse from TransferData received.\n");
}

/* Test for processFrameData with a valid ResponseRequestTransferExit frame */
TEST_F(HandleFramesTest, ResponseRequestTransferExitValTest){
    /* Set an arbitrary CAN ID */
    testFrame.can_id= 0;
    /* Set the Data Length Code to at least 2 to access data[1] */
    testFrame.can_dlc = 2;
    testFrame.data[0] = 0x03;
    testFrame.data[1] = 0x77;
    int pci = testFrame.data[0];
    int sid = testFrame.data[1];
    /* simulated frame_data for valid single frame response */
    std::vector<uint8_t> frame_data = {0x03, 0x77};
    bool isMultiFrame = false;

    /* redirect stdout to capture the output */
    testing::internal::CaptureStdout();
    handler.processFrameData(testFrame.can_id, sid, frame_data, isMultiFrame);
    std::string output = testing::internal::GetCapturedStdout();
    /* check if the correct message is printed */
    EXPECT_EQ(output, "Response from RequestTransferExit received.\n");
}

/* Test for handleFrame with a valid single frame - ResponseRequestTransferExit SID*/
TEST_F(HandleFramesTest, ResponseRequestTransferExitSingTest){
    /* Set an arbitrary CAN ID */
    testFrame.can_id= 0;
    /* Set the Data Length Code to at least 2 to access data[1] */
    testFrame.can_dlc = 2;
    testFrame.data[0] = 0x03;
    testFrame.data[1] = 0x77;
    int pci = testFrame.data[0];
    int sid = testFrame.data[1];
    /* simulated frame_data for valid single frame response */
    std::vector<uint8_t> frame_data = {0x03, 0x77};
    bool isMultiFrame = false;

    /* redirect stdout to capture the output */
    testing::internal::CaptureStdout();
    handler.handleFrame(testFrame);
    std::string output = testing::internal::GetCapturedStdout();
    /* check if the correct message is printed */
    EXPECT_EQ(output, "Single Frame received:\n\nResponse from RequestTransferExit received.\n");
}

/* Test for processFrameData with a valid ResponseRequestUpdateStatus frame */
TEST_F(HandleFramesTest, ResponseRequestUpdateStatusValTest){
    /* Set an arbitrary CAN ID */
    testFrame.can_id= 0;
    /* Set the Data Length Code to at least 2 to access data[1] */
    testFrame.can_dlc = 2;
    testFrame.data[0] = 0x03;
    testFrame.data[1] = 0x72;
    int pci = testFrame.data[0];
    int sid = testFrame.data[1];
    /* simulated frame_data for valid single frame response */
    std::vector<uint8_t> frame_data = {0x03, 0x72};
    bool isMultiFrame = false;

    /* redirect stdout to capture the output */
    testing::internal::CaptureStdout();
    handler.processFrameData(testFrame.can_id, sid, frame_data, isMultiFrame);
    std::string output = testing::internal::GetCapturedStdout();
    /* check if the correct message is printed */
    EXPECT_EQ(output, "Response from RequestUpdateStatus received.\n");
}

/* Test for handleFrame with a valid single frame - ResponseRequestUpdateStatus SID*/
TEST_F(HandleFramesTest, ResponseRequestUpdateStatusSingTest){
    /* Set an arbitrary CAN ID */
    testFrame.can_id= 0;
    /* Set the Data Length Code to at least 2 to access data[1] */
    testFrame.can_dlc = 2;
    testFrame.data[0] = 0x03;
    testFrame.data[1] = 0x72;
    int pci = testFrame.data[0];
    int sid = testFrame.data[1];
    /* simulated frame_data for valid single frame response */
    std::vector<uint8_t> frame_data = {0x03, 0x72};
    bool isMultiFrame = false;

    /* redirect stdout to capture the output */
    testing::internal::CaptureStdout();
    handler.handleFrame(testFrame);
    std::string output = testing::internal::GetCapturedStdout();
    /* check if the correct message is printed */
    EXPECT_EQ(output, "Single Frame received:\n\nResponse from RequestUpdateStatus received.\n");
}

/* Test for processFrameData with a valid UnknownFrame frame */
TEST_F(HandleFramesTest, UnknownFrameValTest){
    /* Set an arbitrary CAN ID */
    testFrame.can_id= 0;
    /* Set the Data Length Code to at least 2 to access data[1] */
    testFrame.can_dlc = 2;
    testFrame.data[0] = 0x03;
    testFrame.data[1] = 0x99;
    int pci = testFrame.data[0];
    int sid = testFrame.data[1];
    /* simulated frame_data for valid single frame response */
    std::vector<uint8_t> frame_data = {0x03, 0x99};
    bool isMultiFrame = false;

    /* redirect stdout to capture the output */
    testing::internal::CaptureStdout();
    handler.processFrameData(testFrame.can_id, sid, frame_data, isMultiFrame);
    std::string output = testing::internal::GetCapturedStdout();
    /* check if the correct message is printed */
    EXPECT_EQ(output, "Unknown request/response received.\n");
}

/* Test for handleFrame with a valid single frame - ResponseRequestUpdateStatus SID*/
TEST_F(HandleFramesTest, UnknownFrameSingTest){
    /* Set an arbitrary CAN ID */
    testFrame.can_id= 0;
    /* Set the Data Length Code to at least 2 to access data[1] */
    testFrame.can_dlc = 2;
    testFrame.data[0] = 0x03;
    testFrame.data[1] = 0x99;
    int pci = testFrame.data[0];
    int sid = testFrame.data[1];
    /* simulated frame_data for valid single frame response */
    std::vector<uint8_t> frame_data = {0x03, 0x99};
    bool isMultiFrame = false;

    /* redirect stdout to capture the output */
    testing::internal::CaptureStdout();
    handler.handleFrame(testFrame);
    std::string output = testing::internal::GetCapturedStdout();
    /* check if the correct message is printed */
    EXPECT_EQ(output, "Single Frame received:\n\nUnknown request/response received.\n");
}

int main(int argc, char **argv)
{
testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}