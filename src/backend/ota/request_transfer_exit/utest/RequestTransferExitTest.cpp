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
#include "../include/RequestTransferExit.h"
#include "../../../utils/include/Logger.h"
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

/* Class fixture for general tests */
class RequestTransferExitTest : public ::testing::Test
{
protected:
    Logger logger;
    CaptureFrame* captured_frame;
    RequestTransferExit* transfer_exit;
    RequestTransferExitTest()
    {
    transfer_exit = new RequestTransferExit(socket_, logger);
    captured_frame = new CaptureFrame();
    }

    /* Setup and teardown methods */
    void SetUp() override
    {
        /* Populate some test data for MCU */
        MCU::mcu->default_DID_MCU[0x01E0] = {0x31, 0x02, 0x03, 0x04};      
    }

    void TearDown() override 
    {
        delete transfer_exit;
        delete captured_frame;
    }
};

TEST_F(RequestTransferExitTest, TransferSuccesNoCallbackSet)
{
    /* simulate a successfull transfer data */
    bool transfer_success =  true;
    bool result = transfer_exit->requestTransferExit(0x10, transfer_success);
    /* should stop as no callback is set */
    EXPECT_FALSE(result);
}

TEST_F(RequestTransferExitTest, TransferFailNoCallbackSet)
{
    /* simulate a failed transfer data*/
    bool transfer_success =  false;
    bool result = transfer_exit->requestTransferExit(0x10, transfer_success);
    /* should stop as no callback is set */
    EXPECT_FALSE(result);
}

TEST_F(RequestTransferExitTest, TransferSuccesCallbackSet)
{
    /* simulate a successfull transfer data*/
    bool transfer_success =  true;
    /** Set a lambda function as the callback for the RequestTransferExit class that takes 
    *   one parameter of type boolean named and the body of lambda simply returns the success parameter
    */
    transfer_exit->setTransferCompleteCallBack([](bool success) { return success; });

    bool result = transfer_exit->requestTransferExit(0x10, transfer_success);

    EXPECT_TRUE(result);
}

TEST_F(RequestTransferExitTest, TransferFailCallbackSet)
{
   /* simulate a failed transfer data*/
    bool transfer_success =  false;
    transfer_exit->setTransferCompleteCallBack([](bool success) { return success; });

    bool result = transfer_exit->requestTransferExit(0x10, transfer_success);

    EXPECT_FALSE(result); 
}

TEST_F(RequestTransferExitTest, CallbackNullptr)
{
    /* simulate a successfull transfer data*/
    bool transfer_success =  true;
    /* Explicitly set the callback nullptr */
    transfer_exit->setTransferCompleteCallBack(nullptr);

    bool result = transfer_exit->requestTransferExit(0x10, transfer_success);
    /* should stop the transfer as callback is nullptr */
    EXPECT_FALSE(result); 
}

/* Test for Incorrect Message Length */
TEST_F(RequestTransferExitTest, IncorrectMessageLengthTest) {

    std::vector<uint8_t> invalid_frame_data = {0x04, 0x37};
    std::vector<uint8_t> expected_frame_data = {0x03, 0x7F, 0x37, 0x13};

    transfer_exit->requestTRansferExitRequest(0xFA10, invalid_frame_data);
    captured_frame->capture();
    for (int i = 0; i < captured_frame->frame.can_dlc; ++i) {
        EXPECT_EQ(expected_frame_data[i], captured_frame->frame.data[i]);
    }
}

/* Test for Positive Response */
TEST_F(RequestTransferExitTest, PositiveResponse) 
{    
    std::vector<uint8_t> request = {0x03, 0x37, 0x01};
    std::vector<uint8_t> expected_response = {0x02, 0x77, 0x01};

    transfer_exit->requestTRansferExitRequest(0xFA10, request);
    captured_frame->capture();
    for (int i = 0; i < captured_frame->frame.can_dlc; ++i) {
        EXPECT_EQ(expected_response[i], captured_frame->frame.data[i]);
    }
}

/* Test for Request Sequence Error */
TEST_F(RequestTransferExitTest, RequestSequenceError) 
{       
    MCU::mcu->default_DID_MCU[0x01E0] = {0x01, 0x02, 0x03, 0x04};
    std::vector<uint8_t> request = {0x03, 0x37, 0x01};
    std::vector<uint8_t> expected_response = {0x02, 0x7F, 0x37, 0x24};

    transfer_exit->requestTRansferExitRequest(0xFA10, request);
    captured_frame->capture();
    for (int i = 0; i < captured_frame->frame.can_dlc; ++i) {
        EXPECT_EQ(expected_response[i], captured_frame->frame.data[i]);
    }
}

/* Test for OTA_UPDATE_STATUS_DID */
TEST_F(RequestTransferExitTest, OTAUpStDIDNotFound) 
{       
    MCU::mcu->default_DID_MCU.clear();
    std::vector<uint8_t> request = {0x03, 0x37, 0x01};
    
    /* redirect stdout to capture the output */
    testing::internal::CaptureStdout();
    transfer_exit->requestTRansferExitRequest(0xFA10, request);
    std::string output = testing::internal::GetCapturedStdout();
    /* check if the correct message is printed */
    std::string message = "OTA_UPDATE_STATUS_DID 01E0 not found ";
    EXPECT_NE(output.find(message), std::string::npos);    
}

int main(int argc, char **argv)
{  
    socket_ = createSocket();
    socket2_ = createSocket();  
    testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
