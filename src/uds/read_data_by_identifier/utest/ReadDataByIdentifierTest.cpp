#include <gtest/gtest.h>
#include "src/uds/read_data_by_identifier/include/ReadDataByIdentifier.h"
#include "src/ecu_simulation/BatteryModule/include/BatteryModule.h"
#include "src/mcu/include/MCUModule.h"

int socket_;
class ReadDataByIdentifierTest : public ::testing::Test {
protected:
    Logger* logger;
    ReadDataByIdentifier* rdbi;
    CreateInterface* create_interface;

    ReadDataByIdentifierTest()
    {
        logger = new Logger;
        rdbi = new ReadDataByIdentifier(socket_, logger);
        battery = new BatteryModule(0x00, 0x11);
        MCU::mcu = new MCU::MCUModule(0x01);
    }
    ~ReadDataByIdentifierTest()
    {
        delete logger;
        delete rdbi;
        MCU::mcu = nullptr;
        battery = nullptr;
    }
};
TEST_F(ReadDataByIdentifierTest, InvalidMessageLength) {
    canid_t can_id = 0x10; 
    MCU::mcu->mcu_data[0x1234] = {0x01, 0x02, 0x03, 0x04};
    /* ReadDataByIdentifier request for data identifier 0x1234 */
    std::vector<uint8_t> request = {0x22, 0x01, 0x12};

    std::vector<uint8_t> expected_response = {0x03, 0x7F, 0x22, 0x13};
    std::vector<uint8_t> actual_response = rdbi->readDataByIdentifier(can_id, request, true);

    EXPECT_EQ(expected_response, actual_response);
}

TEST_F(ReadDataByIdentifierTest, ValidRequestForMCUData) {
    canid_t can_id = 0x10; 
    MCU::mcu->mcu_data[0x1234] = {0x01, 0x02, 0x03, 0x04};
    /* ReadDataByIdentifier request for data identifier 0x1234 */
    std::vector<uint8_t> request = {0x22, 0x01, 0x12, 0x34};

    std::vector<uint8_t> expected_response = {0x01, 0x02, 0x03, 0x04};
    std::vector<uint8_t> actual_response = rdbi->readDataByIdentifier(can_id, request, true);

    EXPECT_EQ(expected_response, actual_response);
}


TEST_F(ReadDataByIdentifierTest, ValidRequestForBatteryData) {
    ASSERT_NE(battery, nullptr);
    canid_t can_id = 0x1011;
    battery->ecu_data[0x01A0] = {0x01, 0x02, 0x03, 0x04};
    /* ReadDataByIdentifier request for data identifier 0x5678 */
    std::vector<uint8_t> request = {0x22, 0x01, 0x01, 0xA0};

    std::vector<uint8_t> expected_response = {0x01, 0x02, 0x03, 0x04};
    std::vector<uint8_t> actual_response = rdbi->readDataByIdentifier(can_id, request, false);

    EXPECT_EQ(expected_response, actual_response);
}

TEST_F(ReadDataByIdentifierTest, InvalidRequestLength) {
    canid_t can_id = 0x1000;
    /* Invalid request (length < 4) */
    std::vector<uint8_t> request = {0x22, 0x01};  

    std::vector<uint8_t> expected_response = {0x03, 0x7F, 0x22, 0x13};
    std::vector<uint8_t> actual_response = rdbi->readDataByIdentifier(can_id, request, false);

    EXPECT_EQ(expected_response, actual_response);
}

TEST_F(ReadDataByIdentifierTest, DataIdentifierNotFound) {
    canid_t can_id = 0x1000;
    /* Data identifier 0x9999 not in MCU data */
    std::vector<uint8_t> request = {0x22, 0x01, 0x99, 0x99};

    std::vector<uint8_t> expected_response = {};
    EXPECT_NO_THROW({
        std::vector<uint8_t> actual_response = rdbi->readDataByIdentifier(can_id, request, false);
        EXPECT_EQ(expected_response, actual_response);
    });
}

TEST_F(ReadDataByIdentifierTest, CheckAPIFrameGeneration) {
    canid_t can_id = 0xFA00;
    std::vector<uint8_t> request = {0x22, 0x01, 0x12, 0x34};

    EXPECT_NO_THROW({
        rdbi->readDataByIdentifier(can_id, request, false);
    });
}

TEST_F(ReadDataByIdentifierTest, CheckNonAPIFrameGeneration) {
    canid_t can_id = 0x1000;
    std::vector<uint8_t> request = {0x22, 0x01, 0x12, 0x34};

    EXPECT_NO_THROW({
        rdbi->readDataByIdentifier(can_id, request, false);
    });
}
TEST_F(ReadDataByIdentifierTest, LongResponseForAPIFrame) {
    canid_t can_id = 0xFA10;
    std::vector<uint8_t> request = {0x22, 0x01, 0x12, 0x34};
    /* 10 bytes of data */
    std::vector<uint8_t> long_data(10, 0xBB);
    MCU::mcu->mcu_data[0x1234] = long_data;

    std::vector<uint8_t> actual_response = rdbi->readDataByIdentifier(can_id, request, true);

    std::vector<uint8_t> expected_response;
    expected_response.insert(expected_response.end(), long_data.begin(), long_data.end());

    EXPECT_EQ(expected_response.size(), actual_response.size());

    for (size_t i = 0; i < expected_response.size(); ++i) {
        EXPECT_EQ(expected_response[i], actual_response[i]) << "Mismatch at index " << i;
    }

    EXPECT_GT(actual_response.size(), 8);
}

TEST_F(ReadDataByIdentifierTest, LongResponseForNonAPIFrame) {
    canid_t can_id = 0x1010;
    std::vector<uint8_t> request = {0x22, 0x01, 0x12, 0x34};
    create_interface = nullptr;
    /* Populate MCU data with a long response */
    std::vector<uint8_t> long_data(10, 0xBB);
    MCU::mcu->mcu_data[0x1234] = long_data;

    std::vector<uint8_t> actual_response = rdbi->readDataByIdentifier(can_id, request, false);

    std::vector<uint8_t> expected_response;
    expected_response.insert(expected_response.end(), long_data.begin(), long_data.end());

    EXPECT_EQ(expected_response.size(), actual_response.size());

    for (size_t i = 0; i < expected_response.size(); ++i) {
        EXPECT_EQ(expected_response[i], actual_response[i]) << "Mismatch at index " << i;
    }

    /* Assert that the size of the actual response is greater than 8 bytes */
    EXPECT_GT(actual_response.size(), 8);
}

int main(int argc, char* argv[])
{
    testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}