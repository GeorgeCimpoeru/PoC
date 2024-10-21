#include "gtest/gtest.h"
#include "../include/HandleFrames.h"
#include "../../uds/diagnostic_session_control/include/DiagnosticSessionControl.h"
#include <linux/can.h>
#include "../include/MCULogger.h"

Logger logger;
int skt = 1;
const int FRAME_ID = 0xFA10;

class HandleFramesTest : public ::testing::Test{
    public:
    HandleFramesTest():handler(skt, logger), dsc(logger, skt)
    {
        
    };
    protected:
    HandleFrames handler;
    DiagnosticSessionControl dsc;
};

struct can_frame createFrame(std::vector<uint8_t> test_data)
{
    struct can_frame result_frame;
    result_frame.can_id = (FRAME_ID & CAN_EFF_MASK) | CAN_EFF_FLAG;
    int i=0;
    for (auto d : test_data)
        result_frame.data[i++] = d;
    result_frame.can_dlc = test_data.size();
    return result_frame;
}

/* Test for HandleFrameNegativeResponse */
TEST_F(HandleFramesTest, HandleFrameNegativeResponse)
{
    dsc.sessionControl(0xFA10, 0x02, true);
    struct can_frame testFrame = createFrame({0x03,0x7F,0x10, 0x12});
    testing::internal::CaptureStdout();
    handler.handleFrame(skt, testFrame);
    std::string output = testing::internal::GetCapturedStdout();
    /* check if the correct message is printed */
    EXPECT_NE(output.find("Single Frame received:"), std::string::npos);
    EXPECT_NE(output.find("DiagnosticSessionControl called."), std::string::npos);
}

/* Test for HandleFrameEcuReset */
TEST_F(HandleFramesTest, HandleFrameEcuReset)
{
    struct can_frame testFrame = createFrame({0x02,0x11, 0x02});
    testing::internal::CaptureStdout();
    handler.handleFrame(skt, testFrame);
    std::string output = testing::internal::GetCapturedStdout();
    /* check if the correct message is printed */
    EXPECT_NE(output.find("Single Frame received:"), std::string::npos);
    EXPECT_NE(output.find("Service 0x11 EcuReset called"), std::string::npos);
    EXPECT_NE(output.find("sub_function: 2"), std::string::npos);
    
}

/* Test for InvalidFrameType */
TEST_F(HandleFramesTest, InvalidFrameType)
{
    struct can_frame testFrame = createFrame({0x31,0x11, 0x02});
    testing::internal::CaptureStdout();
    handler.handleFrame(skt, testFrame);
    std::string output = testing::internal::GetCapturedStdout();
    /* check if the correct message is printed */
    EXPECT_NE(output.find("Invalid frame type."), std::string::npos);
    
}

/* Test for WrongSessionSecurity */
TEST_F(HandleFramesTest, WrongSessionSecurity)
{
    dsc.sessionControl(0xFA10, 0x01, true);
    struct can_frame testFrame = createFrame({0x02,0x27, 0x01});
    testing::internal::CaptureStdout();
    handler.handleFrame(skt, testFrame);
    std::string output = testing::internal::GetCapturedStdout();
    /* check if the correct message is printed */
    EXPECT_NE(output.find("Single Frame received:"), std::string::npos);
    EXPECT_NE(output.find("Subfunction not supported in active session."), std::string::npos);
    
}

/* Test for SecurityTest */
TEST_F(HandleFramesTest, SecurityTest)
{
    struct can_frame testFrame = createFrame({0x02,0x27, 0x01});
    dsc.sessionControl(0xFA10, 0x02, true);
    testing::internal::CaptureStdout();
    handler.handleFrame(skt, testFrame);
    std::string output = testing::internal::GetCapturedStdout();
    /* check if the correct message is printed */
    EXPECT_NE(output.find("Single Frame received:"), std::string::npos);
    EXPECT_NE(output.find("SecurityAccess called."), std::string::npos);
    
}

/* Test for TesterPresent */
TEST_F(HandleFramesTest, TesterPresent)
{
    struct can_frame testFrame = createFrame({0x01,0x3E});
    testing::internal::CaptureStdout();
    handler.handleFrame(skt, testFrame);
    std::string output = testing::internal::GetCapturedStdout();
    /* check if the correct message is printed */
    EXPECT_NE(output.find("Single Frame received:"), std::string::npos);
    EXPECT_NE(output.find("TesterPresent called."), std::string::npos);
    
}

