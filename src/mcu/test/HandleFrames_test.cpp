#include "gtest/gtest.h"
#include "../include/HandleFrames.h"
#include <linux/can.h>
#include "../include/MCULogger.h"
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
    EXPECT_NE(output.find("DiagnosticSessionControl called."), std::string::npos);
    EXPECT_EQ(sid, testFrame.data[1]);
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
    EXPECT_NE(output.find("Single Frame received:"), std::string::npos);
    EXPECT_NE(output.find("DiagnosticSessionControl called."), std::string::npos);
}

/** Test for handleFrameprocessNrc called in processFrameData with a valid single frame for negative response
for DiagnosticSessionControl SID - Service not supported */
TEST_F(HandleFramesTest, DiagnosticSessionControlServiceNegTest){
    /* Set an arbitrary CAN ID */
    testFrame.can_id= 0;
    /* Set the Data Length Code to at least 4 */
    testFrame.can_dlc = 4;
    testFrame.data[0] = 0x03;
    testFrame.data[1] = 0x7F;
    testFrame.data[2] = 0x10;    
    int pci = testFrame.data[0];
    int sid = testFrame.data[2];
    
    /* simulated frame_data for valid single frame response */
    std::vector<uint8_t> frame_data = {0x03, 0X7F, 0x10, 0x11};
    int nrc = int(frame_data[3]);
    bool isMultiFrame = false;

    /* redirect stdout to capture the output */
    testing::internal::CaptureStdout();       
    /* handler.processNrc(testFrame.can_id, sid, nrc); */
    handler.processFrameData(testFrame.can_id, sid, frame_data, isMultiFrame);    
    std::string output = testing::internal::GetCapturedStdout();
    /* check if the correct message is printed */
    EXPECT_NE(output.find("Error: Service not supported for service: 10"), std::string::npos); 
}

/** Test for handleFrame method when frame is negative response 
 * - sid = frame.data[2];  */
TEST_F(HandleFramesTest, DiagnosticSessionControlSidNegTest){
    /* Set an arbitrary CAN ID */
    testFrame.can_id= 0;
    /* Set the Data Length Code to at least 4 */
    testFrame.can_dlc = 4;
    testFrame.data[0] = 0x03;
    testFrame.data[1] = 0x7F;
    testFrame.data[2] = 0x10;    
    int pci = testFrame.data[0];
    int sid = testFrame.data[2];
    
    /* simulated frame_data for valid single frame response */
    std::vector<uint8_t> frame_data = {0x03, 0X7F, 0x10, 0x11};
    int nrc = int(frame_data[3]);
    bool isMultiFrame = false;

    /* redirect stdout to capture the output */
    testing::internal::CaptureStdout();       
    /* handler.processNrc(testFrame.can_id, sid, nrc); */
    handler.handleFrame(testFrame);    
    std::string output = testing::internal::GetCapturedStdout();
    /* check if the correct message is printed */    
    EXPECT_EQ(sid, testFrame.data[2]);
}

/** Test for handleFrameprocessNrc called in processFrameData with a valid single frame for negative response
for DiagnosticSessionControl SID - Incorrect message length or invalid format */
TEST_F(HandleFramesTest, DiagnosticSessionControlIncorrectNegTest){
    /* Set an arbitrary CAN ID */
    testFrame.can_id= 0;
    /* Set the Data Length Code to at least 4 */
    testFrame.can_dlc = 4;
    testFrame.data[0] = 0x03;
    testFrame.data[1] = 0x7F;
    testFrame.data[2] = 0x10;    
    int pci = testFrame.data[0];
    int sid = testFrame.data[2];
    
    /* simulated frame_data for valid single frame response */
    std::vector<uint8_t> frame_data = {0x03, 0X7F, 0x10, 0x13};
    int nrc = int(frame_data[3]);
    bool isMultiFrame = false;

    /* redirect stdout to capture the output */
    testing::internal::CaptureStdout();      
    /* handler.processNrc(testFrame.can_id, sid, nrc); */
    handler.processFrameData(testFrame.can_id, sid, frame_data, isMultiFrame);      
    std::string output = testing::internal::GetCapturedStdout();
    /* check if the correct message is printed */
    EXPECT_NE(output.find("Error: Incorrect message length or invalid format for service: 10"), std::string::npos); 
}

/** Test for handleFrameprocessNrc called in processFrameData with a valid single frame for negative response
for DiagnosticSessionControl SID - Response too long */
TEST_F(HandleFramesTest, DiagnosticSessionControlRespLongNegTest){
    /* Set an arbitrary CAN ID */
    testFrame.can_id= 0;
    /* Set the Data Length Code to at least 4 */
    testFrame.can_dlc = 4;
    testFrame.data[0] = 0x03;
    testFrame.data[1] = 0x7F;
    testFrame.data[2] = 0x10;    
    int pci = testFrame.data[0];
    int sid = testFrame.data[2];
    
    /* simulated frame_data for valid single frame response */
    std::vector<uint8_t> frame_data = {0x03, 0X7F, 0x10, 0x14};
    int nrc = int(frame_data[3]);
    bool isMultiFrame = false;

    /* redirect stdout to capture the output */
    testing::internal::CaptureStdout();      
    /* handler.processNrc(testFrame.can_id, sid, nrc); */   
    handler.processFrameData(testFrame.can_id, sid, frame_data, isMultiFrame);    
    std::string output = testing::internal::GetCapturedStdout();
    /* check if the correct message is printed */
    EXPECT_NE(output.find("Error: Response too long for service: 10"), std::string::npos);
}

/** Test for handleFrameprocessNrc called in processFrameData with a valid single frame for negative response
for DiagnosticSessionControl SID - No response from subnet component */
TEST_F(HandleFramesTest, DiagnosticSessionControlNoRespNegTest){
    /* Set an arbitrary CAN ID */
    testFrame.can_id= 0;
    /* Set the Data Length Code to at least 4 */
    testFrame.can_dlc = 4;
    testFrame.data[0] = 0x03;
    testFrame.data[1] = 0x7F;
    testFrame.data[2] = 0x10;    
    int pci = testFrame.data[0];
    int sid = testFrame.data[2];
    
    /* simulated frame_data for valid single frame response */
    std::vector<uint8_t> frame_data = {0x03, 0X7F, 0x10, 0x25};
    int nrc = int(frame_data[3]);
    bool isMultiFrame = false;

    /* redirect stdout to capture the output */
    testing::internal::CaptureStdout();      
    /* handler.processNrc(testFrame.can_id, sid, nrc); */  
    handler.processFrameData(testFrame.can_id, sid, frame_data, isMultiFrame);     
    std::string output = testing::internal::GetCapturedStdout();
    /* check if the correct message is printed */
    EXPECT_NE(output.find("Error: No response from subnet component for service: 10"), std::string::npos);
}

/** Test for handleFrameprocessNrc called in processFrameData with a valid single frame for negative response
for DiagnosticSessionControl SID - Authentication failed */
TEST_F(HandleFramesTest, DiagnosticSessionControlNoAuthNegTest){
    /* Set an arbitrary CAN ID */
    testFrame.can_id= 0;
    /* Set the Data Length Code to at least 4 */
    testFrame.can_dlc = 4;
    testFrame.data[0] = 0x03;
    testFrame.data[1] = 0x7F;
    testFrame.data[2] = 0x10;    
    int pci = testFrame.data[0];
    int sid = testFrame.data[2];
    
    /* simulated frame_data for valid single frame response */
    std::vector<uint8_t> frame_data = {0x03, 0X7F, 0x10, 0x34};
    int nrc = int(frame_data[3]);
    bool isMultiFrame = false;

    /* redirect stdout to capture the output */
    testing::internal::CaptureStdout();      
    /* handler.processNrc(testFrame.can_id, sid, nrc); */  
    handler.processFrameData(testFrame.can_id, sid, frame_data, isMultiFrame);     
    std::string output = testing::internal::GetCapturedStdout();
    /* check if the correct message is printed */
    EXPECT_NE(output.find("Error: Authentication failed for service: 10"), std::string::npos);
}

/** Test for handleFrameprocessNrc called in processFrameData with a valid single frame for negative response
for DiagnosticSessionControl SID - Resource temporarily unavailable */
TEST_F(HandleFramesTest, DiagnosticSessionControlResUnNegTest){
    /* Set an arbitrary CAN ID */
    testFrame.can_id= 0;
    /* Set the Data Length Code to at least 4 */
    testFrame.can_dlc = 4;
    testFrame.data[0] = 0x03;
    testFrame.data[1] = 0x7F;
    testFrame.data[2] = 0x10;    
    int pci = testFrame.data[0];
    int sid = testFrame.data[2];
    
    /* simulated frame_data for valid single frame response */
    std::vector<uint8_t> frame_data = {0x03, 0X7F, 0x10, 0x94};
    int nrc = int(frame_data[3]);
    bool isMultiFrame = false;

    /* redirect stdout to capture the output */
    testing::internal::CaptureStdout();      
    /* handler.processNrc(testFrame.can_id, sid, nrc); */    
    handler.processFrameData(testFrame.can_id, sid, frame_data, isMultiFrame);     
    std::string output = testing::internal::GetCapturedStdout();
    /* check if the correct message is printed */
    EXPECT_NE(output.find("Error: Resource temporarily unavailable for service: 10"), std::string::npos);
}

/** Test for handleFrameprocessNrc called in processFrameData with a valid single frame for negative response
for DiagnosticSessionControl SID - Upload download not accepted */
TEST_F(HandleFramesTest, DiagnosticSessionControlUDNotAccNegTest){
    /* Set an arbitrary CAN ID */
    testFrame.can_id= 0;
    /* Set the Data Length Code to at least 4 */
    testFrame.can_dlc = 4;
    testFrame.data[0] = 0x03;
    testFrame.data[1] = 0x7F;
    testFrame.data[2] = 0x10;    
    int pci = testFrame.data[0];
    int sid = testFrame.data[2];
    
    /* simulated frame_data for valid single frame response */
    std::vector<uint8_t> frame_data = {0x03, 0X7F, 0x10, 0x70};
    int nrc = int(frame_data[3]);
    bool isMultiFrame = false;

    /* redirect stdout to capture the output */
    testing::internal::CaptureStdout();      
    /* handler.processNrc(testFrame.can_id, sid, nrc); */   
    handler.processFrameData(testFrame.can_id, sid, frame_data, isMultiFrame);      
    std::string output = testing::internal::GetCapturedStdout();
    /* check if the correct message is printed */
    EXPECT_NE(output.find("Error: Upload download not accepted for service: 10"), std::string::npos);
}

/** Test for handleFrameprocessNrc called in processFrameData with a valid single frame for negative response
for DiagnosticSessionControl SID - Transfer data suspended */
TEST_F(HandleFramesTest, DiagnosticSessionControlTDataSuspNegTest){
    /* Set an arbitrary CAN ID */
    testFrame.can_id= 0;
    /* Set the Data Length Code to at least 4 */
    testFrame.can_dlc = 4;
    testFrame.data[0] = 0x03;
    testFrame.data[1] = 0x7F;
    testFrame.data[2] = 0x10;    
    int pci = testFrame.data[0];
    int sid = testFrame.data[2];
    
    /* simulated frame_data for valid single frame response */
    std::vector<uint8_t> frame_data = {0x03, 0X7F, 0x10, 0x71};
    int nrc = int(frame_data[3]);
    bool isMultiFrame = false;

    /* redirect stdout to capture the output */
    testing::internal::CaptureStdout();      
    /* handler.processNrc(testFrame.can_id, sid, nrc); */      
    handler.processFrameData(testFrame.can_id, sid, frame_data, isMultiFrame);   
    std::string output = testing::internal::GetCapturedStdout();
    /* check if the correct message is printed */
    EXPECT_NE(output.find("Error: Transfer data suspended for service: 10"), std::string::npos);
}

