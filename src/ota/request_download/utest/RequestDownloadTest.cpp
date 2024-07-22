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
    //int socket;
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
        //socket = 8;
        generate = new GenerateFrames(8, logger);      
    }

    void TearDown() override 
    {
        delete generate;
    }
};

TEST_F(RequestDownloadTest, CompressionType)
{      
    //ASSERT_NE(generate, nullptr);
    //generate->requestDownload(0x111110,0x0,0x12,0x12,0x88);
    //int id = 0x10FA10;
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
    //ASSERT_NE(generate, nullptr);
    //generate->requestDownload(0x111110,0x0,0x12,0x12,0x88);
    //int id = 0x10FA10;
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
    //ASSERT_NE(generate, nullptr);
    int id = 0x101110;  
    std::vector<uint8_t> stored_data = { 0x07, 0x34, 0x00, 0x12, 0x12, 0x88 };
    /* redirect stdout to capture the output */
    testing::internal::CaptureStdout();
    //generate->requestDownload(0x111110,0x0,0x12,0x12,0x88);
    request_download.requestDownloadRequest(id, stored_data, logger, diagnostic_session, software_version, logged_in);
    std::string output = testing::internal::GetCapturedStdout();
    // int id = 0x101110; 
    // std::vector<uint8_t> stored_data = { 0x07, 0x34, 0x00, 0x01, 0x12, 0x12, 0x88 };
    
            
    /* redirect stdout to capture the output */
    // testing::internal::CaptureStdout();
    // request_download.requestDownloadRequest(id, stored_data, logger, diagnostic_session, software_version, logged_in);
    // std::string output = testing::internal::GetCapturedStdout();
    /* check if the correct message is printed */   
    std::string message = "Validated Memory Address: 0x12";
    std::string message1 = "Validated Memory Size: 0x8800";
    EXPECT_NE(output.find(message), std::string::npos);
    EXPECT_NE(output.find(message1), std::string::npos);
}

TEST_F(RequestDownloadTest, InvalidMemorySize)
{
    //ASSERT_NE(generate, nullptr);
    int id = 0x101110;
    //std::vector<uint8_t> stored_data = { 0x07, 0x34, 0x00, 0x11, 0xFF, 0x00, 0x88 };
    std::vector<uint8_t> stored_data = { 0x07, 0x34, 0x00, 0x21, 0x12, 0x34, 0x00, 0x88 };
    /* redirect stdout to capture the output */
    testing::internal::CaptureStderr();
    //generate->requestDownload(0x111110,0x0,0x12,0x12,0x88);
    request_download.requestDownloadRequest(id, stored_data, logger, diagnostic_session, software_version, logged_in);
    std::string output = testing::internal::GetCapturedStderr();    
    /* check if the correct message is printed */  
    std::string message = "Error: Invalid memory size: 0";
    std::string output1 = testing::internal::GetCapturedStderr(); 
    std::string message1 = "Error: Invalid memory range";    
    EXPECT_NE(output.find(message), std::string::npos);
    EXPECT_NE(output1.find(message1), std::string::npos);
}

// TEST_F(RequestDownloadTest, InvalidMemoryAddressRange)
// {
//     //ASSERT_NE(generate, nullptr);
//     int id = 0x101110;  
//     std::vector<uint8_t> stored_data = { 0x07, 0x34, 0x00, 0x21, 0x01, 0x00, 0x01, 0x88 };
//     /* redirect stdout to capture the output */
//     testing::internal::CaptureStderr();
//     //generate->requestDownload(0x111110,0x0,0x12,0x12,0x88);
//     request_download.requestDownloadRequest(id, stored_data, logger, diagnostic_session, software_version, logged_in);
//     std::string output = testing::internal::GetCapturedStderr();    
//     /* check if the correct message is printed */  
//     std::string message = "Error: Invalid memory address: 0"; 
//     std::string output1 = testing::internal::GetCapturedStderr();
//     std::string message1 = "Error: Invalid memory range";    
//     EXPECT_NE(output.find(message), std::string::npos);
//     EXPECT_NE(output.find(message1), std::string::npos);
// }

// TEST_F(RequestDownloadTest, RequestDownloadResp)
// {
//     int id = 0x101110; 
//     std::vector<uint8_t> stored_data = { 0x07, 0x34, 0x00, 0x12, 0x12, 0x88 };
//     int max_number_block = 0;
//     /* redirect stdout to capture the output */
//     testing::internal::CaptureStdout();
//     request_download.requestDownloadRequest(id, stored_data, logger, diagnostic_session, software_version, logged_in);
//     std::string output = testing::internal::GetCapturedStdout();
//     /* check if the correct message is printed */   
//     std::string message = " frame id dest: 0x}";
//     EXPECT_EQ(output, message );
// }

TEST_F(RequestDownloadTest, DonwloadFromDrivePart3If)
{       
    //ASSERT_NE(generate, nullptr);
    //generate->requestDownload(0x101110,0x0,0x12,0x12,0x89);
    //int id = 0x10FA10;
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
    //ASSERT_NE(generate, nullptr);
    //generate->requestDownload(0x111110,0x0,0x12,0x12,0x89);
    //int id = 0x10FA10;
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