/* Test for AccessTimingParameter */
TEST_F(HandleFramesTest, AccessTimingParameter)
{
    struct can_frame testFrame = createFrame({0x02,0x83, 0x02});
    testing::internal::CaptureStdout();
    handler.handleFrame(skt, testFrame);
    std::string output = testing::internal::GetCapturedStdout();
    /* check if the correct message is printed */
    EXPECT_NE(output.find("Single Frame received:"), std::string::npos);
    EXPECT_NE(output.find("AccessTimingParameters called."), std::string::npos);
    
}

/* Test for AccessTimingParameter2 */
TEST_F(HandleFramesTest, AccessTimingParameter2)
{
    struct can_frame testFrame = createFrame({0x06,0x83, 0x03, 0x00, 0x090, 0x01, 0x50});
    testing::internal::CaptureStdout();
    handler.handleFrame(skt, testFrame);
    std::string output = testing::internal::GetCapturedStdout();
    /* check if the correct message is printed */
    EXPECT_NE(output.find("Single Frame received:"), std::string::npos);
    EXPECT_NE(output.find("AccessTimingParameters called."), std::string::npos);
    
}

/* Test for ReadDataByIdentifier */
TEST_F(HandleFramesTest, ReadDataByIdentifier)
{
    struct can_frame testFrame = createFrame({0x03,0x22, 0x01, 0xa0});
    testing::internal::CaptureStdout();
    handler.handleFrame(skt, testFrame);
    std::string output = testing::internal::GetCapturedStdout();
    /* check if the correct message is printed */
    EXPECT_NE(output.find("Single Frame received:"), std::string::npos);
    EXPECT_NE(output.find("ReadDataByIdentifier called."), std::string::npos);
    
}

/* Test for ReadMemoryByAdressNRC */
TEST_F(HandleFramesTest, ReadMemoryByAdressNRC)
{
    struct can_frame testFrame = createFrame({0x03,0x7F, 0x23, 0x12});
    testing::internal::CaptureStdout();
    handler.handleFrame(skt, testFrame);
    std::string output = testing::internal::GetCapturedStdout();
    /* check if the correct message is printed */
    EXPECT_NE(output.find("Single Frame received:"), std::string::npos);
    EXPECT_NE(output.find("Negative Response received."), std::string::npos);
    
}
/* Test for ReadMemoryByAdress */
TEST_F(HandleFramesTest, ReadMemoryByAdress)
{
    struct can_frame testFrame = createFrame({0x03,0x23, 0x01, 0xaa});
    testing::internal::CaptureStdout();
    handler.handleFrame(skt, testFrame);
    std::string output = testing::internal::GetCapturedStdout();
    /* check if the correct message is printed */
    EXPECT_NE(output.find("Single Frame received:"), std::string::npos);
    EXPECT_NE(output.find("ReadMemoryByAddress called."), std::string::npos);
    
}
/* Test for WriteDataByIdentifier */
TEST_F(HandleFramesTest, WriteDataByIdentifier)
{
    struct can_frame testFrame = createFrame({0x04,0x2e, 0x01, 0xa0, 0x11});
    testing::internal::CaptureStdout();
    handler.handleFrame(skt, testFrame);
    std::string output = testing::internal::GetCapturedStdout();
    /* check if the correct message is printed */
    EXPECT_NE(output.find("Single Frame received:"), std::string::npos);
    EXPECT_NE(output.find("WriteDataByIdentifier service called!"), std::string::npos);
    
}
/* Test for ClearDiagnosticInformation */
TEST_F(HandleFramesTest, ClearDiagnosticInformation)
{
    struct can_frame testFrame = createFrame({0x02,0x14, 0xff});
    testing::internal::CaptureStdout();
    handler.handleFrame(skt, testFrame);
    std::string output = testing::internal::GetCapturedStdout();
    /* check if the correct message is printed */
    EXPECT_NE(output.find("Single Frame received:"), std::string::npos);
    EXPECT_NE(output.find("ClearDiagnosticInformation called."), std::string::npos);
    
}
/* Test for ReadDtcInformation */
TEST_F(HandleFramesTest, ReadDtcInformation)
{
    struct can_frame testFrame = createFrame({0x02,0x19, 0x02, 0xff});
    testing::internal::CaptureStdout();
    handler.handleFrame(skt, testFrame);
    std::string output = testing::internal::GetCapturedStdout();
    /* check if the correct message is printed */
    EXPECT_NE(output.find("Single Frame received:"), std::string::npos);
    EXPECT_NE(output.find("ReadDtcInformation called."), std::string::npos);
    
}
/* Test for RoutineControl */
TEST_F(HandleFramesTest, RoutineControl)
{
    struct can_frame testFrame = createFrame({0x02,0x31, 0x02,0x01, 0x01});
    testing::internal::CaptureStdout();
    handler.handleFrame(skt, testFrame);
    std::string output = testing::internal::GetCapturedStdout();
    /* check if the correct message is printed */
    EXPECT_NE(output.find("Single Frame received:"), std::string::npos);
    EXPECT_NE(output.find("RoutineControl called."), std::string::npos);
}
/* Test for DiagnosticSessionResponse */
TEST_F(HandleFramesTest, DiagnosticSessionControlResponse)
{
    struct can_frame testFrame = createFrame({0x01, 0x50});
    testing::internal::CaptureStdout();
    handler.handleFrame(skt, testFrame);
    std::string output = testing::internal::GetCapturedStdout();
    
    EXPECT_NE(output.find("Single Frame received:"), std::string::npos);
    EXPECT_NE(output.find("Response for DiagnosticSessionControl received."), std::string::npos);
}

