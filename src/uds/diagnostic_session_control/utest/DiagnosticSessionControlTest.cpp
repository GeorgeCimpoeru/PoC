#include <iostream>
#include <vector>
#include <cstdint>
#include <string>
#include <gtest/gtest.h>
#include <linux/can.h>
#include "../include/DiagnosticSessionControl.h"
#include "../../../utils/include/Logger.h"

/* Test Fixture for DiagnosticSessionControl */
class DiagnosticSessionControlTest : public ::testing::Test {
protected:
    int socket;
    Logger mockLogger;
    DiagnosticSessionControl* dsc;

    void SetUp() override {
        socket = 1; // Example socket number
        dsc = new DiagnosticSessionControl(&mockLogger, socket);
    }

    void TearDown() override {
        delete dsc;
    }
};

/* Test for Default Constructor Initializing Default Session */
TEST_F(DiagnosticSessionControlTest, DefaultConstructorInitializesDefaultSession) {
    EXPECT_EQ(dsc->getCurrentSession(), DEFAULT_SESSION);
}

/* Test for Parameterized Constructor Initializing Default Session */
TEST_F(DiagnosticSessionControlTest, ParameterizedConstructorInitializesDefaultSession) {
    DiagnosticSessionControl dscWithModuleId(0x11, &mockLogger, socket);
    EXPECT_EQ(dscWithModuleId.getCurrentSession(), DEFAULT_SESSION);
}

/* Test for Switching to Default Session */
TEST_F(DiagnosticSessionControlTest, SwitchToDefaultSession) {
    dsc->sessionControl(0x1011, SUB_FUNCTION_DEFAULT_SESSION);
    EXPECT_EQ(dsc->getCurrentSession(), DEFAULT_SESSION);
}

/* Test for Switching to Programming Session */
TEST_F(DiagnosticSessionControlTest, SwitchToProgrammingSession) {
    dsc->sessionControl(0x1011, SUB_FUNCTION_PROGRAMMING_SESSION);
    EXPECT_EQ(dsc->getCurrentSession(), PROGRAMMING_SESSION);
}

/* Test for Unsupported Sub-Function */
TEST_F(DiagnosticSessionControlTest, UnsupportedSubFunction) {
    std::cerr << "Running TestUnsupportedSubFunction" << std::endl;

    testing::internal::CaptureStdout();
    dsc->sessionControl(0x1011, 0x99);

    std::string output = testing::internal::GetCapturedStdout();
    EXPECT_NE(output.find("Unsupported sub-function"), std::string::npos);
    std::cerr << "Finished TestUnsupportedSubFunction" << std::endl;
}

/* Test for Sending Negative Response */
TEST_F(DiagnosticSessionControlTest, SendNegativeResponse) {
    std::cerr << "Running TestSendNegativeResponse" << std::endl;
    
    testing::internal::CaptureStdout();
    dsc->sendNegativeResponse(NR_AUTHENTICATION_FAILED);

    std::string output = testing::internal::GetCapturedStdout();
    EXPECT_NE(output.find("Sending Negative Response: 52"), std::string::npos);
    std::cerr << "Finished TestSendNegativeResponse" << std::endl;
}

/* Test for Valid Session Switch to Default Session */
TEST_F(DiagnosticSessionControlTest, ValidSessionSwitchToDefaultSession) {
    std::cerr << "Running TestValidSessionSwitchToDefaultSession" << std::endl;

    testing::internal::CaptureStdout();
    dsc->sessionControl(0x1011, SUB_FUNCTION_DEFAULT_SESSION);

    std::string output = testing::internal::GetCapturedStdout();
    EXPECT_NE(output.find("Switched to Default Session"), std::string::npos);
    std::cerr << "Finished TestValidSessionSwitchToDefaultSession" << std::endl;
}

/* Test for Switching to Default Session with Specific Module ID */
TEST_F(DiagnosticSessionControlTest, SwitchToDefaultSessionWithModuleId) {
    std::cerr << "Running SwitchToDefaultSessionWithModuleId" << std::endl;

    DiagnosticSessionControl dscWithModuleId(0x11, &mockLogger, socket);

    testing::internal::CaptureStdout();
    dscWithModuleId.sessionControl(0x1011, SUB_FUNCTION_DEFAULT_SESSION);

    std::string output = testing::internal::GetCapturedStdout();
    EXPECT_NE(output.find("Sent pozitive response frame to ECU"), std::string::npos);
    std::cerr << "Finished SwitchToDefaultSessionWithModuleId" << std::endl;
}


/* Test for Valid Session Switch to Programming Session */
TEST_F(DiagnosticSessionControlTest, ValidSessionSwitchToProgrammingSession) {
    std::cerr << "Running TestValidSessionSwitchToProgrammingSession" << std::endl;

    testing::internal::CaptureStdout();
    dsc->sessionControl(0x1011, SUB_FUNCTION_PROGRAMMING_SESSION);

    std::string output = testing::internal::GetCapturedStdout();
    EXPECT_NE(output.find("Switched to Programming Session"), std::string::npos);
    std::cerr << "Finished TestValidSessionSwitchToProgrammingSession" << std::endl;
}

/* Test for Switching to Programming Session with Specific Module ID */
TEST_F(DiagnosticSessionControlTest, SwitchToProgrammingSessionWithModuleId) {
    std::cerr << "Running SwitchToProgrammingSessionWithModuleId" << std::endl;

    DiagnosticSessionControl dscWithModuleId(0x11, &mockLogger, socket);

    testing::internal::CaptureStdout();
    dscWithModuleId.sessionControl(0x1011, SUB_FUNCTION_PROGRAMMING_SESSION);

    std::string output = testing::internal::GetCapturedStdout();
    EXPECT_NE(output.find("Sent pozitive response frame to ECU"), std::string::npos);
    std::cerr << "Finished SwitchToProgrammingSessionWithModuleId" << std::endl;
}


int main(int argc, char **argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