/** Test for handleFrameprocessNrc called in processFrameData with a valid single frame for negative response
for DiagnosticSessionControl SID - Transfer data suspended */
TEST_F(HandleFramesTest, DiagnosticSessionControlDefaultNegTest){
    /* Set an arbitrary CAN ID */
    testFrame.can_id= 0;
    /* Set the Data Length Code to at least 4 */
    testFrame.can_dlc = 4;
    testFrame.data[0] = 0x03;
    testFrame.data[1] = 0x7F;
    testFrame.data[2] = 0x10;    
    int pci = testFrame.data[0];
    int sid = testFrame.data[2];
    
    /* simulated frame_data for valid single frame response */
    std::vector<uint8_t> frame_data = {0x03, 0X7F, 0x10, 0x99};
    int nrc = int(frame_data[3]);
    bool isMultiFrame = false;

    /* redirect stdout to capture the output */
    testing::internal::CaptureStdout();      
    /* handler.processNrc(testFrame.can_id, sid, nrc); */   
    handler.processFrameData(testFrame.can_id, sid, frame_data, isMultiFrame);      
    std::string output = testing::internal::GetCapturedStdout();
    /* check if the correct message is printed */
    EXPECT_NE(output.find("Error: Unknown negative response code for service: 10"), std::string::npos);
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
    EXPECT_NE(output.find("EcuReset called."), std::string::npos);
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
    EXPECT_NE(output.find("Single Frame received:"), std::string::npos);
    EXPECT_NE(output.find("EcuReset called."), std::string::npos);
}

/** Test for handleFrameprocessNrc called in processFrameData with a valid single frame for negative response
for EcuReset SID - Service not supported */
TEST_F(HandleFramesTest, EcuResetServiceNegTest){
    /* Set an arbitrary CAN ID */
    testFrame.can_id= 0;
    /* Set the Data Length Code to at least 4 */
    testFrame.can_dlc = 4;
    testFrame.data[0] = 0x03;
    testFrame.data[1] = 0x7F;
    testFrame.data[2] = 0x11;    
    int pci = testFrame.data[0];
    int sid = testFrame.data[2];
    
    /* simulated frame_data for valid single frame response */
    std::vector<uint8_t> frame_data = {0x03, 0X7F, 0x11, 0x11};
    int nrc = int(frame_data[3]);
    bool isMultiFrame = false;

    /* redirect stdout to capture the output */
    testing::internal::CaptureStdout();       
    /* handler.processNrc(testFrame.can_id, sid, nrc); */    
    handler.processFrameData(testFrame.can_id, sid, frame_data, isMultiFrame); 
    std::string output = testing::internal::GetCapturedStdout();
    /* check if the correct message is printed */
    EXPECT_NE(output.find("Error: Service not supported for service: 11"), std::string::npos);
    EXPECT_EQ(sid, testFrame.data[2]);
}

/** Test for handleFrameprocessNrc called in processFrameData with a valid single frame for negative response
for EcuReset SID - Incorrect message length or invalid format */
TEST_F(HandleFramesTest, EcuResetIncorrectNegTest){
    /* Set an arbitrary CAN ID */
    testFrame.can_id= 0;
    /* Set the Data Length Code to at least 4 */
    testFrame.can_dlc = 4;
    testFrame.data[0] = 0x03;
    testFrame.data[1] = 0x7F;
    testFrame.data[2] = 0x11;    
    int pci = testFrame.data[0];
    int sid = testFrame.data[2];
    
    /* simulated frame_data for valid single frame response */
    std::vector<uint8_t> frame_data = {0x03, 0X7F, 0x11, 0x13};
    int nrc = int(frame_data[3]);
    bool isMultiFrame = false;

    /* redirect stdout to capture the output */
    testing::internal::CaptureStdout();      
    /* handler.processNrc(testFrame.can_id, sid, nrc); */    
    handler.processFrameData(testFrame.can_id, sid, frame_data, isMultiFrame);      
    std::string output = testing::internal::GetCapturedStdout();
    /* check if the correct message is printed */
    EXPECT_NE(output.find("Error: Incorrect message length or invalid format for service: 11"), std::string::npos);
}

/** Test for handleFrameprocessNrc called in processFrameData with a valid single frame for negative response
for EcuReset SID - Response too long */
TEST_F(HandleFramesTest, EcuResetRespLongNegTest){
    /* Set an arbitrary CAN ID */
    testFrame.can_id= 0;
    /* Set the Data Length Code to at least 4 */
    testFrame.can_dlc = 4;
    testFrame.data[0] = 0x03;
    testFrame.data[1] = 0x7F;
    testFrame.data[2] = 0x11;    
    int pci = testFrame.data[0];
    int sid = testFrame.data[2];
    
    /* simulated frame_data for valid single frame response */
    std::vector<uint8_t> frame_data = {0x03, 0X7F, 0x11, 0x14};
    int nrc = int(frame_data[3]);
    bool isMultiFrame = false;

    /* redirect stdout to capture the output */
    testing::internal::CaptureStdout();      
    /* handler.processNrc(testFrame.can_id, sid, nrc); */    
    handler.processFrameData(testFrame.can_id, sid, frame_data, isMultiFrame);      
    std::string output = testing::internal::GetCapturedStdout();
    /* check if the correct message is printed */
    EXPECT_NE(output.find("Error: Response too long for service: 11"), std::string::npos);
}

/** Test for handleFrameprocessNrc called in processFrameData with a valid single frame for negative response
for EcuReset SID - No response from subnet component */
TEST_F(HandleFramesTest, EcuResetNoRespNegTest){
    /* Set an arbitrary CAN ID */
    testFrame.can_id= 0;
    /* Set the Data Length Code to at least 4 */
    testFrame.can_dlc = 4;
    testFrame.data[0] = 0x03;
    testFrame.data[1] = 0x7F;
    testFrame.data[2] = 0x11;    
    int pci = testFrame.data[0];
    int sid = testFrame.data[2];
    
    /* simulated frame_data for valid single frame response */
    std::vector<uint8_t> frame_data = {0x03, 0X7F, 0x11, 0x25};
    int nrc = int(frame_data[3]);
    bool isMultiFrame = false;

    /* redirect stdout to capture the output */
    testing::internal::CaptureStdout();      
   /* handler.processNrc(testFrame.can_id, sid, nrc); */    
    handler.processFrameData(testFrame.can_id, sid, frame_data, isMultiFrame);    
    std::string output = testing::internal::GetCapturedStdout();
    /* check if the correct message is printed */
    EXPECT_NE(output.find("Error: No response from subnet component for service: 11"), std::string::npos);
}

/** Test for handleFrameprocessNrc called in processFrameData with a valid single frame for negative response
for EcuReset SID - Authentication failed */
TEST_F(HandleFramesTest, EcuResetNoAuthNegTest){
    /* Set an arbitrary CAN ID */
    testFrame.can_id= 0;
    /* Set the Data Length Code to at least 4 */
    testFrame.can_dlc = 4;
    testFrame.data[0] = 0x03;
    testFrame.data[1] = 0x7F;
    testFrame.data[2] = 0x11;    
    int pci = testFrame.data[0];
    int sid = testFrame.data[2];
    
    /* simulated frame_data for valid single frame response */
    std::vector<uint8_t> frame_data = {0x03, 0X7F, 0x11, 0x34};
    int nrc = int(frame_data[3]);
    bool isMultiFrame = false;

    /* redirect stdout to capture the output */
    testing::internal::CaptureStdout();      
    /* handler.processNrc(testFrame.can_id, sid, nrc); */    
    handler.processFrameData(testFrame.can_id, sid, frame_data, isMultiFrame);       
    std::string output = testing::internal::GetCapturedStdout();
    /* check if the correct message is printed */
    EXPECT_NE(output.find("Error: Authentication failed for service: 11"), std::string::npos);
}

/** Test for handleFrameprocessNrc called in processFrameData with a valid single frame for negative response
for EcuReset SID - Resource temporarily unavailable */
TEST_F(HandleFramesTest, EcuResetResUnNegTest){
    /* Set an arbitrary CAN ID */
    testFrame.can_id= 0;
    /* Set the Data Length Code to at least 4 */
    testFrame.can_dlc = 4;
    testFrame.data[0] = 0x03;
    testFrame.data[1] = 0x7F;
    testFrame.data[2] = 0x11;    
    int pci = testFrame.data[0];
    int sid = testFrame.data[2];
    
    /* simulated frame_data for valid single frame response */
    std::vector<uint8_t> frame_data = {0x03, 0X7F, 0x11, 0x94};
    int nrc = int(frame_data[3]);
    bool isMultiFrame = false;

    /* redirect stdout to capture the output */
    testing::internal::CaptureStdout();      
    /* handler.processNrc(testFrame.can_id, sid, nrc); */    
    handler.processFrameData(testFrame.can_id, sid, frame_data, isMultiFrame);      
    std::string output = testing::internal::GetCapturedStdout();
    /* check if the correct message is printed */
    EXPECT_NE(output.find("Error: Resource temporarily unavailable for service: 11"), std::string::npos);
}

/** Test for handleFrameprocessNrc called in processFrameData with a valid single frame for negative response
for EcuReset SID - Upload download not accepted */
TEST_F(HandleFramesTest, EcuResetUDNotAccNegTest){
    /* Set an arbitrary CAN ID */
    testFrame.can_id= 0;
    /* Set the Data Length Code to at least 4 */
    testFrame.can_dlc = 4;
    testFrame.data[0] = 0x03;
    testFrame.data[1] = 0x7F;
    testFrame.data[2] = 0x11;    
    int pci = testFrame.data[0];
    int sid = testFrame.data[2];
    
    /* simulated frame_data for valid single frame response */
    std::vector<uint8_t> frame_data = {0x03, 0X7F, 0x11, 0x70};
    int nrc = int(frame_data[3]);
    bool isMultiFrame = false;

    /* redirect stdout to capture the output */
    testing::internal::CaptureStdout();      
   /* handler.processNrc(testFrame.can_id, sid, nrc); */    
    handler.processFrameData(testFrame.can_id, sid, frame_data, isMultiFrame);   
    std::string output = testing::internal::GetCapturedStdout();
    /* check if the correct message is printed */
    EXPECT_NE(output.find("Error: Upload download not accepted for service: 11"), std::string::npos);
}

/** Test for handleFrameprocessNrc called in processFrameData with a valid single frame for negative response
for EcuReset SID - Transfer data suspended */
TEST_F(HandleFramesTest, EcuResetTDataSuspNegTest){
    /* Set an arbitrary CAN ID */
    testFrame.can_id= 0;
    /* Set the Data Length Code to at least 4 */
    testFrame.can_dlc = 4;
    testFrame.data[0] = 0x03;
    testFrame.data[1] = 0x7F;
    testFrame.data[2] = 0x11;    
    int pci = testFrame.data[0];
    int sid = testFrame.data[2];
    
    /* simulated frame_data for valid single frame response */
    std::vector<uint8_t> frame_data = {0x03, 0X7F, 0x11, 0x71};
    int nrc = int(frame_data[3]);
    bool isMultiFrame = false;

    /* redirect stdout to capture the output */
    testing::internal::CaptureStdout();      
    /* handler.processNrc(testFrame.can_id, sid, nrc); */    
    handler.processFrameData(testFrame.can_id, sid, frame_data, isMultiFrame);       
    std::string output = testing::internal::GetCapturedStdout();
    /* check if the correct message is printed */
    EXPECT_NE(output.find("Error: Transfer data suspended for service: 11"), std::string::npos);
}