/* Test for ECUResetResponse */
TEST_F(HandleFramesTest, ECUResetResponse)
{
    struct can_frame testFrame = createFrame({0x01, 0x51});
    testing::internal::CaptureStdout();
    handler.handleFrame(skt, testFrame);
    std::string output = testing::internal::GetCapturedStdout();
    
    EXPECT_NE(output.find("Single Frame received:"), std::string::npos);
    EXPECT_NE(output.find("Response for ECUReset received."), std::string::npos);
}

/* Test for SecurityAccessResponse */
TEST_F(HandleFramesTest, SecurityAccessResponse)
{
    struct can_frame testFrame = createFrame({0x01, 0x67});
    testing::internal::CaptureStdout();
    handler.handleFrame(skt, testFrame);
    std::string output = testing::internal::GetCapturedStdout();
    
    EXPECT_NE(output.find("Single Frame received:"), std::string::npos);
    EXPECT_NE(output.find("Response from SecurityAccess received."), std::string::npos);
}

/* Test for AuthenticationResponse */
TEST_F(HandleFramesTest, AuthenticationResponse)
{
    struct can_frame testFrame = createFrame({0x01, 0x69});
    testing::internal::CaptureStdout();
    handler.handleFrame(skt, testFrame);
    std::string output = testing::internal::GetCapturedStdout();
    
    EXPECT_NE(output.find("Single Frame received:"), std::string::npos);
    EXPECT_NE(output.find("Response from Authentication received."), std::string::npos);
}

/* Test for TesterPresentResponse */
TEST_F(HandleFramesTest, TesterPresentResponse)
{
    struct can_frame testFrame = createFrame({0x01, 0x7E});
    testing::internal::CaptureStdout();
    handler.handleFrame(skt, testFrame);
    std::string output = testing::internal::GetCapturedStdout();
    
    EXPECT_NE(output.find("Single Frame received:"), std::string::npos);
    EXPECT_NE(output.find("Response from TesterPresent received."), std::string::npos);
}

/* Test for AccessTimingParametersResponse */
TEST_F(HandleFramesTest, AccessTimingParametersResponse)
{
    struct can_frame testFrame = createFrame({0x01, 0xC3});
    testing::internal::CaptureStdout();
    handler.handleFrame(skt, testFrame);
    std::string output = testing::internal::GetCapturedStdout();
    
    EXPECT_NE(output.find("Single Frame received:"), std::string::npos);
    EXPECT_NE(output.find("Response from AccessTimingParameters received."), std::string::npos);
}

/* Test for ReadDataByIdentifierResponse */
TEST_F(HandleFramesTest, ReadDataByIdentifierResponse)
{
    struct can_frame testFrame = createFrame({0x01, 0x62});
    testing::internal::CaptureStdout();
    handler.handleFrame(skt, testFrame);
    std::string output = testing::internal::GetCapturedStdout();
    
    EXPECT_NE(output.find("Single Frame received:"), std::string::npos);
    EXPECT_NE(output.find("Response from ReadDataByIdentifier received in one frame."), std::string::npos);
}

