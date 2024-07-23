#include <gtest/gtest.h>
#include "../../../utils/include/CreateInterface.h"
#include "../../../utils/include/GenerateFrames.h"
#include "../include/RequestDownload.h"
#include "../../../mcu/include/MCUModule.h"
#include <fcntl.h>
#include <sys/ioctl.h>
#include <net/if.h>

class RequestDownloadTest : public ::testing::Test
{
protected:
    Logger logger;    
    GenerateFrames* generate;
    DiagnosticSessionControl diagnostic_session;
    ReadDataByIdentifier software_version;
    SecurityAccess logged_in;
    RequestDownloadService request_download;
    CreateInterface* create_interface;

    RequestDownloadTest()
        : diagnostic_session(logger, 8),
          software_version(8, logger),
          logged_in(8, logger),
          request_download(8, logger)
    {
    }

    /* Setup and teardown methods */
    void SetUp() override
    {
        generate = new GenerateFrames(8, logger);
    }

    void TearDown() override
    {
        delete generate;
    }
};

TEST_F(RequestDownloadTest, CompressionType)
{
    int id = 0x111110;
    std::vector<uint8_t> stored_data = { 0x07, 0x34, 0x00, 0x12, 0x12, 0x88 };
    /* redirect stdout to capture the output */
    testing::internal::CaptureStdout();
    request_download.requestDownloadRequest(id, stored_data, logger, diagnostic_session, software_version, logged_in);
    std::string output = testing::internal::GetCapturedStdout();
    /* check if the correct message is printed */
    std::string message = "No compression method used";
    EXPECT_NE(output.find(message), std::string::npos);
}

TEST_F(RequestDownloadTest, EncryptionType)
{
    int id = 0x111110;
    std::vector<uint8_t> stored_data = { 0x07, 0x34, 0x00, 0x12, 0x12, 0x88 };
    /* redirect stdout to capture the output */
    testing::internal::CaptureStdout();
    request_download.requestDownloadRequest(id, stored_data, logger, diagnostic_session, software_version, logged_in);
    std::string output = testing::internal::GetCapturedStdout();
    /* check if the correct message is printed */
    std::string message = "No encryption method used";
    EXPECT_NE(output.find(message), std::string::npos);
}

TEST_F(RequestDownloadTest, ValidMemoryRange)
{
    int id = 0x101110;  
    std::vector<uint8_t> stored_data = { 0x07, 0x34, 0x00, 0x11, 0x12, 0x12, 0x88 };
    /* redirect stdout to capture the output */
    testing::internal::CaptureStdout();
    request_download.requestDownloadRequest(id, stored_data, logger, diagnostic_session, software_version, logged_in);
    std::string output = testing::internal::GetCapturedStdout();
    /* check if the correct message is printed */   
    std::string message = "Validated Memory Address: 0x12";
    std::string message1 = "Validated Memory Size: 0x12";
    EXPECT_NE(output.find(message), std::string::npos);
    EXPECT_NE(output.find(message1), std::string::npos);
}

TEST_F(RequestDownloadTest, InvalidMemoryAddress)
{
    int id = 0x101110;
    std::vector<uint8_t> stored_data = { 0x07, 0x34, 0x00, 0x51, 0x12, 0xFF, 0xFF, 0xFF, 0xFF, 0x12, 0x88 };
    /* redirect stdout to capture the output */
    testing::internal::CaptureStdout();
    request_download.requestDownloadRequest(id, stored_data, logger, diagnostic_session, software_version, logged_in);
    std::string output = testing::internal::GetCapturedStdout();    
    /* check if the correct message is printed */  
    std::string message = "Error: Invalid memory address:";
    std::string message1 = "Error: Invalid memory range";
    EXPECT_NE(output.find(message), std::string::npos);
    EXPECT_NE(output.find(message1), std::string::npos);
}

TEST_F(RequestDownloadTest, InvalidMemorySize)
{
    int id = 0x101110;
    std::vector<uint8_t> stored_data = { 0x07, 0x34, 0x00, 0x15, 0x12, 0XFF, 0XFF, 0XFF, 0XFF, 0xFF, 0x88 };
    /* redirect stdout to capture the output */
    testing::internal::CaptureStdout();
    request_download.requestDownloadRequest(id, stored_data, logger, diagnostic_session, software_version, logged_in);
    std::string output = testing::internal::GetCapturedStdout();    
    /* check if the correct message is printed */  
    std::string message = "Error: Invalid memory size:";    
    std::string message1 = "Error: Invalid memory range";    
    EXPECT_NE(output.find(message), std::string::npos);
    EXPECT_NE(output.find(message1), std::string::npos);
}

TEST_F(RequestDownloadTest, RequestDownloadResp)
{
    int id = 0x101110; 
    std::vector<uint8_t> stored_data = { 0x07, 0x34, 0x00, 0x11, 0x12, 0x12, 0x88 };
    int max_number_block = 0;
    /* redirect stdout to capture the output */
    testing::internal::CaptureStdout();
    request_download.requestDownloadRequest(id, stored_data, logger, diagnostic_session, software_version, logged_in);
    std::string output = testing::internal::GetCapturedStdout();
    /* check if the correct message is printed */   
    std::string message = "frame id dest: 0x10";
    std::string message1 = "log in service";
    std::string message2 = "frame id";
    std::string message3 = "max no block";
    EXPECT_NE(output.find(message), std::string::npos);
    EXPECT_NE(output.find(message1), std::string::npos);
    EXPECT_NE(output.find(message2), std::string::npos);
    EXPECT_NE(output.find(message3), std::string::npos);
}

TEST_F(RequestDownloadTest, RequestDownloadRespElse)
{
    int id = 0x111111; 
    std::vector<uint8_t> stored_data = { 0x07, 0x34, 0x00, 0x11, 0x12, 0x12, 0x88 };
    int max_number_block = 0;
    /* redirect stdout to capture the output */
    testing::internal::CaptureStderr();
    request_download.requestDownloadRequest(id, stored_data, logger, diagnostic_session, software_version, logged_in);
    std::string output = testing::internal::GetCapturedStderr();
    /* check if the correct message is printed */   
    std::string message = "Create interface is nullptr";
    EXPECT_NE(output.find(message), std::string::npos);
}   

TEST_F(RequestDownloadTest, DonwloadFromDrivePart3If)
{           
    int id = 0x101110;
    std::vector<uint8_t> stored_data = { 0x5, 0x34, 0x00, 0x11, 0x12, 0x12, 0x89 };
    /* redirect stdout to capture the output */
    testing::internal::CaptureStdout();
    request_download.requestDownloadRequest(id, stored_data, logger, diagnostic_session, software_version, logged_in);
    std::string output = testing::internal::GetCapturedStdout();
    /* check if the correct message is printed */
    std::string message = "Map memory in MCU and transfer data";
    EXPECT_NE(output.find(message), std::string::npos);
}

TEST_F(RequestDownloadTest, DonwloadFromDrivePart3else)
{
    int id = 0x111110;
    std::vector<uint8_t> stored_data = { 0x5, 0x34, 0x00, 0x11, 0x12, 0x12, 0x89 };
    /* redirect stdout to capture the output */
    testing::internal::CaptureStdout();
    request_download.requestDownloadRequest(id, stored_data, logger, diagnostic_session, software_version, logged_in);
    std::string output = testing::internal::GetCapturedStdout();
    /* check if the correct message is printed */
    std::string message = "Map memory in ECU and transfer data";
    EXPECT_NE(output.find(message), std::string::npos);
}

int main(int argc, char **argv)
{    
testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}



