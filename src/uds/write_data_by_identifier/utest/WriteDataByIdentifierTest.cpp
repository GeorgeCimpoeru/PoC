#include <iostream>
#include <vector>
#include <cstdint>
#include <string>
#include <gtest/gtest.h>
#include <linux/can.h>
#include "../include/WriteDataByIdentifier.h"
#include "../../../utils/include/Logger.h"

/* Test Fixture for WriteDataByIdentifier */
class WriteDataByIdentifierTest : public ::testing::Test {
protected:
    canid_t frame_id;
    std::vector<uint8_t> frame_data;
    Logger mockLogger;
    WriteDataByIdentifier* writeDataByIdentifier;

    void SetUp() override {
        /* Example frame ID */
        frame_id = 0x10FA;
        /* Example frame data */
        frame_data = {0x2E, 0x00, 0x01, 0x02, 0x03};
        writeDataByIdentifier = new WriteDataByIdentifier(frame_id, frame_data, mockLogger);
    }

    void TearDown() override {
        delete writeDataByIdentifier;
    }
};

/* Test for Incorrect Message Length */
TEST_F(WriteDataByIdentifierTest, IncorrectMessageLength) {
    std::cerr << "Running TestIncorrectMessageLength" << std::endl;
    
    std::vector<uint8_t> invalid_frame_data = {0x2E, 0x00};

    testing::internal::CaptureStdout();
    WriteDataByIdentifier writeDataByIdentifier(frame_id, invalid_frame_data, mockLogger);

    std::string output = testing::internal::GetCapturedStdout();
    EXPECT_NE(output.find("Write Data By Identifier Service invoked."), std::string::npos);
    EXPECT_NE(output.find("Incorrect Message Length or Invalid Format"), std::string::npos);
    std::cerr << "Finished TestIncorrectMessageLength" << std::endl;
}

/* Test for Valid Data Identifier in MCUModule on API socket*/
TEST_F(WriteDataByIdentifierTest, ValidDIDInMCUModuleOnAPISocket) {
    std::cerr << "Running TestValidDIDInMCUModuleOnAPISocket" << std::endl;

    frame_id = 0xFA10;
    frame_data = {0x05, 0x2E, 0x01, 0xA0, 0x03, 0x04};

    testing::internal::CaptureStdout();
    WriteDataByIdentifier writeDataByIdentifier(frame_id, frame_data, mockLogger);

    std::string output = testing::internal::GetCapturedStdout();

    EXPECT_NE(output.find("Write Data By Identifier Service invoked."), std::string::npos);
    EXPECT_NE(output.find("Data written to new DID 0x01a0 in MCUModule."), std::string::npos);
    std::cerr << "Finished TestValidDIDInMCUModuleOnAPISocket" << std::endl;
}

/* Test for Valid Data Identifier in MCUModule on CANBus socket*/
TEST_F(WriteDataByIdentifierTest, ValidDIDInMCUModuleOnCANBusSocket) {
    std::cerr << "Running TestValidDIDInMCUModuleOnCANBusSocket" << std::endl;

    frame_id = 0x1110;
    frame_data = {0x05, 0x2E, 0x01, 0xA0, 0x03, 0x04};

    testing::internal::CaptureStdout();
    WriteDataByIdentifier writeDataByIdentifier(frame_id, frame_data, mockLogger);

    std::string output = testing::internal::GetCapturedStdout();

    EXPECT_NE(output.find("Write Data By Identifier Service invoked."), std::string::npos);
    EXPECT_NE(output.find("Data written to DID 0x01a0 in MCUModule."), std::string::npos);
    std::cerr << "Finished TestValidDIDInMCUModuleOnCANBusSocket" << std::endl;
}

/* Test for Valid Data Identifier in BatteryModule */
TEST_F(WriteDataByIdentifierTest, ValidDIDInBatteryModule) {
    std::cerr << "Running TestValidDIDInBatteryModule" << std::endl;

    frame_id = 0xFA11;
    frame_data = {0x05, 0x2E, 0x01, 0xA0, 0x03, 0x04};

    testing::internal::CaptureStdout();
    WriteDataByIdentifier writeDataByIdentifier(frame_id, frame_data, mockLogger);

    std::string output = testing::internal::GetCapturedStdout();

    EXPECT_NE(output.find("Write Data By Identifier Service invoked."), std::string::npos);
    EXPECT_NE(output.find("Data written to DID 0x01a0 in BatteryModule."), std::string::npos);
    std::cerr << "Finished TestValidDIDInBatteryModule" << std::endl;
}

/* Test for New Valid Data Identifier in BatteryModule */
TEST_F(WriteDataByIdentifierTest, NewValidDIDInBatteryModule) {
    std::cerr << "Running TestNewValidDIDInBatteryModule" << std::endl;

    frame_id = 0xFA11;
    frame_data = {0x05, 0x2E, 0xf1, 0x8c, 0x03, 0x04};

    testing::internal::CaptureStdout();
    WriteDataByIdentifier writeDataByIdentifier(frame_id, frame_data, mockLogger);

    std::string output = testing::internal::GetCapturedStdout();
    EXPECT_NE(output.find("Write Data By Identifier Service invoked."), std::string::npos);
    EXPECT_NE(output.find("Data written to new DID 0xf18c in BatteryModule."), std::string::npos);
    EXPECT_NE(output.find("BatteryModule contents:"), std::string::npos);
    EXPECT_NE(output.find("DID 0x01d0: 00"), std::string::npos);
    EXPECT_NE(output.find("DID 0x01c0: 00"), std::string::npos);
    EXPECT_NE(output.find("DID 0x01b0: 00"), std::string::npos);
    EXPECT_NE(output.find("DID 0xf18c: 03 04"), std::string::npos);
    EXPECT_NE(output.find("DID 0x01a0: 03 04"), std::string::npos);
    std::cerr << "Finished TestNewValidDIDInBatteryModule" << std::endl;
}

/* Test for Invalid Data Identifier */
TEST_F(WriteDataByIdentifierTest, InvalidDID) {
    std::cerr << "Running TestInvalidDID" << std::endl;
    
    frame_id = 0xFA10;
    std::vector<uint8_t> invalid_did_frame = {0x05, 0x2E, 0xff, 0xff, 0x03, 0x04};

    testing::internal::CaptureStdout();
    WriteDataByIdentifier writeDataByIdentifier(frame_id, invalid_did_frame, mockLogger);

    std::string output = testing::internal::GetCapturedStdout();
    EXPECT_NE(output.find("Write Data By Identifier Service invoked."), std::string::npos);
    EXPECT_NE(output.find("Request Out Of Range: Identifier not found in memory"), std::string::npos);
    std::cerr << "Finished TestInvalidDID" << std::endl;
}

int main(int argc, char **argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}