/* Test for ReadDataByIdentifierLongResponse */
TEST_F(HandleFramesTest, ReadDataByIdentifierLongResponse)
{
    testing::internal::CaptureStdout();
    handler.processFrameData(skt, 0xFA10, 0x62, {0x01, 0x62}, true);
    std::string output = testing::internal::GetCapturedStdout();
    
    EXPECT_NE(output.find("Response from ReadDataByIdentifier received."), std::string::npos);
    EXPECT_NE(output.find("Received multiple frames containing 2 bytes of data"), std::string::npos);
}

/* Test for ReadMemoryByAdressResponse */
TEST_F(HandleFramesTest, ReadMemoryByAdressResponse)
{
    struct can_frame testFrame = createFrame({0x01, 0x63});
    testing::internal::CaptureStdout();
    handler.handleFrame(skt, testFrame);
    std::string output = testing::internal::GetCapturedStdout();
    
    EXPECT_NE(output.find("Single Frame received:"), std::string::npos);
    EXPECT_NE(output.find("Response from ReadMemoryByAdress received in one frame."), std::string::npos);
}

/* Test for ReadMemoryByAdressLongResponse */
TEST_F(HandleFramesTest, ReadMemoryByAdressLongResponse)
{
    testing::internal::CaptureStdout();
    handler.processFrameData(skt, 0xFA10, 0x63, {0x01, 0x63}, true);
    std::string output = testing::internal::GetCapturedStdout();
    
    EXPECT_NE(output.find("Response from ReadMemoryByAdress received."), std::string::npos);
    EXPECT_NE(output.find("Received multiple frames containing 2 bytes of data"), std::string::npos);
}

/* Test for WriteDataByIdentifierResponse */
TEST_F(HandleFramesTest, WriteDataByIdentifierResponse)
{
    struct can_frame testFrame = createFrame({0x01, 0x6E});
    testing::internal::CaptureStdout();
    handler.handleFrame(skt, testFrame);
    std::string output = testing::internal::GetCapturedStdout();
    
    EXPECT_NE(output.find("Single Frame received:"), std::string::npos);
    EXPECT_NE(output.find("Response from WriteDataByIdentifier received."), std::string::npos);
}

/* Test for ClearDiagnosticInformationResponse */
TEST_F(HandleFramesTest, ClearDiagnosticInformationResponse)
{
    struct can_frame testFrame = createFrame({0x01, 0x54});
    testing::internal::CaptureStdout();
    handler.handleFrame(skt, testFrame);
    std::string output = testing::internal::GetCapturedStdout();
    
    EXPECT_NE(output.find("Single Frame received:"), std::string::npos);
    EXPECT_NE(output.find("Response from ClearDiagnosticInformation received."), std::string::npos);
}

/* Test for ReadDtcInformationResponse */
TEST_F(HandleFramesTest, ReadDtcInformationResponse)
{
    struct can_frame testFrame = createFrame({0x01, 0x59});
    testing::internal::CaptureStdout();
    handler.handleFrame(skt, testFrame);
    std::string output = testing::internal::GetCapturedStdout();
    
    EXPECT_NE(output.find("Single Frame received:"), std::string::npos);
    EXPECT_NE(output.find("Response from ReadDtcInformation received."), std::string::npos);
}

/* Test for RoutineControlResponse */
TEST_F(HandleFramesTest, RoutineControlResponse)
{
    struct can_frame testFrame = createFrame({0x01, 0x71});
    testing::internal::CaptureStdout();
    handler.handleFrame(skt, testFrame);
    std::string output = testing::internal::GetCapturedStdout();
    
    EXPECT_NE(output.find("Single Frame received:"), std::string::npos);
    EXPECT_NE(output.find("Response from RoutineControl received."), std::string::npos);
}

/* Test for RequestDownload */
TEST_F(HandleFramesTest, RequestDownload)
{
    struct can_frame testFrame = createFrame({0x02,0x34, 0x02});
    testing::internal::CaptureStdout();
    handler.handleFrame(skt, testFrame);
    std::string output = testing::internal::GetCapturedStdout();
    /* check if the correct message is printed */
    EXPECT_NE(output.find("Single Frame received:"), std::string::npos);
    EXPECT_NE(output.find("RequestDownload called."), std::string::npos);
}

