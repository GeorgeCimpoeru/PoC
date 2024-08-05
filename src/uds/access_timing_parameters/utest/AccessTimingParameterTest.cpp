#include <iostream>
#include <vector>
#include <cstdint>
#include <string>
#include <gtest/gtest.h>
#include <linux/can.h>
#include "../include/AccessTimingParameter.h"
#include "../../../utils/include/Logger.h"

/* Test Fixture for AccessTimingParameter */
class AccessTimingParameterTest : public ::testing::Test {
protected:
    int socket;
    Logger mockLogger;
    AccessTimingParameter* atp;

    void SetUp() override {
        socket = 1; // Example socket number
        atp = new AccessTimingParameter(mockLogger, socket);
    }

    void TearDown() override {
        delete atp;
    }
};

/* Test for Constructor Initializing Default Timing Parameters */
TEST_F(AccessTimingParameterTest, ConstructorInitializesDefaultTimingParameters) {
    EXPECT_EQ(AccessTimingParameter::p2_max_time, AccessTimingParameter::DEFAULT_P2_MAX_TIME);
    EXPECT_EQ(AccessTimingParameter::p2_star_max_time, AccessTimingParameter::DEFAULT_P2_STAR_MAX_TIME);
}

/* Test for Reading Extended Timing Parameters */
TEST_F(AccessTimingParameterTest, ReadExtendedTimingParameters) {
    testing::internal::CaptureStdout();
    atp->readExtendedTimingParameters(0x101);
    std::string output = testing::internal::GetCapturedStdout();

    EXPECT_NE(output.find("DEFAULT P2 MAX TIME: 40 in miliseconds"), std::string::npos);
    EXPECT_NE(output.find("DEFAULT P2 STAR MAX TIME: 400 in miliseconds"), std::string::npos);
}

/* Test for Setting Timing Parameters to Default */
TEST_F(AccessTimingParameterTest, SetTimingParametersToDefault) {
    AccessTimingParameter::p2_max_time = 50;
    AccessTimingParameter::p2_star_max_time = 500;
    
    testing::internal::CaptureStdout();
    atp->setTimingParametersToDefault(0x101);
    std::string output = testing::internal::GetCapturedStdout();

    EXPECT_EQ(AccessTimingParameter::p2_max_time, AccessTimingParameter::DEFAULT_P2_MAX_TIME);
    EXPECT_EQ(AccessTimingParameter::p2_star_max_time, AccessTimingParameter::DEFAULT_P2_STAR_MAX_TIME);

    EXPECT_NE(output.find("Updated P2 Max Time to default: 40 in miliseconds"), std::string::npos);
    EXPECT_NE(output.find("Updated P2 Star Max Time to default: 400 in miliseconds"), std::string::npos);
}

/* Test for Reading Currently Active Timing Parameters */
TEST_F(AccessTimingParameterTest, ReadCurrentlyActiveTimingParameters) {
    AccessTimingParameter::p2_max_time = 60;
    AccessTimingParameter::p2_star_max_time = 600;
    
    testing::internal::CaptureStdout();
    atp->readCurrentlyActiveTimingParameters(0x101);
    std::string output = testing::internal::GetCapturedStdout();

    EXPECT_NE(output.find("Current P2 Max Time: 60 in miliseconds"), std::string::npos);
    EXPECT_NE(output.find("Current P2 Star Max Time: 600 in miliseconds"), std::string::npos);
}

/* Test for Setting Timing Parameters */
TEST_F(AccessTimingParameterTest, SetTimingParameters) {
    std::vector<uint8_t> frame_data = {0x00, 0x3C, 0x01, 0x90}; // 60 ms and 400 ms
    atp->setTimingParameters(0x101, frame_data);

    EXPECT_EQ(AccessTimingParameter::p2_max_time, 60);
    EXPECT_EQ(AccessTimingParameter::p2_star_max_time, 400);
}

/* Test for Handling Request with Unsupported Sub-Function */
TEST_F(AccessTimingParameterTest, HandleRequestUnsupportedSubFunction) {
    testing::internal::CaptureStdout();
    atp->handleRequest(0x101, 0x99, {});
    std::string output = testing::internal::GetCapturedStdout();

    EXPECT_NE(output.find("Unsupported sub-function"), std::string::npos);
}

/* Test for Handling Request with Incorrect Message Length */
TEST_F(AccessTimingParameterTest, HandleRequestIncorrectMessageLength) {
    testing::internal::CaptureStdout();
    std::vector<uint8_t> frame_data = {0x00, 0x3C}; // Incorrect length
    atp->handleRequest(0x101, 0x04, frame_data);
    std::string output = testing::internal::GetCapturedStdout();

    EXPECT_NE(output.find("Incorrect Message Length Or Invalid Format"), std::string::npos);
}

/* Test for Handling Request with Read Extended Timing Parameters */
TEST_F(AccessTimingParameterTest, HandleRequestReadExtendedTimingParameters) {
    testing::internal::CaptureStdout();
    atp->handleRequest(0x101, 0x01, {});
    std::string output = testing::internal::GetCapturedStdout();

    EXPECT_NE(output.find("Read Extended Timing Parameters method invoked"), std::string::npos);
}

/* Test for Handling Request with Set Timing Parameters to Default */
TEST_F(AccessTimingParameterTest, HandleRequestSetTimingParametersToDefault) {
    testing::internal::CaptureStdout();
    atp->handleRequest(0x101, 0x02, {});
    std::string output = testing::internal::GetCapturedStdout();

    EXPECT_NE(output.find("Read Extended Timing Parameters method invoked"), std::string::npos);
}

/* Test for Handling Request with Read Currently Active Timing Parameters */
TEST_F(AccessTimingParameterTest, HandleRequestReadCurrentlyActiveTimingParameters) {
    testing::internal::CaptureStdout();
    atp->handleRequest(0x101, 0x03, {});
    std::string output = testing::internal::GetCapturedStdout();

    EXPECT_NE(output.find("Read Currently Active Timing Parameters method invoked"), std::string::npos);
}

/* Test for Handling Request with Set Timing Parameters */
TEST_F(AccessTimingParameterTest, HandleRequestSetTimingParameters) {
    std::vector<uint8_t> frame_data = {0x00, 0x3C, 0x01, 0x90}; // 60 ms and 400 ms
    testing::internal::CaptureStdout();
    atp->handleRequest(0x101, 0x04, frame_data);
    std::string output = testing::internal::GetCapturedStdout();

    EXPECT_NE(output.find("Set Timing Parameters method invoked"), std::string::npos);
}

int main(int argc, char **argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