/** Test for handleFrameprocessNrc called in processFrameData with a valid single frame for negative response
for EcuReset SID - Transfer data suspended */
TEST_F(HandleFramesTest, EcuResetDefaultNegTest){
    /* Set an arbitrary CAN ID */
    testFrame.can_id= 0;
    /* Set the Data Length Code to at least 4 */
    testFrame.can_dlc = 4;
    testFrame.data[0] = 0x03;
    testFrame.data[1] = 0x7F;
    testFrame.data[2] = 0x11;    
    int pci = testFrame.data[0];
    int sid = testFrame.data[2];
    
    /* simulated frame_data for valid single frame response */
    std::vector<uint8_t> frame_data = {0x03, 0X7F, 0x11, 0x99};
    int nrc = int(frame_data[3]);
    bool isMultiFrame = false;

    /* redirect stdout to capture the output */
    testing::internal::CaptureStdout();      
    /* handler.processNrc(testFrame.can_id, sid, nrc); */    
    handler.processFrameData(testFrame.can_id, sid, frame_data, isMultiFrame);    
    std::string output = testing::internal::GetCapturedStdout();
    /* check if the correct message is printed */
    EXPECT_NE(output.find("Error: Unknown negative response code for service: 11"), std::string::npos);
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
    EXPECT_NE(output.find("SecurityAccess called."), std::string::npos);
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
    EXPECT_NE(output.find("Single Frame received:"), std::string::npos);
    EXPECT_NE(output.find("SecurityAccess called."), std::string::npos);
}

/** Test for handleFrameprocessNrc called in processFrameData with a valid single frame for negative response
for SecurityAccess SID - Service not supported */
TEST_F(HandleFramesTest, SecurityAccessNegTest){
    /* Set an arbitrary CAN ID */
    testFrame.can_id= 0;
    /* Set the Data Length Code to at least 4 */
    testFrame.can_dlc = 4;
    testFrame.data[0] = 0x03;
    testFrame.data[1] = 0x7F;
    testFrame.data[2] = 0x27;    
    int pci = testFrame.data[0];
    int sid = testFrame.data[2];
    
    /* simulated frame_data for valid single frame response */
    std::vector<uint8_t> frame_data = {0x03, 0X7F, 0x27, 0x11};
    int nrc = int(frame_data[3]);
    bool isMultiFrame = false;

    /* redirect stdout to capture the output */
    testing::internal::CaptureStdout();       
    /* handler.processNrc(testFrame.can_id, sid, nrc); */    
    handler.processFrameData(testFrame.can_id, sid, frame_data, isMultiFrame); 
    std::string output = testing::internal::GetCapturedStdout();
    /* check if the correct message is printed */
    EXPECT_NE(output.find("Error: Service not supported for service: 27"), std::string::npos);   
}

/** Test for handleFrameprocessNrc called in processFrameData with a valid single frame for negative response
for SecurityAccess SID - Incorrect message length or invalid format */
TEST_F(HandleFramesTest, SecurityAccessIncorrectNegTest){
    /* Set an arbitrary CAN ID */
    testFrame.can_id= 0;
    /* Set the Data Length Code to at least 4 */
    testFrame.can_dlc = 4;
    testFrame.data[0] = 0x03;
    testFrame.data[1] = 0x7F;
    testFrame.data[2] = 0x27;    
    int pci = testFrame.data[0];
    int sid = testFrame.data[2];
    
    /* simulated frame_data for valid single frame response */
    std::vector<uint8_t> frame_data = {0x03, 0X7F, 0x27, 0x13};
    int nrc = int(frame_data[3]);
    bool isMultiFrame = false;

    /* redirect stdout to capture the output */
    testing::internal::CaptureStdout();      
    /* handler.processNrc(testFrame.can_id, sid, nrc); */    
    handler.processFrameData(testFrame.can_id, sid, frame_data, isMultiFrame);      
    std::string output = testing::internal::GetCapturedStdout();
    /* check if the correct message is printed */
    EXPECT_NE(output.find("Error: Incorrect message length or invalid format for service: 27"), std::string::npos); 
}

/** Test for handleFrameprocessNrc called in processFrameData with a valid single frame for negative response
for SecurityAccess SID - Response too long */
TEST_F(HandleFramesTest, SecurityAccessRespLongNegTest){
    /* Set an arbitrary CAN ID */
    testFrame.can_id= 0;
    /* Set the Data Length Code to at least 4 */
    testFrame.can_dlc = 4;
    testFrame.data[0] = 0x03;
    testFrame.data[1] = 0x7F;
    testFrame.data[2] = 0x27;    
    int pci = testFrame.data[0];
    int sid = testFrame.data[2];
    
    /* simulated frame_data for valid single frame response */
    std::vector<uint8_t> frame_data = {0x03, 0X7F, 0x27, 0x14};
    int nrc = int(frame_data[3]);
    bool isMultiFrame = false;

    /* redirect stdout to capture the output */
    testing::internal::CaptureStdout();      
    /* handler.processNrc(testFrame.can_id, sid, nrc); */    
    handler.processFrameData(testFrame.can_id, sid, frame_data, isMultiFrame);      
    std::string output = testing::internal::GetCapturedStdout();
    /* check if the correct message is printed */
    EXPECT_NE(output.find("Error: Response too long for service: 27"), std::string::npos); 
}

/** Test for handleFrameprocessNrc called in processFrameData with a valid single frame for negative response
for SecurityAccess SID - No response from subnet component */
TEST_F(HandleFramesTest, SecurityAccessNoRespNegTest){
    /* Set an arbitrary CAN ID */
    testFrame.can_id= 0;
    /* Set the Data Length Code to at least 4 */
    testFrame.can_dlc = 4;
    testFrame.data[0] = 0x03;
    testFrame.data[1] = 0x7F;
    testFrame.data[2] = 0x27;    
    int pci = testFrame.data[0];
    int sid = testFrame.data[2];
    
    /* simulated frame_data for valid single frame response */
    std::vector<uint8_t> frame_data = {0x03, 0X7F, 0x27, 0x25};
    int nrc = int(frame_data[3]);
    bool isMultiFrame = false;

    /* redirect stdout to capture the output */
    testing::internal::CaptureStdout();      
   /* handler.processNrc(testFrame.can_id, sid, nrc); */    
    handler.processFrameData(testFrame.can_id, sid, frame_data, isMultiFrame);    
    std::string output = testing::internal::GetCapturedStdout();
    /* check if the correct message is printed */
    EXPECT_NE(output.find("Error: No response from subnet component for service: 27"), std::string::npos); 
}

/** Test for handleFrameprocessNrc called in processFrameData with a valid single frame for negative response
for SecurityAccess SID - Authentication failed */
TEST_F(HandleFramesTest, SecurityAccessNoAuthNegTest){
    /* Set an arbitrary CAN ID */
    testFrame.can_id= 0;
    /* Set the Data Length Code to at least 4 */
    testFrame.can_dlc = 4;
    testFrame.data[0] = 0x03;
    testFrame.data[1] = 0x7F;
    testFrame.data[2] = 0x27;    
    int pci = testFrame.data[0];
    int sid = testFrame.data[2];
    
    /* simulated frame_data for valid single frame response */
    std::vector<uint8_t> frame_data = {0x03, 0X7F, 0x27, 0x34};
    int nrc = int(frame_data[3]);
    bool isMultiFrame = false;

    /* redirect stdout to capture the output */
    testing::internal::CaptureStdout();      
    /* handler.processNrc(testFrame.can_id, sid, nrc); */    
    handler.processFrameData(testFrame.can_id, sid, frame_data, isMultiFrame);       
    std::string output = testing::internal::GetCapturedStdout();
    /* check if the correct message is printed */
    EXPECT_NE(output.find("Error: Authentication failed for service: 27"), std::string::npos); 
}

/** Test for handleFrameprocessNrc called in processFrameData with a valid single frame for negative response
for SecurityAccess SID - Resource temporarily unavailable */
TEST_F(HandleFramesTest, SecurityAccessResUnNegTest){
    /* Set an arbitrary CAN ID */
    testFrame.can_id= 0;
    /* Set the Data Length Code to at least 4 */
    testFrame.can_dlc = 4;
    testFrame.data[0] = 0x03;
    testFrame.data[1] = 0x7F;
    testFrame.data[2] = 0x27;    
    int pci = testFrame.data[0];
    int sid = testFrame.data[2];
    
    /* simulated frame_data for valid single frame response */
    std::vector<uint8_t> frame_data = {0x03, 0X7F, 0x27, 0x94};
    int nrc = int(frame_data[3]);
    bool isMultiFrame = false;

    /* redirect stdout to capture the output */
    testing::internal::CaptureStdout();      
    /* handler.processNrc(testFrame.can_id, sid, nrc); */    
    handler.processFrameData(testFrame.can_id, sid, frame_data, isMultiFrame);      
    std::string output = testing::internal::GetCapturedStdout();
    /* check if the correct message is printed */
    EXPECT_NE(output.find("Error: Resource temporarily unavailable for service: 27"), std::string::npos); 
}

/** Test for handleFrameprocessNrc called in processFrameData with a valid single frame for negative response
for SecurityAccess SID - Upload download not accepted */
TEST_F(HandleFramesTest, SecurityAccessUDNotAccNegTest){
    /* Set an arbitrary CAN ID */
    testFrame.can_id= 0;
    /* Set the Data Length Code to at least 4 */
    testFrame.can_dlc = 4;
    testFrame.data[0] = 0x03;
    testFrame.data[1] = 0x7F;
    testFrame.data[2] = 0x27;    
    int pci = testFrame.data[0];
    int sid = testFrame.data[2];
    
    /* simulated frame_data for valid single frame response */
    std::vector<uint8_t> frame_data = {0x03, 0X7F, 0x27, 0x70};
    int nrc = int(frame_data[3]);
    bool isMultiFrame = false;

    /* redirect stdout to capture the output */
    testing::internal::CaptureStdout();      
   /* handler.processNrc(testFrame.can_id, sid, nrc); */    
    handler.processFrameData(testFrame.can_id, sid, frame_data, isMultiFrame);   
    std::string output = testing::internal::GetCapturedStdout();
    /* check if the correct message is printed */
    EXPECT_NE(output.find("Error: Upload download not accepted for service: 27"), std::string::npos);
}

/** Test for handleFrameprocessNrc called in processFrameData with a valid single frame for negative response
for SecurityAccess SID - Transfer data suspended */
TEST_F(HandleFramesTest, SecurityAccessTDataSuspNegTest){
    /* Set an arbitrary CAN ID */
    testFrame.can_id= 0;
    /* Set the Data Length Code to at least 4 */
    testFrame.can_dlc = 4;
    testFrame.data[0] = 0x03;
    testFrame.data[1] = 0x7F;
    testFrame.data[2] = 0x27;    
    int pci = testFrame.data[0];
    int sid = testFrame.data[2];
    
    /* simulated frame_data for valid single frame response */
    std::vector<uint8_t> frame_data = {0x03, 0X7F, 0x27, 0x71};
    int nrc = int(frame_data[3]);
    bool isMultiFrame = false;

    /* redirect stdout to capture the output */
    testing::internal::CaptureStdout();      
    /* handler.processNrc(testFrame.can_id, sid, nrc); */    
    handler.processFrameData(testFrame.can_id, sid, frame_data, isMultiFrame);       
    std::string output = testing::internal::GetCapturedStdout();
    /* check if the correct message is printed */
    EXPECT_NE(output.find("Error: Transfer data suspended for service: 27"), std::string::npos);

}