/* Test for WrongSessionRequestDownload */
TEST_F(HandleFramesTest, WrongSessionRequestDownload)
{
    dsc.sessionControl(0xFA10, 0x01, true);
    struct can_frame testFrame = createFrame({0x02,0x34, 0x02});
    testing::internal::CaptureStdout();
    handler.handleFrame(skt, testFrame);
    std::string output = testing::internal::GetCapturedStdout();
    /* check if the correct message is printed */
    EXPECT_NE(output.find("Single Frame received:"), std::string::npos);
    EXPECT_NE(output.find("Subfunction not supported in active session."), std::string::npos);
}

/* Test for WrongSessionTransferData */
TEST_F(HandleFramesTest, WrongSessionTransferData)
{
    dsc.sessionControl(0xFA10, 0x01, true);
    struct can_frame testFrame = createFrame({0x02,0x36, 0x02});
    testing::internal::CaptureStdout();
    handler.handleFrame(skt, testFrame);
    std::string output = testing::internal::GetCapturedStdout();
    /* check if the correct message is printed */
    EXPECT_NE(output.find("Single Frame received:"), std::string::npos);
    EXPECT_NE(output.find("Subfunction not supported in active session."), std::string::npos);
}

// /* Test for TransferData */
// TEST_F(HandleFramesTest, TransferData)
// {
//     struct can_frame testFrame = createFrame({0x02,0x36, 0x02});
//     testing::internal::CaptureStdout();
//     handler.handleFrame(skt, testFrame);
//     std::string output = testing::internal::GetCapturedStdout();
//     /* check if the correct message is printed */
//     EXPECT_NE(output.find("Single Frame received:"), std::string::npos);
//     EXPECT_NE(output.find("TransferData called with one frame."), std::string::npos);
// }

/* Test for WrongSessionRequestTransferExit */
TEST_F(HandleFramesTest, WrongSessionRequestTransferExit)
{
    dsc.sessionControl(0xFA10, 0x01, true);
    struct can_frame testFrame = createFrame({0x02,0x37, 0x02});
    testing::internal::CaptureStdout();
    handler.handleFrame(skt, testFrame);
    std::string output = testing::internal::GetCapturedStdout();
    /* check if the correct message is printed */
    EXPECT_NE(output.find("Single Frame received:"), std::string::npos);
    EXPECT_NE(output.find("Subfunction not supported in active session."), std::string::npos);
}

/* Test for RequestTransferExit */
TEST_F(HandleFramesTest, RequestTransferExit)
{
    dsc.sessionControl(0xFA10, 0x02, true);
    struct can_frame testFrame = createFrame({0x02,0x37, 0x02});
    testing::internal::CaptureStdout();
    handler.handleFrame(skt, testFrame);
    std::string output = testing::internal::GetCapturedStdout();
    /* check if the correct message is printed */
    EXPECT_NE(output.find("Single Frame received:"), std::string::npos);
    EXPECT_NE(output.find("Request Transfer Exit Service 0x37 called"), std::string::npos);
}

/* Test for TransferDataMultipleFrame */
TEST_F(HandleFramesTest, TransferDataMultipleFrame)
{
    testing::internal::CaptureStdout();
    handler.processFrameData(skt, 0xFA10, 0x36, {0x02,0x36, 0x02}, true);
    std::string output = testing::internal::GetCapturedStdout();
    /* check if the correct message is printed */
    EXPECT_NE(output.find("TransferData called with multiple frames."), std::string::npos);
}

// /* Test for RequestUpdateStatus */
// TEST_F(HandleFramesTest, RequestUpdateStatus)
// {
//     struct can_frame testFrame = createFrame({0x02,0x32, 0x02});
//     testing::internal::CaptureStdout();
//     handler.handleFrame(skt, testFrame);
//     std::string output = testing::internal::GetCapturedStdout();
//     /* check if the correct message is printed */
//     EXPECT_NE(output.find("Single Frame received:"), std::string::npos);
//     EXPECT_NE(output.find("RequestUpdateStatus called."), std::string::npos);
// }

