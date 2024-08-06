#include <gtest/gtest.h>
#include <gmock/gmock.h>
#include "../include/RequestUpdateStatus.h"
#include "../../../uds/write_data_by_identifier/include/WriteDataByIdentifier.h"
#include "../../../mcu/include/MCUModule.h"

class RequestUpdateStatusTest : public ::testing::Test {
protected:
    int socket_id = 2;
    Logger logger;
    RequestUpdateStatus RUS = RequestUpdateStatus(socket_id);
};

/**
 * @brief Test for valid ota states (belongs to the valid ota states enumeration).
 * 
 */
TEST_F(RequestUpdateStatusTest, StatusValidTest)
{
    uint8_t status = IDLE; /* valid OTA status */
    EXPECT_TRUE(RUS.isValidStatus(status));

    status = WAIT_DOWNLOAD_COMPLETED; /* valid OTA intermediary status */
    EXPECT_TRUE(RUS.isValidStatus(status));
}

/**
 * @brief Test for OTA invalid states.
 * 
 */
TEST_F(RequestUpdateStatusTest, StatusInvalidTest)
{
    uint8_t status = 0x05; /* invalid OTA status, IDLE can't have intermediary states */
    EXPECT_FALSE(RUS.isValidStatus(status));

    status = 0xA0; /* invalit OTA status, last state is 0x70 */
    EXPECT_FALSE(RUS.isValidStatus(status));
}

/**
 * @brief Test if a wrong request id is redirected to the right one (caller API, receiver MCU)
 * 
 */
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

/**
 * @brief Test if the initial OTA status is IDLE
 * 
 */
TEST_F(RequestUpdateStatusTest, InitialOtaStatusTest)
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

/**
 * @brief Test if status value is correctly read after a write data by identifier
 * 
 */
TEST_F(RequestUpdateStatusTest, AfterStatusUpdateTest)
{
    int request_id = 0x0000fa10;
    uint8_t new_status = WAIT;

    WriteDataByIdentifier WDBI(logger, socket_id);
    WDBI.WriteDataByIdentifierService(request_id, {PCI_L, WRITE_DATA_BY_IDENTIFIER_SID, OTA_UPDATE_STATUS_DID_MSB, OTA_UPDATE_STATUS_DID_LSB, new_status});

    std::vector<uint8_t> request = {PCI_L, REQUEST_UPDATE_STATUS_SID};
    std::vector<uint8_t> response = RUS.requestUpdateStatus(request_id, request);

    std::vector<uint8_t> expected_response = {PCI_L, REQUEST_UPDATE_STATUS_SID_SUCCESS, new_status};
    
    EXPECT_EQ(response.size(), expected_response.size());
    for(uint8_t index; index <= expected_response.size() - 1; index++)
    {
        EXPECT_EQ(response[index], expected_response[index]);
    }
}

/**
 * @brief Test if a negative response is sent in case of an invalid status.
 * 
 */
TEST_F(RequestUpdateStatusTest, NegativeResponseInvalidStatusTest)
{
    int request_id = 0x0000fa10;
    uint8_t invalid_status = 0xFF;

    WriteDataByIdentifier WDBI(logger, socket_id);
    WDBI.WriteDataByIdentifierService(request_id, {PCI_L, WRITE_DATA_BY_IDENTIFIER_SID, OTA_UPDATE_STATUS_DID_MSB, OTA_UPDATE_STATUS_DID_LSB, invalid_status});

    std::vector<uint8_t> request = {PCI_L, REQUEST_UPDATE_STATUS_SID};
    std::vector<uint8_t> response = RUS.requestUpdateStatus(request_id, request);

    std::vector<uint8_t> expected_response = {PCI_L, NEGATIVE_RESPONSE, REQUEST_UPDATE_STATUS_SID, REQUEST_OUT_OF_RANGE};
    
    EXPECT_EQ(response.size(), expected_response.size());
    for(uint8_t index; index <= expected_response.size() - 1; index++)
    {
        EXPECT_EQ(response[index], expected_response[index]);
    }
}