/** Test for handleFrameprocessNrc called in processFrameData with a valid single frame for negative response
for EcuReset SID - Transfer data suspended */
TEST_F(HandleFramesTest, SecurityAccessDefaultNegTest){
    /* Set an arbitrary CAN ID */
    testFrame.can_id= 0;
    /* Set the Data Length Code to at least 4 */
    testFrame.can_dlc = 4;
    testFrame.data[0] = 0x03;
    testFrame.data[1] = 0x7F;
    testFrame.data[2] = 0x27;    
    int pci = testFrame.data[0];
    int sid = testFrame.data[2];
    
    /* simulated frame_data for valid single frame response */
    std::vector<uint8_t> frame_data = {0x03, 0X7F, 0x27, 0x99};
    int nrc = int(frame_data[3]);
    bool isMultiFrame = false;

    /* redirect stdout to capture the output */
    testing::internal::CaptureStdout();      
    /* handler.processNrc(testFrame.can_id, sid, nrc); */    
    handler.processFrameData(testFrame.can_id, sid, frame_data, isMultiFrame);    
    std::string output = testing::internal::GetCapturedStdout();
    /* check if the correct message is printed */
    EXPECT_NE(output.find("Error: Unknown negative response code for service: 27"), std::string::npos);
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
    EXPECT_NE(output.find("Authentication called."), std::string::npos);
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
    EXPECT_NE(output.find("Single Frame received:"), std::string::npos);
    EXPECT_NE(output.find("Authentication called."), std::string::npos);
}

/** Test for handleFrameprocessNrc called in processFrameData with a valid single frame for negative response
for Authentication SID - Service not supported */
TEST_F(HandleFramesTest, AuthenticationServiceNegTest){
    /* Set an arbitrary CAN ID */
    testFrame.can_id= 0;
    /* Set the Data Length Code to at least 4 */
    testFrame.can_dlc = 4;
    testFrame.data[0] = 0x03;
    testFrame.data[1] = 0x7F;
    testFrame.data[2] = 0x29;    
    int pci = testFrame.data[0];
    int sid = testFrame.data[2];
    
    /* simulated frame_data for valid single frame response */
    std::vector<uint8_t> frame_data = {0x03, 0X7F, 0x29, 0x11};
    int nrc = int(frame_data[3]);
    bool isMultiFrame = false;

    /* redirect stdout to capture the output */
    testing::internal::CaptureStdout();       
    /* handler.processNrc(testFrame.can_id, sid, nrc); */
    handler.processFrameData(testFrame.can_id, sid, frame_data, isMultiFrame);    
    std::string output = testing::internal::GetCapturedStdout();
    /* check if the correct message is printed */
    EXPECT_NE(output.find("Error: Service not supported for service: 29"), std::string::npos);
    EXPECT_EQ(sid, testFrame.data[2]);
}

/** Test for handleFrameprocessNrc called in processFrameData with a valid single frame for negative response
for Authentication SID - Incorrect message length or invalid format */
TEST_F(HandleFramesTest, AuthenticationIncorrectNegTest){
    /* Set an arbitrary CAN ID */
    testFrame.can_id= 0;
    /* Set the Data Length Code to at least 4 */
    testFrame.can_dlc = 4;
    testFrame.data[0] = 0x03;
    testFrame.data[1] = 0x7F;
    testFrame.data[2] = 0x29;    
    int pci = testFrame.data[0];
    int sid = testFrame.data[2];
    
    /* simulated frame_data for valid single frame response */
    std::vector<uint8_t> frame_data = {0x03, 0X7F, 0x29, 0x13};
    int nrc = int(frame_data[3]);
    bool isMultiFrame = false;

    /* redirect stdout to capture the output */
    testing::internal::CaptureStdout();      
    /* handler.processNrc(testFrame.can_id, sid, nrc); */    
    handler.processFrameData(testFrame.can_id, sid, frame_data, isMultiFrame);      
    std::string output = testing::internal::GetCapturedStdout();
    /* check if the correct message is printed */
    EXPECT_NE(output.find("Error: Incorrect message length or invalid format for service: 29"), std::string::npos);
}

/** Test for handleFrameprocessNrc called in processFrameData with a valid single frame for negative response
for Authentication SID - Incorrect message length or invalid format */
TEST_F(HandleFramesTest, AuthenticationRespLongNegTest){
    /* Set an arbitrary CAN ID */
    testFrame.can_id= 0;
    /* Set the Data Length Code to at least 4 */
    testFrame.can_dlc = 4;
    testFrame.data[0] = 0x03;
    testFrame.data[1] = 0x7F;
    testFrame.data[2] = 0x29;    
    int pci = testFrame.data[0];
    int sid = testFrame.data[2];
    
    /* simulated frame_data for valid single frame response */
    std::vector<uint8_t> frame_data = {0x03, 0X7F, 0x29, 0x14};
    int nrc = int(frame_data[3]);
    bool isMultiFrame = false;

    /* redirect stdout to capture the output */
    testing::internal::CaptureStdout();      
    /* handler.processNrc(testFrame.can_id, sid, nrc); */    
    handler.processFrameData(testFrame.can_id, sid, frame_data, isMultiFrame);      
    std::string output = testing::internal::GetCapturedStdout();
    /* check if the correct message is printed */
    EXPECT_NE(output.find("Error: Response too long for service: 29"), std::string::npos);
}

/** Test for handleFrameprocessNrc called in processFrameData with a valid single frame for negative response
for Authentication SID - No response from subnet component */
TEST_F(HandleFramesTest, AuthenticationNoRespNegTest){
    /* Set an arbitrary CAN ID */
    testFrame.can_id= 0;
    /* Set the Data Length Code to at least 4 */
    testFrame.can_dlc = 4;
    testFrame.data[0] = 0x03;
    testFrame.data[1] = 0x7F;
    testFrame.data[2] = 0x29;    
    int pci = testFrame.data[0];
    int sid = testFrame.data[2];
    
    /* simulated frame_data for valid single frame response */
    std::vector<uint8_t> frame_data = {0x03, 0X7F, 0x29, 0x25};
    int nrc = int(frame_data[3]);
    bool isMultiFrame = false;

    /* redirect stdout to capture the output */
    testing::internal::CaptureStdout();      
   /* handler.processNrc(testFrame.can_id, sid, nrc); */    
    handler.processFrameData(testFrame.can_id, sid, frame_data, isMultiFrame);    
    std::string output = testing::internal::GetCapturedStdout();
    /* check if the correct message is printed */
    EXPECT_NE(output.find("Error: No response from subnet component for service: 29"), std::string::npos);
}

/** Test for handleFrameprocessNrc called in processFrameData with a valid single frame for negative response
for Authentication SID - Authentication failed */
TEST_F(HandleFramesTest, AuthenticationNoAuthNegTest){
    /* Set an arbitrary CAN ID */
    testFrame.can_id= 0;
    /* Set the Data Length Code to at least 4 */
    testFrame.can_dlc = 4;
    testFrame.data[0] = 0x03;
    testFrame.data[1] = 0x7F;
    testFrame.data[2] = 0x29;    
    int pci = testFrame.data[0];
    int sid = testFrame.data[2];
    
    /* simulated frame_data for valid single frame response */
    std::vector<uint8_t> frame_data = {0x03, 0X7F, 0x29, 0x34};
    int nrc = int(frame_data[3]);
    bool isMultiFrame = false;

    /* redirect stdout to capture the output */
    testing::internal::CaptureStdout();      
    /* handler.processNrc(testFrame.can_id, sid, nrc); */    
    handler.processFrameData(testFrame.can_id, sid, frame_data, isMultiFrame);       
    std::string output = testing::internal::GetCapturedStdout();
    /* check if the correct message is printed */
    EXPECT_NE(output.find("Error: Authentication failed for service: 29"), std::string::npos);
}

/** Test for handleFrameprocessNrc called in processFrameData with a valid single frame for negative response
for Authentication SID - Resource temporarily unavailable */
TEST_F(HandleFramesTest, AuthenticationResUnNegTest){
    /* Set an arbitrary CAN ID */
    testFrame.can_id= 0;
    /* Set the Data Length Code to at least 4 */
    testFrame.can_dlc = 4;
    testFrame.data[0] = 0x03;
    testFrame.data[1] = 0x7F;
    testFrame.data[2] = 0x29;    
    int pci = testFrame.data[0];
    int sid = testFrame.data[2];
    
    /* simulated frame_data for valid single frame response */
    std::vector<uint8_t> frame_data = {0x03, 0X7F, 0x29, 0x94};
    int nrc = int(frame_data[3]);
    bool isMultiFrame = false;

    /* redirect stdout to capture the output */
    testing::internal::CaptureStdout();      
    /* handler.processNrc(testFrame.can_id, sid, nrc); */    
    handler.processFrameData(testFrame.can_id, sid, frame_data, isMultiFrame);      
    std::string output = testing::internal::GetCapturedStdout();
    /* check if the correct message is printed */
    EXPECT_NE(output.find("Error: Resource temporarily unavailable for service: 29"), std::string::npos);
}

/** Test for handleFrameprocessNrc called in processFrameData with a valid single frame for negative response
for Authentication SID - Upload download not accepted */
TEST_F(HandleFramesTest, AuthenticationUDNotAccNegTest){
    /* Set an arbitrary CAN ID */
    testFrame.can_id= 0;
    /* Set the Data Length Code to at least 4 */
    testFrame.can_dlc = 4;
    testFrame.data[0] = 0x03;
    testFrame.data[1] = 0x7F;
    testFrame.data[2] = 0x29;    
    int pci = testFrame.data[0];
    int sid = testFrame.data[2];
    
    /* simulated frame_data for valid single frame response */
    std::vector<uint8_t> frame_data = {0x03, 0X7F, 0x29, 0x70};
    int nrc = int(frame_data[3]);
    bool isMultiFrame = false;

    /* redirect stdout to capture the output */
    testing::internal::CaptureStdout();      
   /* handler.processNrc(testFrame.can_id, sid, nrc); */    
    handler.processFrameData(testFrame.can_id, sid, frame_data, isMultiFrame);   
    std::string output = testing::internal::GetCapturedStdout();
    /* check if the correct message is printed */
    EXPECT_NE(output.find("Error: Upload download not accepted for service: 29"), std::string::npos);
}

/** Test for handleFrameprocessNrc called in processFrameData with a valid single frame for negative response
for Authentication SID - Transfer data suspended */
TEST_F(HandleFramesTest, AuthenticationTDataSuspNegTest){
    /* Set an arbitrary CAN ID */
    testFrame.can_id= 0;
    /* Set the Data Length Code to at least 4 */
    testFrame.can_dlc = 4;
    testFrame.data[0] = 0x03;
    testFrame.data[1] = 0x7F;
    testFrame.data[2] = 0x29;    
    int pci = testFrame.data[0];
    int sid = testFrame.data[2];
    
    /* simulated frame_data for valid single frame response */
    std::vector<uint8_t> frame_data = {0x03, 0X7F, 0x29, 0x71};
    int nrc = int(frame_data[3]);
    bool isMultiFrame = false;

    /* redirect stdout to capture the output */
    testing::internal::CaptureStdout();      
    /* handler.processNrc(testFrame.can_id, sid, nrc); */    
    handler.processFrameData(testFrame.can_id, sid, frame_data, isMultiFrame);       
    std::string output = testing::internal::GetCapturedStdout();
    /* check if the correct message is printed */
    EXPECT_NE(output.find("Error: Transfer data suspended for service: 29"), std::string::npos);
}