/* Test for RequestDownload */
TEST_F(HandleFramesTest, WrongSessionRequestUpdateStatus)
{
    dsc.sessionControl(0xFA10, 0x01, true);
    struct can_frame testFrame = createFrame({0x02,0x32, 0x02});
    testing::internal::CaptureStdout();
    handler.handleFrame(skt, testFrame);
    std::string output = testing::internal::GetCapturedStdout();
    /* check if the correct message is printed */
    EXPECT_NE(output.find("Single Frame received:"), std::string::npos);
    EXPECT_NE(output.find("Subfunction not supported in active session."), std::string::npos);
}

TEST_F(HandleFramesTest, RequestDownloadResponse)
{
    struct can_frame testFrame = createFrame({0x01, 0x74});
    testing::internal::CaptureStdout();
    handler.handleFrame(skt, testFrame);
    std::string output = testing::internal::GetCapturedStdout();

    EXPECT_NE(output.find("Single Frame received:"), std::string::npos);
    EXPECT_NE(output.find("Response from RequestDownload received."), std::string::npos);
}

TEST_F(HandleFramesTest, TransferDataResponse)
{
    struct can_frame testFrame = createFrame({0x01, 0x76});
    testing::internal::CaptureStdout();
    handler.handleFrame(skt, testFrame);
    std::string output = testing::internal::GetCapturedStdout();

    EXPECT_NE(output.find("Single Frame received:"), std::string::npos);
    EXPECT_NE(output.find("Response from TransferData received."), std::string::npos);
}

TEST_F(HandleFramesTest, RequestTransferExitResponse)
{
    struct can_frame testFrame = createFrame({0x01, 0x77});
    testing::internal::CaptureStdout();
    handler.handleFrame(skt, testFrame);
    std::string output = testing::internal::GetCapturedStdout();

    EXPECT_NE(output.find("Single Frame received:"), std::string::npos);
    EXPECT_NE(output.find("Response from RequestTransferExit received."), std::string::npos);
}

TEST_F(HandleFramesTest, RequestUpdateStatusResponse)
{
    struct can_frame testFrame = createFrame({0x01, 0x72});
    testing::internal::CaptureStdout();
    handler.handleFrame(skt, testFrame);
    std::string output = testing::internal::GetCapturedStdout();

    EXPECT_NE(output.find("Single Frame received:"), std::string::npos);
    EXPECT_NE(output.find("Response from RequestUpdateStatus received."), std::string::npos);
}

TEST_F(HandleFramesTest, UnknownResponse)
{
    struct can_frame testFrame = createFrame({0x01, 0xFF});
    testing::internal::CaptureStdout();
    handler.handleFrame(skt, testFrame);
    std::string output = testing::internal::GetCapturedStdout();

    EXPECT_NE(output.find("Single Frame received:"), std::string::npos);
    EXPECT_NE(output.find("Unknown request/response received."), std::string::npos);
}

TEST_F(HandleFramesTest, FirstFrame)
{
    struct can_frame testFrame = createFrame({0x10, 0x08,0x05});
    testing::internal::CaptureStdout();
    handler.handleFrame(skt, testFrame);
    std::string output = testing::internal::GetCapturedStdout();

    EXPECT_NE(output.find("Multi-frame Sequence with 2 frames"), std::string::npos);
}

TEST_F(HandleFramesTest, WrongConsecutiveFrames)
{
    struct can_frame testFrame = createFrame({0x22, 0x0F,0x05});
    testing::internal::CaptureStdout();
    handler.handleFrame(skt, testFrame);
    std::string output = testing::internal::GetCapturedStdout();
    EXPECT_NE(output.find("Consecutive frames received."), std::string::npos);
    EXPECT_NE(output.find("Invalid consecutive frame sequence: expected 33 but received 34"), std::string::npos);
}

TEST_F(HandleFramesTest, ConsecutiveFrames)
{
    struct can_frame testFrame = createFrame({0x21, 0x08,0x05,0x11,0x11,0x11,0x11,0x11,0x11,0x11,0x10,0x11});
    testing::internal::CaptureStdout();
    handler.handleFrame(skt, testFrame);
    std::string output = testing::internal::GetCapturedStdout();

    EXPECT_NE(output.find("Consecutive frames received."), std::string::npos);
}

int main(int argc, char **argv)
{
testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}