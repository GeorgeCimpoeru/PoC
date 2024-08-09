#include <iostream>
#include <vector>
#include <cstdint>
#include <string>
#include <gtest/gtest.h>
#include <linux/can.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <net/if.h>
#include <gtest/gtest.h>
#include "../../../utils/include/CreateInterface.h"
#include "../../../utils/include/GenerateFrames.h"
#include "../include/RequestDownload.h"
#include "../../../mcu/include/MCUModule.h"

int socket_;
int socket2_;
const int id = 0xfa10;

class CaptureFrame
{
    public:
        struct can_frame frame;
        void capture()
        {
            int nbytes = read(socket_, &frame, sizeof(struct can_frame));            
            if(nbytes < 0)
            {
                return;
            }
        }
};

struct can_frame createFrame(std::vector<uint8_t> test_data)
{
    struct can_frame result_frame;
    result_frame.can_id = id;
    int i=0;
    for (auto d : test_data)
        result_frame.data[i++] = d;
    result_frame.can_dlc = test_data.size();
    return result_frame;
}

int createSocket()
{
    /* Create socket */
    std::string name_interface = "vcan0";
    struct sockaddr_can addr;
    struct ifreq ifr;
    int s;

    s = socket(PF_CAN, SOCK_RAW, CAN_RAW);
    if( s<0)
    {
        std::cout<<"Error trying to create the socket\n";
        return 1;
    }
    /* Giving name and index to the interface created */
    strcpy(ifr.ifr_name, name_interface.c_str() );
    ioctl(s, SIOCGIFINDEX, &ifr);
    /* Set addr structure with info. of the CAN interface */
    addr.can_family = AF_CAN;
    addr.can_ifindex = ifr.ifr_ifindex;
    /* Bind the socket to the CAN interface */
    int b = bind( s, (struct sockaddr*)&addr, sizeof(addr));
    if( b < 0 )
    {
        std::cout<<"Error binding\n";
        return 1;
    }
    int flags = fcntl(s, F_GETFL, 0);
    if (flags == -1) {
        return 1;
    }
    /* Set the O_NONBLOCK flag to make the socket non-blocking */
    flags |= O_NONBLOCK;
    if (fcntl(s, F_SETFL, flags) == -1) {
        return -1;
    }
    return s;
}

class RequestDownloadTest : public ::testing::Test
{
protected:
    Logger logger;    
    GenerateFrames* generate;    
    RequestDownloadService request_download;
    CreateInterface* create_interface;

    RequestDownloadTest()                
           :request_download(socket_, logger)
    {
    }

    /* Setup and teardown methods */
    void SetUp() override
    {
        generate = new GenerateFrames(socket_, logger);
    }

    void TearDown() override
    {
        delete generate;
    }
};

TEST_F(RequestDownloadTest, CompressionType)
{
    int id = 0x11fa10;
    std::vector<uint8_t> stored_data = { 0x07, 0x34, 0x00, 0x12, 0x12, 0x88 };
    /* redirect stdout to capture the output */
    testing::internal::CaptureStdout();
    request_download.requestDownloadRequest(id, stored_data);
    std::string output = testing::internal::GetCapturedStdout();
    /* check if the correct message is printed */
    std::string message = "No compression method used";
    EXPECT_NE(output.find(message), std::string::npos);
}

TEST_F(RequestDownloadTest, EncryptionType)
{
    int id = 0x11fa10;
    std::vector<uint8_t> stored_data = { 0x07, 0x34, 0x00, 0x12, 0x12, 0x88 };
    /* redirect stdout to capture the output */
    testing::internal::CaptureStdout();
    request_download.requestDownloadRequest(id, stored_data);
    std::string output = testing::internal::GetCapturedStdout();
    /* check if the correct message is printed */
    std::string message = "No encryption method used";
    EXPECT_NE(output.find(message), std::string::npos);
}

TEST_F(RequestDownloadTest, ValidMemoryRange)
{
    int id = 0x11fa10;
    std::vector<uint8_t> stored_data = { 0x07, 0x34, 0x00, 0x11, 0x12, 0x12, 0x88 };
    /* redirect stdout to capture the output */
    testing::internal::CaptureStdout();
    request_download.requestDownloadRequest(id, stored_data);
    std::string output = testing::internal::GetCapturedStdout();
    /* check if the correct message is printed */
    std::string message = "Validated Memory Address: 0x12";
    std::string message1 = "Validated Memory Size: 0x12";
    EXPECT_NE(output.find(message), std::string::npos);
    EXPECT_NE(output.find(message1), std::string::npos);
}