/** Test for handleFrameprocessNrc called in processFrameData with a valid single frame for negative response
for Authentication SID - default */
TEST_F(HandleFramesTest, AuthenticationDefaultNegTest){
    /* Set an arbitrary CAN ID */
    testFrame.can_id= 0;
    /* Set the Data Length Code to at least 4 */
    testFrame.can_dlc = 4;
    testFrame.data[0] = 0x03;
    testFrame.data[1] = 0x7F;
    testFrame.data[2] = 0x29;    
    int pci = testFrame.data[0];
    int sid = testFrame.data[2];
    
    /* simulated frame_data for valid single frame response */
    std::vector<uint8_t> frame_data = {0x03, 0X7F, 0x29, 0x99};
    int nrc = int(frame_data[3]);
    bool isMultiFrame = false;

    /* redirect stdout to capture the output */
    testing::internal::CaptureStdout();      
    /* handler.processNrc(testFrame.can_id, sid, nrc); */    
    handler.processFrameData(testFrame.can_id, sid, frame_data, isMultiFrame);    
    std::string output = testing::internal::GetCapturedStdout();
    /* check if the correct message is printed */
    EXPECT_NE(output.find("Error: Unknown negative response code for service: 29"), std::string::npos);
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
    EXPECT_NE(output.find("TesterPresent called."), std::string::npos);
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
    EXPECT_NE(output.find("Single Frame received:"), std::string::npos);
    EXPECT_NE(output.find("TesterPresent called."), std::string::npos);
}

/** Test for handleFrameprocessNrc called in processFrameData with a valid single frame for negative response
for TesterPresent SID - Service not supported */
TEST_F(HandleFramesTest, TesterPresentServiceNegTest){
    /* Set an arbitrary CAN ID */
    testFrame.can_id= 0;
    /* Set the Data Length Code to at least 4 */
    testFrame.can_dlc = 4;
    testFrame.data[0] = 0x03;
    testFrame.data[1] = 0x7F;
    testFrame.data[2] = 0x3E;    
    int pci = testFrame.data[0];
    int sid = testFrame.data[2];
    
    /* simulated frame_data for valid single frame response */
    std::vector<uint8_t> frame_data = {0x03, 0X7F, 0x3E, 0x11};
    int nrc = int(frame_data[3]);
    bool isMultiFrame = false;

    /* redirect stdout to capture the output */
    testing::internal::CaptureStdout();       
    /* handler.processNrc(testFrame.can_id, sid, nrc); */
    handler.processFrameData(testFrame.can_id, sid, frame_data, isMultiFrame);    
    std::string output = testing::internal::GetCapturedStdout();
    /* check if the correct message is printed */
    EXPECT_NE(output.find("Error: Service not supported for service: 3e"), std::string::npos);
}

/** Test for handleFrameprocessNrc called in processFrameData with a valid single frame for negative response
for TesterPresent SID - Incorrect message length or invalid format */
TEST_F(HandleFramesTest, TesterPresentIncorrectNegTest){
    /* Set an arbitrary CAN ID */
    testFrame.can_id= 0;
    /* Set the Data Length Code to at least 4 */
    testFrame.can_dlc = 4;
    testFrame.data[0] = 0x03;
    testFrame.data[1] = 0x7F;
    testFrame.data[2] = 0x3E;    
    int pci = testFrame.data[0];
    int sid = testFrame.data[2];
    
    /* simulated frame_data for valid single frame response */
    std::vector<uint8_t> frame_data = {0x03, 0X7F, 0x3E, 0x13};
    int nrc = int(frame_data[3]);
    bool isMultiFrame = false;

    /* redirect stdout to capture the output */
    testing::internal::CaptureStdout();      
    /* handler.processNrc(testFrame.can_id, sid, nrc); */    
    handler.processFrameData(testFrame.can_id, sid, frame_data, isMultiFrame);      
    std::string output = testing::internal::GetCapturedStdout();
    /* check if the correct message is printed */
    EXPECT_NE(output.find("Error: Incorrect message length or invalid format for service: 3e"), std::string::npos);
}

/** Test for handleFrameprocessNrc called in processFrameData with a valid single frame for negative response
for TesterPresent SID - Incorrect message length or invalid format */
TEST_F(HandleFramesTest, TesterPresentRespLongNegTest){
    /* Set an arbitrary CAN ID */
    testFrame.can_id= 0;
    /* Set the Data Length Code to at least 4 */
    testFrame.can_dlc = 4;
    testFrame.data[0] = 0x03;
    testFrame.data[1] = 0x7F;
    testFrame.data[2] = 0x3E;    
    int pci = testFrame.data[0];
    int sid = testFrame.data[2];
    
    /* simulated frame_data for valid single frame response */
    std::vector<uint8_t> frame_data = {0x03, 0X7F, 0x3E, 0x14};
    int nrc = int(frame_data[3]);
    bool isMultiFrame = false;

    /* redirect stdout to capture the output */
    testing::internal::CaptureStdout();      
    /* handler.processNrc(testFrame.can_id, sid, nrc); */    
    handler.processFrameData(testFrame.can_id, sid, frame_data, isMultiFrame);      
    std::string output = testing::internal::GetCapturedStdout();
    /* check if the correct message is printed */
    EXPECT_NE(output.find("Error: Response too long for service: 3e"), std::string::npos);
}

/** Test for handleFrameprocessNrc called in processFrameData with a valid single frame for negative response
for TesterPresent SID - No response from subnet component */
TEST_F(HandleFramesTest, TesterPresentNoRespNegTest){
    /* Set an arbitrary CAN ID */
    testFrame.can_id= 0;
    /* Set the Data Length Code to at least 4 */
    testFrame.can_dlc = 4;
    testFrame.data[0] = 0x03;
    testFrame.data[1] = 0x7F;
    testFrame.data[2] = 0x3E;    
    int pci = testFrame.data[0];
    int sid = testFrame.data[2];
    
    /* simulated frame_data for valid single frame response */
    std::vector<uint8_t> frame_data = {0x03, 0X7F, 0x3E, 0x25};
    int nrc = int(frame_data[3]);
    bool isMultiFrame = false;

    /* redirect stdout to capture the output */
    testing::internal::CaptureStdout();      
   /* handler.processNrc(testFrame.can_id, sid, nrc); */    
    handler.processFrameData(testFrame.can_id, sid, frame_data, isMultiFrame);    
    std::string output = testing::internal::GetCapturedStdout();
    /* check if the correct message is printed */
    EXPECT_NE(output.find("Error: No response from subnet component for service: 3e"), std::string::npos);
}

/** Test for handleFrameprocessNrc called in processFrameData with a valid single frame for negative response
for TesterPresent SID - Authentication failed */
TEST_F(HandleFramesTest, TesterPresentNoAuthNegTest){
    /* Set an arbitrary CAN ID */
    testFrame.can_id= 0;
    /* Set the Data Length Code to at least 4 */
    testFrame.can_dlc = 4;
    testFrame.data[0] = 0x03;
    testFrame.data[1] = 0x7F;
    testFrame.data[2] = 0x3E;    
    int pci = testFrame.data[0];
    int sid = testFrame.data[2];
    
    /* simulated frame_data for valid single frame response */
    std::vector<uint8_t> frame_data = {0x03, 0X7F, 0x3E, 0x34};
    int nrc = int(frame_data[3]);
    bool isMultiFrame = false;

    /* redirect stdout to capture the output */
    testing::internal::CaptureStdout();      
    /* handler.processNrc(testFrame.can_id, sid, nrc); */    
    handler.processFrameData(testFrame.can_id, sid, frame_data, isMultiFrame);       
    std::string output = testing::internal::GetCapturedStdout();
    /* check if the correct message is printed */
    EXPECT_NE(output.find("Error: Authentication failed for service: 3e"), std::string::npos);
}

/** Test for handleFrameprocessNrc called in processFrameData with a valid single frame for negative response
for TesterPresent SID - Resource temporarily unavailable */
TEST_F(HandleFramesTest, TesterPresentResUnNegTest){
    /* Set an arbitrary CAN ID */
    testFrame.can_id= 0;
    /* Set the Data Length Code to at least 4 */
    testFrame.can_dlc = 4;
    testFrame.data[0] = 0x03;
    testFrame.data[1] = 0x7F;
    testFrame.data[2] = 0x3E;    
    int pci = testFrame.data[0];
    int sid = testFrame.data[2];
    
    /* simulated frame_data for valid single frame response */
    std::vector<uint8_t> frame_data = {0x03, 0X7F, 0x3E, 0x94};
    int nrc = int(frame_data[3]);
    bool isMultiFrame = false;

    /* redirect stdout to capture the output */
    testing::internal::CaptureStdout();      
    /* handler.processNrc(testFrame.can_id, sid, nrc); */    
    handler.processFrameData(testFrame.can_id, sid, frame_data, isMultiFrame);      
    std::string output = testing::internal::GetCapturedStdout();
    /* check if the correct message is printed */
    EXPECT_NE(output.find("Error: Resource temporarily unavailable for service: 3e"), std::string::npos);
}

/** Test for handleFrameprocessNrc called in processFrameData with a valid single frame for negative response
for TesterPresent SID - Upload download not accepted */
TEST_F(HandleFramesTest, TesterPresentUDNotAccNegTest){
    /* Set an arbitrary CAN ID */
    testFrame.can_id= 0;
    /* Set the Data Length Code to at least 4 */
    testFrame.can_dlc = 4;
    testFrame.data[0] = 0x03;
    testFrame.data[1] = 0x7F;
    testFrame.data[2] = 0x3E;    
    int pci = testFrame.data[0];
    int sid = testFrame.data[2];
    
    /* simulated frame_data for valid single frame response */
    std::vector<uint8_t> frame_data = {0x03, 0X7F, 0x3E, 0x70};
    int nrc = int(frame_data[3]);
    bool isMultiFrame = false;

    /* redirect stdout to capture the output */
    testing::internal::CaptureStdout();      
   /* handler.processNrc(testFrame.can_id, sid, nrc); */    
    handler.processFrameData(testFrame.can_id, sid, frame_data, isMultiFrame);   
    std::string output = testing::internal::GetCapturedStdout();
    /* check if the correct message is printed */
    EXPECT_NE(output.find("Error: Upload download not accepted for service: 3e"), std::string::npos);
}

