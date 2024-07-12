#include <gtest/gtest.h>
#include <gmock/gmock.h>
#include "../include/RequestUpdateStatus.h"
#include "../../../uds/write_data_by_identifier/include/WriteDataByIdentifier.h"
#include "../../../mcu/include/MCUModule.h"

class RequestUpdateStatusTest : public ::testing::Test {
protected:
    int socketid = 2;
    RequestUpdateStatus RUS = RequestUpdateStatus(socketid);
    /* Setup and teardown methods */
    void SetUp() override
    {

    }

    void TearDown() override 
    {
    
    }
};

TEST_F(RequestUpdateStatusTest, StatusValidTest)
{
    uint8_t status = IDLE; /* valid OTA status */
    EXPECT_TRUE(RUS.isValidStatus(status));

    status = WAIT_DOWNLOAD_COMPLETED; /* valid OTA intermediary status */
    EXPECT_TRUE(RUS.isValidStatus(status));
}

TEST_F(RequestUpdateStatusTest, StatusInvalidTest)
{
    uint8_t status = 0x05; /* invalid OTA status, IDLE can't have intermediary states */
    EXPECT_FALSE(RUS.isValidStatus(status));

    status = 0xA0; /* invalit OTA status, last state is 0x70 */
    EXPECT_FALSE(RUS.isValidStatus(status));
}

TEST_F(RequestUpdateStatusTest, WrongSenderReceiverCheckTest)
{
    int request_id = 0x00001011;
    std::vector<uint8_t> request = {PCI_L, REQUEST_UPDATE_STATUS_SID};
    std::vector<uint8_t> response = RUS.requestUpdateStatus(request_id, request);

    std::vector<uint8_t> expected_response = {PCI_L, REQUEST_UPDATE_STATUS_SID_SUCCESS, IDLE};

    EXPECT_EQ(response.size(), expected_response.size());
    for(uint8_t index; index <= expected_response.size() - 1; index++)
    {
        EXPECT_EQ(response[index], expected_response[index]);
    }

}

TEST_F(RequestUpdateStatusTest, PositiveResponseTest)
{
    int request_id = 0x0000fa10;
    std::vector<uint8_t> request = {PCI_L, REQUEST_UPDATE_STATUS_SID};
    std::vector<uint8_t> response = RUS.requestUpdateStatus(request_id, request);

    std::vector<uint8_t> expected_response = {PCI_L, REQUEST_UPDATE_STATUS_SID_SUCCESS, IDLE};

    EXPECT_EQ(response.size(), expected_response.size());
    for(uint8_t index; index <= expected_response.size() - 1; index++)
    {
        EXPECT_EQ(response[index], expected_response[index]);
    }
}

TEST_F(RequestUpdateStatusTest, NegativeResponseInvalidStatusTest)
{
    int request_id = 0x0000fa10;

    WriteDataByIdentifier WDBI(0x1111FA10, {PCI_L, WRITE_DATA_BY_IDENTIFIER_SID, OTA_UPDATE_STATUS_DID_MSB, OTA_UPDATE_STATUS_DID_LSB, 0xFF}, MCULogger, 0);

    std::vector<uint8_t> request = {PCI_L, REQUEST_UPDATE_STATUS_SID};
    std::vector<uint8_t> response = RUS.requestUpdateStatus(request_id, request);

    std::vector<uint8_t> expected_response = {PCI_L, NEGATIVE_RESPONSE, REQUEST_UPDATE_STATUS_SID, REQUEST_OUT_OF_RANGE};
    
    EXPECT_EQ(response.size(), expected_response.size());
    for(uint8_t index; index <= expected_response.size() - 1; index++)
    {
        EXPECT_EQ(response[index], expected_response[index]);
    }
}