TEST_F(RequestDownloadTest, InvalidMemoryAddress)
{
    int id = 0x11fa10;
    std::vector<uint8_t> stored_data = { 0x07, 0x34, 0x00, 0x51, 0x12, 0xFF, 0xFF, 0xFF, 0xFF, 0x12, 0x88 };
    /* redirect stdout to capture the output */
    testing::internal::CaptureStdout();
    request_download.requestDownloadRequest(id, stored_data);
    std::string output = testing::internal::GetCapturedStdout();
    /* check if the correct message is printed */
    std::string message = "Error: Invalid memory address:";
    std::string message1 = "Error: Invalid memory range";
    EXPECT_NE(output.find(message), std::string::npos);
    EXPECT_NE(output.find(message1), std::string::npos);
}

TEST_F(RequestDownloadTest, InvalidMemorySize)
{
    int id = 0x11fa10;
    std::vector<uint8_t> stored_data = { 0x07, 0x34, 0x00, 0x15, 0x12, 0XFF, 0XFF, 0XFF, 0XFF, 0xFF, 0x88 };
    /* redirect stdout to capture the output */
    testing::internal::CaptureStdout();
    request_download.requestDownloadRequest(id, stored_data);
    std::string output = testing::internal::GetCapturedStdout();
    /* check if the correct message is printed */
    std::string message = "Error: Invalid memory size:";
    std::string message1 = "Error: Invalid memory range";
    EXPECT_NE(output.find(message), std::string::npos);
    EXPECT_NE(output.find(message1), std::string::npos);
}

TEST_F(RequestDownloadTest, InvalidDownloadType)
{
    int id = 0x11fa10;
    std::vector<uint8_t> stored_data = { 0x07, 0x34, 0x00, 0x11, 0x12, 0xFF, 0x12, 0x12, 0x88 };
    /* redirect stdout to capture the output */
    testing::internal::CaptureStdout();
    request_download.requestDownloadRequest(id, stored_data);
    std::string output = testing::internal::GetCapturedStdout();
    /* check if the correct message is printed */
    std::string message = "Error: Invalid download type: ";
    EXPECT_NE(output.find(message), std::string::npos);
}

TEST_F(RequestDownloadTest, InvalidPayload)
{
    int id = 0x11fa10;
    std::vector<uint8_t> stored_data = { 0x07, 0x34, 0x00 };
    /* redirect stdout to capture the output */
    testing::internal::CaptureStdout();
    request_download.requestDownloadRequest(id, stored_data);
    std::string output = testing::internal::GetCapturedStdout();
    /* check if the correct message is printed */
    std::string message = "Payload does not contain enough data for memory address and size";
    EXPECT_NE(output.find(message), std::string::npos);
}

TEST_F(RequestDownloadTest, RequestDownloadResp)
{
    int id = 0x11fa10;
    std::vector<uint8_t> stored_data = { 0x07, 0x34, 0x00, 0x11, 0x12, 0x12, 0x88 };
    /* redirect stdout to capture the output */
    testing::internal::CaptureStdout();
    request_download.requestDownloadRequest(id, stored_data);
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
    int id = 0x11fa11;
    std::vector<uint8_t> stored_data = { 0x07, 0x34, 0x00, 0x11, 0x12, 0x12, 0x88 };
    /* redirect stdout to capture the output */
    testing::internal::CaptureStderr();
    request_download.requestDownloadRequest(id, stored_data);
    std::string output = testing::internal::GetCapturedStderr();
    /* check if the correct message is printed */
    std::string message = "Create interface is nullptr";
    EXPECT_NE(output.find(message), std::string::npos);
}

TEST_F(RequestDownloadTest, DonwloadFromDrivePart3If)
{           
    int id = 0x11fa10;
    std::vector<uint8_t> stored_data = { 0x5, 0x34, 0x00, 0x11, 0x12, 0x12, 0x89 };
    /* redirect stdout to capture the output */
    testing::internal::CaptureStdout();
    request_download.requestDownloadRequest(id, stored_data);
    std::string output = testing::internal::GetCapturedStdout();
    /* check if the correct message is printed */
    std::string message = "Map memory in MCU and transfer data";
    EXPECT_NE(output.find(message), std::string::npos);
}

TEST_F(RequestDownloadTest, DonwloadFromDrivePart3else)
{
    int id = 0x11fa11;
    std::vector<uint8_t> stored_data = { 0x5, 0x34, 0x00, 0x11, 0x12, 0x12, 0x89 };
    /* redirect stdout to capture the output */
    testing::internal::CaptureStdout();
    request_download.requestDownloadRequest(id, stored_data);
    std::string output = testing::internal::GetCapturedStdout();
    /* check if the correct message is printed */
    std::string message = "Map memory in ECU and transfer data";
    EXPECT_NE(output.find(message), std::string::npos);
}

int main(int argc, char **argv)
{  
    socket_ = createSocket();
    socket2_ = createSocket();  
testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}