/** Test for handleFrameprocessNrc called in processFrameData with a valid single frame for negative response
for TesterPresent SID - Transfer data suspended */
TEST_F(HandleFramesTest, TesterPresentTDataSuspNegTest){
    /* Set an arbitrary CAN ID */
    testFrame.can_id= 0;
    /* Set the Data Length Code to at least 4 */
    testFrame.can_dlc = 4;
    testFrame.data[0] = 0x03;
    testFrame.data[1] = 0x7F;
    testFrame.data[2] = 0x3E;    
    int pci = testFrame.data[0];
    int sid = testFrame.data[2];
    
    /* simulated frame_data for valid single frame response */
    std::vector<uint8_t> frame_data = {0x03, 0X7F, 0x3E, 0x71};
    int nrc = int(frame_data[3]);
    bool isMultiFrame = false;

    /* redirect stdout to capture the output */
    testing::internal::CaptureStdout();      
    /* handler.processNrc(testFrame.can_id, sid, nrc); */    
    handler.processFrameData(testFrame.can_id, sid, frame_data, isMultiFrame);       
    std::string output = testing::internal::GetCapturedStdout();
    /* check if the correct message is printed */
    EXPECT_NE(output.find("Error: Transfer data suspended for service: 3e"), std::string::npos);
}

/** Test for handleFrameprocessNrc called in processFrameData with a valid single frame for negative response
for TesterPresent SID - default */
TEST_F(HandleFramesTest, TesterPresentDefaultNegTest){
    /* Set an arbitrary CAN ID */
    testFrame.can_id= 0;
    /* Set the Data Length Code to at least 4 */
    testFrame.can_dlc = 4;
    testFrame.data[0] = 0x03;
    testFrame.data[1] = 0x7F;
    testFrame.data[2] = 0x3E;    
    int pci = testFrame.data[0];
    int sid = testFrame.data[2];
    
    /* simulated frame_data for valid single frame response */
    std::vector<uint8_t> frame_data = {0x03, 0X7F, 0x3E, 0x99};
    int nrc = int(frame_data[3]);
    bool isMultiFrame = false;

    /* redirect stdout to capture the output */
    testing::internal::CaptureStdout();      
    /* handler.processNrc(testFrame.can_id, sid, nrc); */    
    handler.processFrameData(testFrame.can_id, sid, frame_data, isMultiFrame);    
    std::string output = testing::internal::GetCapturedStdout();
    /* check if the correct message is printed */
    EXPECT_NE(output.find("Error: Unknown negative response code for service: 3e"), std::string::npos);
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
    EXPECT_NE(output.find("AccessTimingParameters called."), std::string::npos);
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
    EXPECT_NE(output.find("Single Frame received:"), std::string::npos);
    EXPECT_NE(output.find("AccessTimingParameters called."), std::string::npos);
}

/** Test for handleFrameprocessNrc called in processFrameData with a valid single frame for negative response
for AccessTimingParameters SID - Service not supported */
TEST_F(HandleFramesTest, AccessTimingParametersServiceNegTest){
    /* Set an arbitrary CAN ID */
    testFrame.can_id= 0;
    /* Set the Data Length Code to at least 4 */
    testFrame.can_dlc = 4;
    testFrame.data[0] = 0x03;
    testFrame.data[1] = 0x7F;
    testFrame.data[2] = 0x83;    
    int pci = testFrame.data[0];
    int sid = testFrame.data[2];
    
    /* simulated frame_data for valid single frame response */
    std::vector<uint8_t> frame_data = {0x03, 0X7F, 0x83, 0x11};
    int nrc = int(frame_data[3]);
    bool isMultiFrame = false;

    /* redirect stdout to capture the output */
    testing::internal::CaptureStdout();       
    /* handler.processNrc(testFrame.can_id, sid, nrc); */
    handler.processFrameData(testFrame.can_id, sid, frame_data, isMultiFrame);    
    std::string output = testing::internal::GetCapturedStdout();
    /* check if the correct message is printed */
    EXPECT_NE(output.find("Error: Service not supported for service: 83"), std::string::npos);
}

/* Test for processFrameData with a valid ReadDataByIdentifier frame */
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
    EXPECT_NE(output.find("ReadDataByIdentifier called."), std::string::npos);
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
    EXPECT_NE(output.find("Single Frame received:"), std::string::npos);
    EXPECT_NE(output.find("ReadDataByIdentifier called."), std::string::npos);
}

/** Test for handleFrameprocessNrc called in processFrameData with a valid single frame for negative response
for ReadDataByIdentifier SID - Service not supported */
TEST_F(HandleFramesTest, ReadDataByIdentifierServiceNegTest){
    /* Set an arbitrary CAN ID */
    testFrame.can_id= 0;
    /* Set the Data Length Code to at least 4 */
    testFrame.can_dlc = 4;
    testFrame.data[0] = 0x03;
    testFrame.data[1] = 0x7F;
    testFrame.data[2] = 0x22;    
    int pci = testFrame.data[0];
    int sid = testFrame.data[2];
    
    /* simulated frame_data for valid single frame response */
    std::vector<uint8_t> frame_data = {0x03, 0X7F, 0x22, 0x11};
    int nrc = int(frame_data[3]);
    bool isMultiFrame = false;

    /* redirect stdout to capture the output */
    testing::internal::CaptureStdout();       
    /* handler.processNrc(testFrame.can_id, sid, nrc); */
    handler.processFrameData(testFrame.can_id, sid, frame_data, isMultiFrame);    
    std::string output = testing::internal::GetCapturedStdout();
    /* check if the correct message is printed */
    EXPECT_NE(output.find("Error: Service not supported for service: 22"), std::string::npos);
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
    EXPECT_NE(output.find("ReadMemoryByAddress called."), std::string::npos);
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
    EXPECT_NE(output.find("Single Frame received:"), std::string::npos);
    EXPECT_NE(output.find("ReadMemoryByAddress called."), std::string::npos);
}

/** Test for handleFrameprocessNrc called in processFrameData with a valid single frame for negative response
for ReadMemoryByAddress SID - Service not supported */
TEST_F(HandleFramesTest, ReadMemoryByAddressServiceNegTest){
    /* Set an arbitrary CAN ID */
    testFrame.can_id= 0;
    /* Set the Data Length Code to at least 4 */
    testFrame.can_dlc = 4;
    testFrame.data[0] = 0x03;
    testFrame.data[1] = 0x7F;
    testFrame.data[2] = 0x23;    
    int pci = testFrame.data[0];
    int sid = testFrame.data[2];
    
    /* simulated frame_data for valid single frame response */
    std::vector<uint8_t> frame_data = {0x03, 0X7F, 0x23, 0x11};
    int nrc = int(frame_data[3]);
    bool isMultiFrame = false;

    /* redirect stdout to capture the output */
    testing::internal::CaptureStdout();       
    /* handler.processNrc(testFrame.can_id, sid, nrc); */
    handler.processFrameData(testFrame.can_id, sid, frame_data, isMultiFrame);    
    std::string output = testing::internal::GetCapturedStdout();
    /* check if the correct message is printed */
    EXPECT_NE(output.find("Error: Service not supported for service: 23"), std::string::npos);
    EXPECT_EQ(sid, testFrame.data[2]);
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
    EXPECT_NE(output.find("WriteDataByIdentifier called with one frame."), std::string::npos);
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
    EXPECT_NE(output.find("Single Frame received:"), std::string::npos);
    EXPECT_NE(output.find("WriteDataByIdentifier called with one frame."), std::string::npos);
}

/** Test for handleFrameprocessNrc called in processFrameData with a valid single frame for negative response
for WriteDataByIdentifier SID - Service not supported */
TEST_F(HandleFramesTest, WriteDataByIdentifierServiceNegTest){
    /* Set an arbitrary CAN ID */
    testFrame.can_id= 0;
    /* Set the Data Length Code to at least 4 */
    testFrame.can_dlc = 4;
    testFrame.data[0] = 0x03;
    testFrame.data[1] = 0x7F;
    testFrame.data[2] = 0x2E;    
    int pci = testFrame.data[0];
    int sid = testFrame.data[2];
    
    /* simulated frame_data for valid single frame response */
    std::vector<uint8_t> frame_data = {0x03, 0X7F, 0x2E, 0x11};
    int nrc = int(frame_data[3]);
    bool isMultiFrame = false;

    /* redirect stdout to capture the output */
    testing::internal::CaptureStdout();       
    /* handler.processNrc(testFrame.can_id, sid, nrc); */
    handler.processFrameData(testFrame.can_id, sid, frame_data, isMultiFrame);    
    std::string output = testing::internal::GetCapturedStdout();
    /* check if the correct message is printed */
    EXPECT_NE(output.find("Error: Service not supported for service: 2e"), std::string::npos);
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
    EXPECT_NE(output.find("ClearDiagnosticInformation called."), std::string::npos);
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
    EXPECT_NE(output.find("Single Frame received:"), std::string::npos);
    EXPECT_NE(output.find("ClearDiagnosticInformation called."), std::string::npos);
}

/** Test for handleFrameprocessNrc called in processFrameData with a valid single frame for negative response
for ClearDiagnosticInformation SID - Service not supported */
TEST_F(HandleFramesTest, ClearDiagnosticInformationServiceNegTest){
    /* Set an arbitrary CAN ID */
    testFrame.can_id= 0;
    /* Set the Data Length Code to at least 4 */
    testFrame.can_dlc = 4;
    testFrame.data[0] = 0x03;
    testFrame.data[1] = 0x7F;
    testFrame.data[2] = 0x14;    
    int pci = testFrame.data[0];
    int sid = testFrame.data[2];
    
    /* simulated frame_data for valid single frame response */
    std::vector<uint8_t> frame_data = {0x03, 0X7F, 0x14, 0x11};
    int nrc = int(frame_data[3]);
    bool isMultiFrame = false;

    /* redirect stdout to capture the output */
    testing::internal::CaptureStdout();       
    /* handler.processNrc(testFrame.can_id, sid, nrc); */
    handler.processFrameData(testFrame.can_id, sid, frame_data, isMultiFrame);    
    std::string output = testing::internal::GetCapturedStdout();
    /* check if the correct message is printed */
    EXPECT_NE(output.find("Error: Service not supported for service: 14"), std::string::npos);
    EXPECT_EQ(sid, testFrame.data[2]);
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
    EXPECT_NE(output.find("ReadDtcInformation called."), std::string::npos);
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
    EXPECT_NE(output.find("Single Frame received:"), std::string::npos);
    EXPECT_NE(output.find("ReadDtcInformation called."), std::string::npos);
}

/** Test for handleFrameprocessNrc called in processFrameData with a valid single frame for negative response
for ReadDtcInformation SID - Service not supported */
TEST_F(HandleFramesTest, ReadDtcInformationServiceNegTest){
    /* Set an arbitrary CAN ID */
    testFrame.can_id= 0;
    /* Set the Data Length Code to at least 4 */
    testFrame.can_dlc = 4;
    testFrame.data[0] = 0x03;
    testFrame.data[1] = 0x7F;
    testFrame.data[2] = 0x19;    
    int pci = testFrame.data[0];
    int sid = testFrame.data[2];
    
    /* simulated frame_data for valid single frame response */
    std::vector<uint8_t> frame_data = {0x03, 0X7F, 0x19, 0x11};
    int nrc = int(frame_data[3]);
    bool isMultiFrame = false;

    /* redirect stdout to capture the output */
    testing::internal::CaptureStdout();       
    /* handler.processNrc(testFrame.can_id, sid, nrc); */
    handler.processFrameData(testFrame.can_id, sid, frame_data, isMultiFrame);    
    std::string output = testing::internal::GetCapturedStdout();
    /* check if the correct message is printed */
    EXPECT_NE(output.find("Error: Service not supported for service: 19"), std::string::npos);
    EXPECT_EQ(sid, testFrame.data[2]);
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
    EXPECT_NE(output.find("RoutineControl called."), std::string::npos);
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
    EXPECT_NE(output.find("Single Frame received:"), std::string::npos);
    EXPECT_NE(output.find("RoutineControl called."), std::string::npos);
}

/** Test for handleFrameprocessNrc called in processFrameData with a valid single frame for negative response
for RoutineControl SID - Service not supported */
TEST_F(HandleFramesTest, RoutineControlServiceNegTest){
    /* Set an arbitrary CAN ID */
    testFrame.can_id= 0;
    /* Set the Data Length Code to at least 4 */
    testFrame.can_dlc = 4;
    testFrame.data[0] = 0x03;
    testFrame.data[1] = 0x7F;
    testFrame.data[2] = 0x31;    
    int pci = testFrame.data[0];
    int sid = testFrame.data[2];
    
    /* simulated frame_data for valid single frame response */
    std::vector<uint8_t> frame_data = {0x03, 0X7F, 0x31, 0x11};
    int nrc = int(frame_data[3]);
    bool isMultiFrame = false;

    /* redirect stdout to capture the output */
    testing::internal::CaptureStdout();       
    /* handler.processNrc(testFrame.can_id, sid, nrc); */
    handler.processFrameData(testFrame.can_id, sid, frame_data, isMultiFrame);    
    std::string output = testing::internal::GetCapturedStdout();
    /* check if the correct message is printed */
    EXPECT_NE(output.find("Error: Service not supported for service: 31"), std::string::npos);
    EXPECT_EQ(sid, testFrame.data[2]);
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
    EXPECT_NE(output.find("Response for DiagnosticSessionControl received."), std::string::npos);
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
    EXPECT_NE(output.find("Single Frame received:"), std::string::npos);
    EXPECT_NE(output.find("Response for DiagnosticSessionControl received."), std::string::npos);
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
    EXPECT_NE(output.find("Response from EcuReset received."), std::string::npos);
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
    EXPECT_NE(output.find("Single Frame received:"), std::string::npos);
    EXPECT_NE(output.find("Response from EcuReset received."), std::string::npos);
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
    EXPECT_NE(output.find("Response from SecurityAccess received."), std::string::npos);
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
    EXPECT_NE(output.find("Single Frame received:"), std::string::npos);
    EXPECT_NE(output.find("Response from SecurityAccess received."), std::string::npos);
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
    EXPECT_NE(output.find("Response from Authentication received."), std::string::npos);
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
    EXPECT_NE(output.find("Single Frame received:"), std::string::npos);
    EXPECT_NE(output.find("Response from Authentication received."), std::string::npos);
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
    EXPECT_NE(output.find("Response from TesterPresent received."), std::string::npos);
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
    EXPECT_NE(output.find("Single Frame received:"), std::string::npos);
    EXPECT_NE(output.find("Response from TesterPresent received."), std::string::npos);
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
    EXPECT_NE(output.find("Response from AccessTimingParameters received."), std::string::npos);
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
    EXPECT_NE(output.find("Single Frame received:"), std::string::npos);
    EXPECT_NE(output.find("Response from AccessTimingParameters received."), std::string::npos);
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
    EXPECT_NE(output.find("Response from ReadDataByIdentifier received in one frame."), std::string::npos);
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
    EXPECT_NE(output.find("Single Frame received:"), std::string::npos);
    EXPECT_NE(output.find("Response from ReadDataByIdentifier received in one frame."), std::string::npos);
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
    EXPECT_NE(output.find("Response from ReadMemoryByAdress received in one frame."), std::string::npos);
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
    EXPECT_NE(output.find("Single Frame received:"), std::string::npos);
    EXPECT_NE(output.find("Response from ReadMemoryByAdress received in one frame."), std::string::npos);
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
    EXPECT_NE(output.find("Response from WriteDataByIdentifier received."), std::string::npos);
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
    EXPECT_NE(output.find("Single Frame received:"), std::string::npos);
    EXPECT_NE(output.find("Response from WriteDataByIdentifier received."), std::string::npos);
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
    EXPECT_NE(output.find("Response from ClearDiagnosticInformation received."), std::string::npos);
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
    EXPECT_NE(output.find("Single Frame received:"), std::string::npos);
    EXPECT_NE(output.find("Response from ClearDiagnosticInformation received."), std::string::npos);
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
    EXPECT_NE(output.find("Response from ReadDtcInformation received."), std::string::npos);
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
    EXPECT_NE(output.find("Single Frame received:"), std::string::npos);
    EXPECT_NE(output.find("Response from ReadDtcInformation received."), std::string::npos);
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
    EXPECT_NE(output.find("Response from RoutineControl received."), std::string::npos);
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
    EXPECT_NE(output.find("Single Frame received:"), std::string::npos);
    EXPECT_NE(output.find("Response from RoutineControl received."), std::string::npos);
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
    EXPECT_NE(output.find("RequestDownload called."), std::string::npos);
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
    EXPECT_NE(output.find("Single Frame received:"), std::string::npos);
    EXPECT_NE(output.find("RequestDownload called."), std::string::npos);
}

/** Test for handleFrameprocessNrc called in processFrameData with a valid single frame for negative response
for OtaRequestDownload SID - Service not supported */
TEST_F(HandleFramesTest, OtaRequestDownloadServiceNegTest){
    /* Set an arbitrary CAN ID */
    testFrame.can_id= 0;
    /* Set the Data Length Code to at least 4 */
    testFrame.can_dlc = 4;
    testFrame.data[0] = 0x03;
    testFrame.data[1] = 0x7F;
    testFrame.data[2] = 0x34;    
    int pci = testFrame.data[0];
    int sid = testFrame.data[2];
    
    /* simulated frame_data for valid single frame response */
    std::vector<uint8_t> frame_data = {0x03, 0X7F, 0x34, 0x11};
    int nrc = int(frame_data[3]);
    bool isMultiFrame = false;

    /* redirect stdout to capture the output */
    testing::internal::CaptureStdout();       
    /* handler.processNrc(testFrame.can_id, sid, nrc); */
    handler.processFrameData(testFrame.can_id, sid, frame_data, isMultiFrame);    
    std::string output = testing::internal::GetCapturedStdout();
    /* check if the correct message is printed */
    EXPECT_NE(output.find("Error: Service not supported for service: 34"), std::string::npos);
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
    EXPECT_NE(output.find("TransferData called with one frame"), std::string::npos);
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
    EXPECT_NE(output.find("Single Frame received:"), std::string::npos);
    EXPECT_NE(output.find("TransferData called with one frame"), std::string::npos);
}

/** Test for handleFrameprocessNrc called in processFrameData with a valid single frame for negative response
for TransferData SID - Service not supported */
TEST_F(HandleFramesTest, TransferDataServiceNegTest){
    /* Set an arbitrary CAN ID */
    testFrame.can_id= 0;
    /* Set the Data Length Code to at least 4 */
    testFrame.can_dlc = 4;
    testFrame.data[0] = 0x03;
    testFrame.data[1] = 0x7F;
    testFrame.data[2] = 0x36;    
    int pci = testFrame.data[0];
    int sid = testFrame.data[2];
    
    /* simulated frame_data for valid single frame response */
    std::vector<uint8_t> frame_data = {0x03, 0X7F, 0x36, 0x11};
    int nrc = int(frame_data[3]);
    bool isMultiFrame = false;

    /* redirect stdout to capture the output */
    testing::internal::CaptureStdout();       
    /* handler.processNrc(testFrame.can_id, sid, nrc); */
    handler.processFrameData(testFrame.can_id, sid, frame_data, isMultiFrame);    
    std::string output = testing::internal::GetCapturedStdout();
    /* check if the correct message is printed */
    EXPECT_NE(output.find("Error: Service not supported for service: 36"), std::string::npos);
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
    /* check if the correct message is printed */\
    EXPECT_NE(output.find("RequestTransferExit called."), std::string::npos);
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
    EXPECT_NE(output.find("Single Frame received:"), std::string::npos);
    EXPECT_NE(output.find("RequestTransferExit called."), std::string::npos);
}

/** Test for handleFrameprocessNrc called in processFrameData with a valid single frame for negative response
for RequestTransferExit SID - Service not supported */
TEST_F(HandleFramesTest, RequestTransferExitServiceNegTest){
    /* Set an arbitrary CAN ID */
    testFrame.can_id= 0;
    /* Set the Data Length Code to at least 4 */
    testFrame.can_dlc = 4;
    testFrame.data[0] = 0x03;
    testFrame.data[1] = 0x7F;
    testFrame.data[2] = 0x37;    
    int pci = testFrame.data[0];
    int sid = testFrame.data[2];
    
    /* simulated frame_data for valid single frame response */
    std::vector<uint8_t> frame_data = {0x03, 0X7F, 0x37, 0x11};
    int nrc = int(frame_data[3]);
    bool isMultiFrame = false;

    /* redirect stdout to capture the output */
    testing::internal::CaptureStdout();       
    /* handler.processNrc(testFrame.can_id, sid, nrc); */
    handler.processFrameData(testFrame.can_id, sid, frame_data, isMultiFrame);    
    std::string output = testing::internal::GetCapturedStdout();
    /* check if the correct message is printed */
    EXPECT_NE(output.find("Error: Service not supported for service: 37"), std::string::npos);
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
    EXPECT_NE(output.find("RequestUpdateStatus called."), std::string::npos);
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
    EXPECT_NE(output.find("Single Frame received:"), std::string::npos);
    EXPECT_NE(output.find("RequestUpdateStatus called."), std::string::npos);
}

/** Test for handleFrameprocessNrc called in processFrameData with a valid single frame for negative response
for RequestUpdateStatus SID - Service not supported */
TEST_F(HandleFramesTest, RequestUpdateStatusServiceNegTest){
    /* Set an arbitrary CAN ID */
    testFrame.can_id= 0;
    /* Set the Data Length Code to at least 4 */
    testFrame.can_dlc = 4;
    testFrame.data[0] = 0x03;
    testFrame.data[1] = 0x7F;
    testFrame.data[2] = 0x32;    
    int pci = testFrame.data[0];
    int sid = testFrame.data[2];
    
    /* simulated frame_data for valid single frame response */
    std::vector<uint8_t> frame_data = {0x03, 0X7F, 0x32, 0x11};
    int nrc = int(frame_data[3]);
    bool isMultiFrame = false;

    /* redirect stdout to capture the output */
    testing::internal::CaptureStdout();       
    /* handler.processNrc(testFrame.can_id, sid, nrc); */
    handler.processFrameData(testFrame.can_id, sid, frame_data, isMultiFrame);    
    std::string output = testing::internal::GetCapturedStdout();
    /* check if the correct message is printed */
    EXPECT_NE(output.find("Error: Service not supported for service: 32"), std::string::npos);
    EXPECT_EQ(sid, testFrame.data[2]);
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
    EXPECT_NE(output.find("Response from RequestDownload received."), std::string::npos);
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
    EXPECT_NE(output.find("Single Frame received:"), std::string::npos);
    EXPECT_NE(output.find("Response from RequestDownload received."), std::string::npos);
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
    EXPECT_NE(output.find("Response from TransferData received."), std::string::npos);
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
    EXPECT_NE(output.find("Single Frame received:"), std::string::npos);
    EXPECT_NE(output.find("Response from TransferData received."), std::string::npos);
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
    EXPECT_NE(output.find("Response from RequestTransferExit received."), std::string::npos);
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
    EXPECT_NE(output.find("Single Frame received:"), std::string::npos);
    EXPECT_NE(output.find("Response from RequestTransferExit received."), std::string::npos);
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
    EXPECT_NE(output.find("Response from RequestUpdateStatus received."), std::string::npos);
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
    EXPECT_NE(output.find("Single Frame received:"), std::string::npos);
    EXPECT_NE(output.find("Response from RequestUpdateStatus received."), std::string::npos);
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
    EXPECT_NE(output.find("Unknown request/response received."), std::string::npos);
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
    EXPECT_NE(output.find("Single Frame received:"), std::string::npos);
    EXPECT_NE(output.find("Unknown request/response received."), std::string::npos);
}

/** Test the first frame */
TEST_F(HandleFramesTest, firstFrameTest){
    /* Set an arbitrary CAN ID */
    testFrame.can_id= 0;
    /* Set the Data Length Code */
    testFrame.can_dlc = 8;
    testFrame.data[0] = 0x10;
    testFrame.data[1] = 0x3E;
    testFrame.data[2] = 0x22;    
    int pci = testFrame.data[0];
    int sid = testFrame.data[2];
   
    // int rest =  testFrame.data[1] % 7 > 0 ? 1 : 0;
    // int expectedFrames = testFrame.data[1] / 7 + rest;
    
    bool isMultiFrame = false;

    /* redirect stdout to capture the output */
    testing::internal::CaptureStdout();    
    handler.handleFrame(testFrame);
    std::string output = testing::internal::GetCapturedStdout();
    /* check if the correct message is printed */
    EXPECT_NE(output.find("Multi-frame Sequence with 9 frames"), std::string::npos);
}

/* Test if the frame's sequence number matches the expected sequence number */
TEST_F(HandleFramesTest, MultipleFrameTest){
    /* Set an arbitrary CAN ID */
    testFrame.can_id= 0;
     /* Set the Data Length Code  */
    testFrame.can_dlc = 8;     
    testFrame.data[0] = 0x10;
    testFrame.data[1] = 0x0A;
    /* sid */
    testFrame.data[2] = 0x2E; 
    testFrame.data[5] = 0x99;
    testFrame.data[6] = 0x99;
    testFrame.data[7] = 0x99;
    //int pci = testFrame.data[0];
    int sid = testFrame.data[2];
    testing::internal::CaptureStdout(); 
    handler.handleFrame(testFrame);
    std::string output = testing::internal::GetCapturedStdout();
   
    testFrame.data[0] = 0x23;   
        
    bool isMultiFrame = true;

    /* redirect stdout to capture the output */
    testing::internal::CaptureStdout();    
    handler.handleFrame(testFrame);
    std::string output1 = testing::internal::GetCapturedStdout();
    /* check if the correct message is printed */
    /* values should be 0x21 and 0x23 but I can't log hex and then words in the same print */
    EXPECT_NE(output1.find("Invalid consecutive frame sequence: expected 33 but received 35"), std::string::npos);                 
}

/** Test if all multi-frames have been received 
 *  WriteDataByIdentifier
*/
TEST_F(HandleFramesTest, WriteDataByIdentifierMultipleFramesReceivedTest){
    /* Set an arbitrary CAN ID */
    testFrame.can_id= 0; 
    /* Set the Data Length Code  */
    testFrame.can_dlc = 8;     
    testFrame.data[0] = 0x10;
    testFrame.data[1] = 0x0A;
    /* sid */
    testFrame.data[2] = 0x2E; 
    testFrame.data[5] = 0x99;
    testFrame.data[6] = 0x99;
    testFrame.data[7] = 0x99;
   
    int sid = testFrame.data[2];
    testing::internal::CaptureStdout(); 
    handler.handleFrame(testFrame);
    std::string output = testing::internal::GetCapturedStdout();
   
    testFrame.can_dlc = 8;
    testFrame.data[0] = 0x21;
    testFrame.data[1] = 0x99;
    testFrame.data[2] = 0x99; 
    testFrame.data[3] = 0x99;
    testFrame.data[4] = 0x99; 
    testFrame.data[5] = 0x99;
    testFrame.data[6] = 0x99;
    testFrame.data[7] = 0x99;
       
    testing::internal::CaptureStdout(); 
    handler.handleFrame(testFrame);
    std::string output1 = testing::internal::GetCapturedStdout();
           
    //int expected_frames = 2;
    std::vector<uint8_t> frame_data = {0x99, 0x99, 0x99, 0X99, 0X99, 0X99, 0X99, 0X99, 0X99, 0X99};  
    
    bool isMultiFrame = true;
     
    /* redirect stdout to capture the output */
    testing::internal::CaptureStdout();    
    handler.processFrameData(testFrame.can_id, sid, frame_data, isMultiFrame);
    std::string output2= testing::internal::GetCapturedStdout();
    /* check if the correct message is printed */
    EXPECT_NE(output2.find("WriteDataByIdentifier called with multiple frames."), std::string::npos);                      
}

/** Test if all multi-frames have been received 
 *  Response from ReadDataByIdentifier
*/
TEST_F(HandleFramesTest, ResReadDataByIdentifierMultipleFrameTest){
    /* Set an arbitrary CAN ID */
    testFrame.can_id= 0; 
    /* Set the Data Length Code  */
    testFrame.can_dlc = 8;     
    testFrame.data[0] = 0x10;
    testFrame.data[1] = 0x0A;
    /* sid */
    testFrame.data[2] = 0x62; 
    testFrame.data[5] = 0x99;
    testFrame.data[6] = 0x99;
    testFrame.data[7] = 0x99;
    
    int sid = testFrame.data[2];
    testing::internal::CaptureStdout(); 
    handler.handleFrame(testFrame);
    std::string output = testing::internal::GetCapturedStdout();
    
    testFrame.can_dlc = 8;
    testFrame.data[0] = 0x21;
    testFrame.data[1] = 0x99;
    testFrame.data[2] = 0x99; 
    testFrame.data[3] = 0x99;
    testFrame.data[4] = 0x99; 
    testFrame.data[5] = 0x99;
    testFrame.data[6] = 0x99;
    testFrame.data[7] = 0x99;
       
    testing::internal::CaptureStdout(); 
    handler.handleFrame(testFrame);
    std::string output1 = testing::internal::GetCapturedStdout();
               
    std::vector<uint8_t> frame_data = {0x99, 0x99, 0x99, 0X99, 0X99, 0X99, 0X99, 0X99, 0X99, 0X99};  
    
    bool isMultiFrame = true;
     
    /* redirect stdout to capture the output */
    testing::internal::CaptureStdout();    
    handler.processFrameData(testFrame.can_id, sid, frame_data, isMultiFrame);
    std::string output2= testing::internal::GetCapturedStdout();
    /* check if the correct message is printed */
    EXPECT_NE(output2.find("Response from ReadDataByIdentifier received."), std::string::npos);
    EXPECT_NE(output2.find("Received multiple frames containing 10 bytes of data"), std::string::npos);                        
}

/** Test if all multi-frames have been received 
 *  Response from ReadMemoryByAddress
*/
TEST_F(HandleFramesTest, ResReadMemoryByAddressMultipleFrameTest){
    /* Set an arbitrary CAN ID */
    /* Set an arbitrary CAN ID */
    testFrame.can_id= 0; 
    /* Set the Data Length Code  */
    testFrame.can_dlc = 8;     
    testFrame.data[0] = 0x10;
    testFrame.data[1] = 0x0A;
    /* sid */
    testFrame.data[2] = 0x63; 
    testFrame.data[5] = 0x99;
    testFrame.data[6] = 0x99;
    testFrame.data[7] = 0x99;
    
    int sid = testFrame.data[2];
    testing::internal::CaptureStdout(); 
    handler.handleFrame(testFrame);
    std::string output = testing::internal::GetCapturedStdout();
    
    testFrame.can_dlc = 8;
    testFrame.data[0] = 0x21;
    testFrame.data[1] = 0x99;
    testFrame.data[2] = 0x99; 
    testFrame.data[3] = 0x99;
    testFrame.data[4] = 0x99; 
    testFrame.data[5] = 0x99;
    testFrame.data[6] = 0x99;
    testFrame.data[7] = 0x99;
       
    testing::internal::CaptureStdout(); 
    handler.handleFrame(testFrame);
    std::string output1 = testing::internal::GetCapturedStdout();
               
    std::vector<uint8_t> frame_data = {0x99, 0x99, 0x99, 0X99, 0X99, 0X99, 0X99, 0X99, 0X99, 0X99};  
    
    bool isMultiFrame = true;
     
    /* redirect stdout to capture the output */
    testing::internal::CaptureStdout();    
    handler.processFrameData(testFrame.can_id, sid, frame_data, isMultiFrame);
    std::string output2= testing::internal::GetCapturedStdout();
    /* check if the correct message is printed */
    EXPECT_NE(output2.find("Response from ReadMemoryByAdress received."), std::string::npos);
    EXPECT_NE(output2.find("Received multiple frames containing 10 bytes of data"), std::string::npos);                     
}

/** Test if all multi-frames have been received 
 *  TransferData
*/
TEST_F(HandleFramesTest, TransferDataMultipleFrameTest){
    
    /* Set an arbitrary CAN ID */
    testFrame.can_id= 0; 
    /* Set the Data Length Code  */
    testFrame.can_dlc = 8;     
    testFrame.data[0] = 0x10;
    testFrame.data[1] = 0x0A;
    /* sid */
    testFrame.data[2] = 0x36; 
    testFrame.data[5] = 0x99;
    testFrame.data[6] = 0x99;
    testFrame.data[7] = 0x99;
   
    int sid = testFrame.data[2];
    testing::internal::CaptureStdout(); 
    handler.handleFrame(testFrame);
    std::string output = testing::internal::GetCapturedStdout();
   
    testFrame.can_dlc = 8;
    testFrame.data[0] = 0x21;
    testFrame.data[1] = 0x99;
    testFrame.data[2] = 0x99; 
    testFrame.data[3] = 0x99;
    testFrame.data[4] = 0x99; 
    testFrame.data[5] = 0x99;
    testFrame.data[6] = 0x99;
    testFrame.data[7] = 0x99;
       
    testing::internal::CaptureStdout(); 
    handler.handleFrame(testFrame);
    std::string output1 = testing::internal::GetCapturedStdout();
           
    //int expected_frames = 2;
    std::vector<uint8_t> frame_data = {0x99, 0x99, 0x99, 0X99, 0X99, 0X99, 0X99, 0X99, 0X99, 0X99};  
    
    bool isMultiFrame = true;
     
    /* redirect stdout to capture the output */
    testing::internal::CaptureStdout();    
    handler.processFrameData(testFrame.can_id, sid, frame_data, isMultiFrame);
    std::string output2= testing::internal::GetCapturedStdout();
    /* check if the correct message is printed */
    EXPECT_NE(output2.find("TransferData called with multiple frames."), std::string::npos);         
} 

/* Test if the frame type is invalid */
TEST_F(HandleFramesTest, InvalidFrameTypeTest){
    /* Set an arbitrary CAN ID */
    testFrame.can_id= 0;
   /* Set the Data Length Code to at least 2 to access data[1] */
    testFrame.can_dlc = 2;
    testFrame.data[0] = 0x20;          
   
    /* redirect stdout to capture the output */
    testing::internal::CaptureStdout();    
    handler.handleFrame(testFrame);
    std::string output = testing::internal::GetCapturedStdout();
    /* check if the correct message is printed */
    EXPECT_NE(output.find("Invalid frame type"), std::string::npos);                    
}

int main(int argc, char **argv)
{
testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}