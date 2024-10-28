#include "../include/GenerateFrames.h"

#include <gtest/gtest.h>
#include <net/if.h>
#include <cstring>
#include <string>
#include <sys/ioctl.h>
#include <thread>
#include <fcntl.h>

/* Global variables */
/* Sockets for Recive/Send */
int s1;
int s2;
/* Const */
const int id = 0xFA10;

/* Class to capture the frame sin the can-bus */
class CaptureFrame
{
    public:
        struct can_frame frame;
        void capture()
        {
            int nbytes = read(s2, &frame, sizeof(frame));
            if(nbytes < 0)
            {
                std::cerr << "Error reading frame\n";
            }
        }
};
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
    return s;
}
struct can_frame createFrame(std::vector<uint8_t> test_data)
{
    struct can_frame result_frame;
    result_frame.can_id = (id & CAN_EFF_MASK) | CAN_EFF_FLAG;
    int i=0;
    for (auto d : test_data)
        result_frame.data[i++] = d;
    result_frame.can_dlc = test_data.size();
    return result_frame;
}

struct can_frame createFrame(uint16_t can_id ,std::vector<uint8_t> test_data)
{
    struct can_frame result_frame;
    result_frame.can_id = (can_id & CAN_EFF_MASK) | CAN_EFF_FLAG;
    int i=0;
    for (auto d : test_data)
        result_frame.data[i++] = d;
    result_frame.can_dlc = test_data.size();
    return result_frame;
}
void testFrames(struct can_frame expected_frame, CaptureFrame &c1 )
{
    EXPECT_EQ(expected_frame.can_id, c1.frame.can_id);
    EXPECT_EQ(expected_frame.can_dlc, c1.frame.can_dlc);
    for (int i = 0; i < expected_frame.can_dlc; ++i) {
        EXPECT_EQ(expected_frame.data[i], c1.frame.data[i]);
    }
}
Logger logger;
/* Create object for all tests */
struct GenerateFramesTest : testing::Test
{
    GenerateFrames* g1;
    GenerateFrames* g2;
    CaptureFrame* c1;
    GenerateFramesTest()
    {
        g1 = new GenerateFrames(s1, logger);
        g2 = new GenerateFrames(logger);
        c1 = new CaptureFrame();
    }
    ~GenerateFramesTest()
    {
        delete g1;
        delete g2;
        delete c1;
    }
};
/* Test for AddSocket */
TEST_F(GenerateFramesTest, AddSocket)
{ 
    EXPECT_EQ(s1, g1->getSocket());
}

/* Test for Send ERROR_FRAME */
TEST_F(GenerateFramesTest, ERROR_FRAME) 
{
    /* Send frame */
    g1->sendFrame(0x101,{0x12}, ERROR_FRAME);
}

/* Test for Send OVERLOAD_FRAME */
TEST_F(GenerateFramesTest, OVERLOAD_FRAME) 
{
    /* Send frame */
    g1->sendFrame(0x101,{0x12}, OVERLOAD_FRAME);
}

/* Test for Send DATA_FRAME */
TEST_F(GenerateFramesTest, DATA_FRAME) 
{
    /* Start listening for frame in the CAN-BUS */
    std::thread receive_thread([this]() {
        c1->capture();
    });
    /* Send frame */
    g1->sendFrame(0x101,{0x12}, DATA_FRAME);
    receive_thread.join();
    /* TEST */
    EXPECT_EQ(0x80000101, c1->frame.can_id);
    EXPECT_EQ(1, c1->frame.can_dlc);
    EXPECT_EQ(0x12, c1->frame.data[0]);
}

/* Test for Send REMOTE_FRAME */
TEST_F(GenerateFramesTest, REMOTE_FRAME) 
{
    /* Start listening for frame in the CAN-BUS */
    std::thread receive_thread([this]() {
        c1->capture();
    });
    /* Send frame */
    g1->sendFrame(0x101,{0x12}, REMOTE_FRAME);
    receive_thread.join();
    /* TEST */
    EXPECT_EQ(0xc0000101, c1->frame.can_id);
    EXPECT_EQ(1, c1->frame.can_dlc);
    EXPECT_EQ(0x12, c1->frame.data[0]);
}

/* Test for Wrong Socket */
TEST_F(GenerateFramesTest, CorrectSocket) 
{
    /* Start listening for frame in the CAN-BUS */
    std::thread receive_thread([this]() {
        c1->capture();
    });
    /* TEST */
    EXPECT_EQ(g1->sendFrame(0x101,{0x12},s1, DATA_FRAME), 0);
    receive_thread.join();
}

/* Test for Service SessionConroll */
TEST_F(GenerateFramesTest, SessionControlTest) 
{
    /* Create expected frame */
    struct can_frame result_frame = createFrame({0x02,0x10,0x01});
    /* Start listening for frame in the CAN-BUS */
    std::thread receive_thread([this]() {
        c1->capture();
    });
    /* Send frame */
    g1->sessionControl(0xFA10,0x01);
    receive_thread.join();
    /* TEST */
    testFrames(result_frame, *c1);
}
/* Test for Service SessionConroll 2 */
TEST_F(GenerateFramesTest, SessionControlTest2) 
{
    /* Create expected frame */
    struct can_frame result_frame = createFrame({0x02,0x50,0x01});
    /* Start listening for frame in the CAN-BUS */
    std::thread receive_thread([this]() {
        c1->capture();
    });
    /* Send frame */
    g1->sessionControl(0xFA10,0x01,true);
    receive_thread.join();
    /* TEST */
    testFrames(result_frame, *c1);
}
/* Test for Service EcuReset */
TEST_F(GenerateFramesTest, EcuResetTest) 
{
    /* Create expected frame */
    struct can_frame result_frame = createFrame({0x02,0x11,0x01});
    /* Start listening for frame in the CAN-BUS */
    std::thread receive_thread([this]() {
        c1->capture();
    });
    /* Send frame */
    g1->ecuReset(id,0x01);
    receive_thread.join();
    /* TEST */
    testFrames(result_frame, *c1);
}
/* Test for Service EcuReset 2 */
TEST_F(GenerateFramesTest, EcuResetTest2) 
{
    /* Create expected frame */
    struct can_frame result_frame = createFrame({0x02,0x51,0x01});
    /* Start listening for frame in the CAN-BUS */
    std::thread receive_thread([this]() {
        c1->capture();
    });
    /* Send frame */
    g1->ecuReset(id,0x01,true);
    receive_thread.join();
    /* TEST */
    testFrames(result_frame, *c1);
}

/* Test for Service EcuResetSocket */
TEST_F(GenerateFramesTest, EcuResetSocket) 
{
    /* Create expected frame */
    struct can_frame result_frame = createFrame({0x02,0x11,0x01});
    /* Start listening for frame in the CAN-BUS */
    std::thread receive_thread([this]() {
        c1->capture();
    });
    /* Send frame */
    g1->ecuReset(id,0x01, s1);
    receive_thread.join();
    /* TEST */
    testFrames(result_frame, *c1);
}

/* Test for Service EcuResetSocket 2 */
TEST_F(GenerateFramesTest, EcuResetSocket2) 
{
    /* Create expected frame */
    struct can_frame result_frame = createFrame({0x02,0x51,0x01});
    /* Start listening for frame in the CAN-BUS */
    std::thread receive_thread([this]() {
        c1->capture();
    });
    /* Send frame */
    g1->ecuReset(id,0x01, s1,true);
    receive_thread.join();
    /* TEST */
    testFrames(result_frame, *c1);
}
/* Test for method securityAccessRequestSeed */
TEST_F(GenerateFramesTest, SecuritySeedTest) 
{
    /* Create expected frame */
    struct can_frame result_frame = createFrame({0x05,0x67,0x1,0x23,0x34,0x35});
    /* Start listening for frame in the CAN-BUS */
    std::thread receive_thread([this]() {
        c1->capture();
    });
    /* Send frame */
    g1->securityAccessRequestSeed(id,{0x23,0x34,0x35});
    receive_thread.join();
    /* TEST */
    testFrames(result_frame, *c1);
}
/* Test for method securityAccessRequestSeed 2 */
TEST_F(GenerateFramesTest, SecuritySeedTest2) 
{
    /* Create expected frame */
    struct can_frame result_frame = createFrame({0x02,0x27,0x1});
    /* Start listening for frame in the CAN-BUS */
    std::thread receive_thread([this]() {
        c1->capture();
    });
    /* Send frame */
    g1->securityAccessRequestSeed(id);
    receive_thread.join();
    /* TEST */
    testFrames(result_frame, *c1);
}
/* Test for method securityAccessSendKey */
TEST_F(GenerateFramesTest, SecurityKeyTest) 
{
    /* Create expected frame */
    struct can_frame result_frame = createFrame({0x05,0x27,0x2,0x23,0x34,0x35});
    /* Start listening for frame in the CAN-BUS */
    std::thread receive_thread([this]() {
        c1->capture();
    });
    /* Send frame */
    g1->securityAccessSendKey(id,{0x23,0x34,0x35});
    receive_thread.join();
    /* TEST */
    testFrames(result_frame, *c1);
}
/* Test for method securityAccessSendKey as a response */
TEST_F(GenerateFramesTest, SecurityKeyResponseTest) 
{
    /* Create expected frame */
    struct can_frame expected_frame = createFrame({0x2,0x67,0x02});
    /* Start listening for frame in the CAN-BUS */
    std::thread receive_thread([this]() {
        c1->capture();
    });
    /* Send frame */
    g1->securityAccessSendKey(id);
    receive_thread.join();
    /* TEST */
    testFrames(expected_frame, *c1);
}
/* Test for Service RoutinControll */
TEST_F(GenerateFramesTest, RoutinControlFrame) 
{
    std::vector<uint8_t> vct = {};
    /* Create expected frame */
    struct can_frame result_frame = createFrame({0x04,0x31,0x02,0x34,0x1A});
    /* Start listening for frame in the CAN-BUS */
    std::thread receive_thread([this]() {
        c1->capture();
    });
    /* Send frame */
    g1->routineControl(id,0x02,0x341A, vct, false);
    receive_thread.join();
    /* TEST */
    testFrames(result_frame, *c1);
}
/* Test for Service RoutinControll 2 */
TEST_F(GenerateFramesTest, RoutinControlFrame2) 
{
    std::vector<uint8_t> vct = {};
    /* Create expected frame */
    struct can_frame result_frame = createFrame({0x04,0x71,0x02,0x34,0x1A});
    /* Start listening for frame in the CAN-BUS */
    std::thread receive_thread([this]() {
        c1->capture();
    });
    /* Send frame */
    g1->routineControl(id,0x02,0x341A, vct, true);
    receive_thread.join();
    /* TEST */
    testFrames(result_frame, *c1);
}
/* Test for Service testerPresent */
TEST_F(GenerateFramesTest, testerPresent) 
{
    /* Create expected frame */
    struct can_frame result_frame = createFrame({0x02,0x3E,0x00});
    /* Start listening for frame in the CAN-BUS */
    std::thread receive_thread([this]() {
        c1->capture();
    });
    /* Send frame */
    g1->testerPresent(id);
    receive_thread.join();
    /* TEST */
    testFrames(result_frame, *c1);
}
/* Test for Service testerPresent 2 */
TEST_F(GenerateFramesTest, testerPresent2) 
{
    /* Create expected frame */
    struct can_frame result_frame = createFrame({0x02,0x7E,0x00});
    /* Start listening for frame in the CAN-BUS */
    std::thread receive_thread([this]() {
        c1->capture();
    });
    /* Send frame */
    g1->testerPresent(id,true);
    receive_thread.join();
    /* TEST */
    testFrames(result_frame, *c1);
}
/* Test for Service ReadByIdentifier */
TEST_F(GenerateFramesTest, ReadByIdentRespTest) 
{
    /* Create expected frame */
    struct can_frame result_frame = createFrame({0x05,0x62,0x33,0x22,0x32,0x11});
    /* Start listening for frame in the CAN-BUS */
    std::thread receive_thread([this]() {
        c1->capture();
    });
    /* Send frame */
    g1->readDataByIdentifier(id,0x3322,{0x32,0x11});
    receive_thread.join();
    /* TEST */
    testFrames(result_frame, *c1);
}
/* Test for Service ReadByIdentifier 2*/
TEST_F(GenerateFramesTest, ReadByIdentRespTest2) 
{
    /* Create expected frame */
    struct can_frame result_frame = createFrame({0x03,0x22,0x33,0x22});
    /* Start listening for frame in the CAN-BUS */
    std::thread receive_thread([this]() {
        c1->capture();
    });
    /* Send frame */
    g1->readDataByIdentifier(id,0x3322);
    receive_thread.join();
    /* TEST */
    testFrames(result_frame, *c1);
}
/* Test for Service ReadByIdentifier for multiple frames */
TEST_F(GenerateFramesTest, ReadByIdentLongRespTest) 
{
    /* Create expected frame */
    struct can_frame result_frame = createFrame({0x10,12,0x62,0x12,0x34,1,2,3});
    /* Start listening for frame in the CAN-BUS */ 
    std::thread receive_thread([this]() {
        c1->capture();
    });
    /*Send frame simulation*/
    std::vector<uint8_t> response = {1,2,3,4,5,6,7,8,9};
    g1->readDataByIdentifierLongResponse(id,0x1234,response, true);
    receive_thread.join();
    /* TEST */
    testFrames(result_frame, *c1);
}
/* Test for Service ReadMemoryByAddress */
TEST_F(GenerateFramesTest, ReadByAddressRespTest) 
{
    /* Create expected frame */
    struct can_frame result_frame = createFrame({0x07,0x63,0x12,0x23,0x45,0x01, 0x01,0x2});
    /* Start listening for frame in the CAN-BUS */
    std::thread receive_thread([this]() {
        c1->capture();
    });
    /* Send frame */
    g1->readMemoryByAddress(id,0x2345,0x01,{1,2});
    receive_thread.join();
    /* TEST */
    testFrames(result_frame, *c1);
}
/* Test for Service ReadMemoryByAddress 2 */
TEST_F(GenerateFramesTest, ReadByAddressRespTest2) 
{
    /* Create expected frame */
    struct can_frame result_frame = createFrame({0x05,0x23,0x12,0x23,0x45,0x01});
    /* Start listening for frame in the CAN-BUS */
    std::thread receive_thread([this]() {
        c1->capture();
    });
    /* Send frame */
    g1->readMemoryByAddress(id,0x2345,0x01);
    receive_thread.join();
    /* TEST */
    testFrames(result_frame, *c1);
}

/* Test for Service ReadMemoryByAddressLong for multiple frames */
TEST_F(GenerateFramesTest, ReadMemoryByAddressLong) 
{
    /* Create expected frame */
    struct can_frame result_frame = createFrame({0x21,0x02,0x03,0x04,0x05,0x06});
    /* Start listening for frame in the CAN-BUS */
    std::thread receive_thread([this]() {
        c1->capture();
    });
    /* Send frame */
    g1->readMemoryByAddressLongResponse(id,0x2345,0x01,{1,2,3,4,5,6},false);
    receive_thread.join();
    /* TEST */
    testFrames(result_frame, *c1);
}

/* Test for Service writeDataByIdentifier */
TEST_F(GenerateFramesTest, writeDataByIdentifier) 
{
    /* Create expected frame */
    struct can_frame result_frame = createFrame({0x03,0x6E,0x23,0x45});
    /* Start listening for frame in the CAN-BUS */
    std::thread receive_thread([this]() {
        c1->capture();
    });
    /* Send frame */
    g1->writeDataByIdentifier(id,0x2345, {});
    receive_thread.join();
    /* TEST */
    testFrames(result_frame, *c1);
}
/* Test for Service writeDataByIdentifier 2 */
TEST_F(GenerateFramesTest, writeDataByIdentifier2) 
{
    /* Create expected frame */
    struct can_frame result_frame = createFrame({0x05,0x2E,0x23,0x45,0x1,0x2});
    /* Start listening for frame in the CAN-BUS */
    std::thread receive_thread([this]() {
        c1->capture();
    });
    /* Send frame */
    g1->writeDataByIdentifier(id,0x2345,{1,2});
    receive_thread.join();
    /* TEST */
    testFrames(result_frame, *c1);
}

/* Test for Service writeDataByIdentifierLong */
TEST_F(GenerateFramesTest, writeDataByIdentifierLong) 
{
    /* Create expected frame */
    struct can_frame result_frame = createFrame({0x21, 0x04, 0x05, 0x06});
    /* Start listening for frame in the CAN-BUS */
    std::thread receive_thread([this]() {
        c1->capture();
    });
    /* Send frame */
    g1->writeDataByIdentifierLongData(id,0x2345,{0x01,0x02,0x03,0x04,0x05,0x06},false);
    receive_thread.join();
    /* TEST */
    testFrames(result_frame, *c1);
}


/* Test for Service flowControll */
TEST_F(GenerateFramesTest, flowControll) 
{
    /* Create expected frame */
    struct can_frame result_frame = createFrame({0x30,0x0,0x0,0x0});
    /* Start listening for frame in the CAN-BUS */ 
    std::thread receive_thread([this]() {
        c1->capture();
    });
    /* Send frame */
    g1->flowControlFrame(id);
    receive_thread.join();
    /* TEST */
    testFrames(result_frame, *c1);
}
/* Test for Service readDTC */
TEST_F(GenerateFramesTest, readDTC) 
{
    /* Create expected frame */
    struct can_frame result_frame = createFrame({0x3,0x19,0x01,0x12});
    /* Start listening for frame in the CAN-BUS */ 
    std::thread receive_thread([this]() {
        c1->capture();
    });
    /* Send frame */
    g1->readDtcInformation(id,0x01,0x12);
    receive_thread.join();
    /* TEST */
    testFrames(result_frame, *c1);
}
/* Test for Service readDTC 2 */
TEST_F(GenerateFramesTest, readDTC2) 
{
    /* Create expected frame */
    struct can_frame result_frame = createFrame({0x6,0x59,0x01,0x2,0x3,0x0,0x4});
    /* Start listening for frame in the CAN-BUS */ 
    std::thread receive_thread([this]() {
        c1->capture();
    });
    /* Send frame */
    g1->readDtcInformationResponse01(id,0x2,0x3,0x4);
    receive_thread.join();
    /* TEST */
    testFrames(result_frame, *c1);
}

/* Test for Service readDTC 3 */
TEST_F(GenerateFramesTest, readDTC3) 
{
    /* Create expected frame */
    struct can_frame result_frame = createFrame({0x04,0x59,0x02,0xff,0x44,0x44});
    /* Start listening for frame in the CAN-BUS */ 
    std::thread receive_thread([this]() {
        c1->capture();
    });
    /* Send frame */
    g1->readDtcInformationResponse02(id,0xff, {{0x44, 0x44}});
    receive_thread.join();
    /* TEST */
    testFrames(result_frame, *c1);
}

/* Test for Service readDTC 4 */
TEST_F(GenerateFramesTest, readDTC4) 
{
    /* Create expected frame */
    struct can_frame result_frame = createFrame({0x22,0x15});
    /* Start listening for frame in the CAN-BUS */ 
    std::thread receive_thread([this]() {
        c1->capture();
        c1->capture();
    });
    /* Send frame */
    g1->readDtcInformationResponse02Long(id,0xff, {{0x12, 0x34}, {0x56, 0x78}, {0x91, 0x11}, {0x12, 0x13}, {0x14, 0x15}}, false);
    receive_thread.join();
    /* TEST */
    testFrames(result_frame, *c1);
}

/* Test for Service readDTC 5 */
TEST_F(GenerateFramesTest, readDTC5) 
{
    /* Create expected frame */
    struct can_frame result_frame = createFrame({0x22, 0x14, 0x15});
    /* Start listening for frame in the CAN-BUS */ 
    std::thread receive_thread([this]() {
        c1->capture();
        c1->capture();
    });
    /* Send frame */
    g1->readDtcInformationResponse02Long(id,0xff, {{0xfffff, 0x34}, {0x56, 0x78}, {0x91, 0x11}, {0x12, 0x13}, {0x14, 0x15}}, false);
    receive_thread.join();
    /* TEST */
    testFrames(result_frame, *c1);
}

/* Test for Service clearDiagnosticInformation */
TEST_F(GenerateFramesTest, ClearDTCTest) 
{
    /* Create expected frame */
    struct can_frame result_frame = createFrame({0x04,0x14,0xFF,0xFF,0xFF});
    /* Start listening for frame in the CAN-BUS */ 
    std::thread receive_thread([this]() {
        c1->capture();
    });
    /* Send frame */
    g1->clearDiagnosticInformation(id);
    receive_thread.join();
    /* TEST */
    testFrames(result_frame, *c1);
}
/* Test for Service clearDiagnosticInformation 2 */
TEST_F(GenerateFramesTest, ClearDTCTest2) 
{
    /* Create expected frame */
    struct can_frame result_frame = createFrame({0x01,0x54});
    /* Start listening for frame in the CAN-BUS */ 
    std::thread receive_thread([this]() {
        c1->capture();
    });
    /* Send frame */
    g1->clearDiagnosticInformation(id,{},true);
    receive_thread.join();
    /* TEST */
    testFrames(result_frame, *c1);
}
/* Test for Service accessTimingParameters */
TEST_F(GenerateFramesTest, AccessTimeParamTest) 
{
    /* Create expected frame */
    struct can_frame result_frame = createFrame({0x02,0xc3,0x1});
    /* Start listening for frame in the CAN-BUS */
    std::thread receive_thread([this]() {
        c1->capture();
    });
    /* Send frame */
    g1->accessTimingParameters(id,0x01, {}, true);
    receive_thread.join();
    /* TEST */
    testFrames(result_frame, *c1);
}
/* Test for Service accessTimingParameters 2 */
TEST_F(GenerateFramesTest, AccessTimeParamTest2) 
{
    /* Create expected frame */
    struct can_frame result_frame = createFrame({0x02,0xc3,0x01});
    /* Start listening for frame in the CAN-BUS */
    std::thread receive_thread([this]() {
        c1->capture();
    });
    /* Send frame */
    g1->accessTimingParameters(id,0x01, {0x02, 0xc3, 0x01}, true);
    receive_thread.join();
    /* TEST */
    testFrames(result_frame, *c1);
}

/* Test for Service accessTimingParameters 3*/
TEST_F(GenerateFramesTest, AccessTimeParamTest3) 
{
    /* Create expected frame */
    struct can_frame result_frame = createFrame({0x02,0x83,0x1});
    /* Start listening for frame in the CAN-BUS */
    std::thread receive_thread([this]() {
        c1->capture();
    });
    /* Send frame */
    g1->accessTimingParameters(id,0x01, {}, false);
    receive_thread.join();
    /* TEST */
    testFrames(result_frame, *c1);
}
/* Test for NegativeResponse */
TEST_F(GenerateFramesTest, NegativeResponse) 
{
    /* Create expected frame */
    struct can_frame result_frame = createFrame({0x03,0x7F,0x10,0x12});
    /* Start listening for frame in the CAN-BUS */
    std::thread receive_thread([this]() {
        c1->capture();
    });
    /* Send frame */
    g1->negativeResponse(id,0x10,0x012);
    receive_thread.join();
    /* TEST */
    testFrames(result_frame, *c1);
}

/* Test for Service requestDownload a response */
TEST_F(GenerateFramesTest, RequestDownloadTest2) 
{
    /* Create expected frame */
    struct can_frame result_frame = createFrame({0x04,0x74, 0x20,0x22, 0x34});
    /* Start listening for frame in the CAN-BUS */ 
    std::thread receive_thread([this]() {
        c1->capture();
    });
    /* Send frame */
    g1->requestDownloadResponse(id,0x2234);
    receive_thread.join();
    /* TEST */
    testFrames(result_frame, *c1);
}
/* Test for Service requestDownload */
TEST_F(GenerateFramesTest, RequestDownloadTest3) 
{
    /* Create expected frame */
    struct can_frame result_frame = createFrame({0x04,0x34, 0x00, 0x10, 0x10, 0x05});
    /* Start listening for frame in the CAN-BUS */ 
    std::thread receive_thread([this]() {
        c1->capture();
    });
    /* Send frame */
    g1->requestDownload(id,0x00,-2,0x10, 0x05);
    receive_thread.join();
    /* TEST */
    testFrames(result_frame, *c1);
}

/* Test for Service requestDownloadLong */
TEST_F(GenerateFramesTest, requestDownloadLong) 
{
    /* Create expected frame */
    struct can_frame result_frame = createFrame({0x21,0x10, 0x05});
    /* Start listening for frame in the CAN-BUS */ 
    std::thread receive_thread([this]() {
        c1->capture();
    });
    /* Send frame */
    g1->requestDownloadLong(id,0x00,0x3445,0x10, 0x05, false);
    receive_thread.join();
    /* TEST */
    testFrames(result_frame, *c1);
}

/* Test for Service transferData */
TEST_F(GenerateFramesTest, TransferDataTest) 
{
    /* Create expected frame */
    struct can_frame result_frame = createFrame({0x07,0x36, 0x20, 1,2,3,4,5});
    /* Start listening for frame in the CAN-BUS */ 
    std::thread receive_thread([this]() {
        c1->capture();
    });
    /* Send frame */
    g1->transferData(id, 0x20,{1,2,3,4,5});
    receive_thread.join();
    /* TEST */
    testFrames(result_frame, *c1);
}
/* Second Test for Service transferData */
TEST_F(GenerateFramesTest, TransferDataTest2) 
{
    /* Create expected frame */
    struct can_frame result_frame = createFrame({0x02,0x76, 0x20});
    /* Start listening for frame in the CAN-BUS */ 
    std::thread receive_thread([this]() {
        c1->capture();
    });
    /* Send frame */
    g1->transferData(id, 0x20);
    receive_thread.join();
    /* TEST */
    testFrames(result_frame, *c1);
}
/* Test for Service transferData for multiple frames */
TEST_F(GenerateFramesTest, TransferDataLongTest) 
{
    /* Create expected frame */
    struct can_frame result_frame = createFrame({0x21,0x05,0x06,0x07});
    /* Start listening for frame in the CAN-BUS */ 
    std::thread receive_thread([this]() {
        c1->capture();
    });
    /*Send frame simulation*/
    std::vector<uint8_t> data = {0x01,0x02,0x03,0x04,0x05,0x06,0x07};
    g1->transferDataLong(id,0x20,data,false);
    receive_thread.join();
    /* TEST */
    testFrames(result_frame, *c1);
}
/* Second Test for Service transferData for multiple frames */
TEST_F(GenerateFramesTest, TransferDataLongTest2) 
{
    /* Create expected frame */
    struct can_frame result_frame = createFrame({0x05,0x36,0x20,1,2,3});
    /* Start listening for frame in the CAN-BUS */ 
    std::thread receive_thread([this]() {
        c1->capture();
    });
    /*Send frame simulation*/
    std::vector<uint8_t> data = {1,2,3};
    if (data.size() > 4)
    {
        g1->transferDataLong(id,0x20,data);
    }
    else{
        g1->transferData(id,0x20,data);
    }
    receive_thread.join();
    /* TEST */
    testFrames(result_frame, *c1);
}
/* Test for Service requestTransferExit */
TEST_F(GenerateFramesTest, ReqTransferExit) 
{
    /* Create expected frame */
    struct can_frame result_frame = createFrame({0x01,0x37});
    /* Start listening for frame in the CAN-BUS */ 
    std::thread receive_thread([this]() {
        c1->capture();
    });
    /* Send frame */
    g1->requestTransferExit(id);
    receive_thread.join();
    /* TEST */
    testFrames(result_frame, *c1);
}
/* Test for Service requestTransferExit 2 */
TEST_F(GenerateFramesTest, ReqTransferExit2) 
{
    /* Create expected frame */
    struct can_frame result_frame = createFrame({0x01,0x77});
    /* Start listening for frame in the CAN-BUS */ 
    std::thread receive_thread([this]() {
        c1->capture();
    });
    /* Send frame */
    g1->requestTransferExit(id,true);
    receive_thread.join();
    /* TEST */
    testFrames(result_frame, *c1);
}
/* Test for LonFrames Error */
TEST_F(GenerateFramesTest, ErrorLongResponse) 
{
    testing::internal::CaptureStdout();
    /* Send frame */
    g1->readDataByIdentifier(id,0x2345,{1,2,3,4,5});
    std::string output = testing::internal::GetCapturedStdout();
    EXPECT_NE(output.find("ERROR: The frame is to long!, consider using method ReadDataByIdentifierLongResponse"), std::string::npos);
    testing::internal::CaptureStdout();
    /* Send frame */
    g1->readMemoryByAddress(id,0x23,45,{1,2,3,4});
    output = testing::internal::GetCapturedStdout();
    EXPECT_NE(output.find("ERROR: Response to long, consider using ReadMemoryByAdressLongResponse method"), std::string::npos);
    testing::internal::CaptureStdout();
    /* Send frame */
    g1->writeDataByIdentifier(id,0x2345,{1,2,3,4,5});
    output = testing::internal::GetCapturedStdout();
    EXPECT_NE(output.find("The data_parameter is to long. Consider using WriteDataByIdentifierLongData method"), std::string::npos);
    testing::internal::CaptureStdout();
    /* Send frame */
    g1->transferData(id,0x20,{1,2,3,4,5,6});
    output = testing::internal::GetCapturedStdout();
    EXPECT_NE(output.find("The transfer_request is to long. Consider using transferDataLong method"), std::string::npos);
    testing::internal::CaptureStdout();
    /* Send frame */
    g1->clearDiagnosticInformation(id,{0x1,2,3,4,5,6,7});
    output = testing::internal::GetCapturedStdout();
    EXPECT_NE(output.find("ERROR: Can't send more than 6 DTC/frame, please consider send 2 or more frames"), std::string::npos);
    testing::internal::CaptureStdout();
    /* Send frame */
    g1->sendFrame(id,{1,2,3,4,5,6,7,8,9});
    output = testing::internal::GetCapturedStdout();
    EXPECT_NE(output.find("Write error"), std::string::npos);
    testing::internal::CaptureStdout();
    /* Send frame */
    std::vector<uint8_t> response = {1,2,3};
    g1->readDataByIdentifierLongResponse(id,0x1234,response, true);
    output = testing::internal::GetCapturedStdout();
    EXPECT_NE(output.find("The data_parameter is to long. Consider using writeDataByIdentifier method\n"), std::string::npos);
    testing::internal::CaptureStdout();
    /* Send frame */
    g1->readMemoryByAddressLongResponse(id,0x2345,0x01,{1,2},false);
    output = testing::internal::GetCapturedStdout();
    EXPECT_NE(output.find("The data_parameter is to long. Consider using writeDataByIdentifier method\n"), std::string::npos);
    testing::internal::CaptureStdout();
    /* Send frame */
    g1->writeDataByIdentifierLongData(id,0x2345,{0x01,0x02},false);
    output = testing::internal::GetCapturedStdout();
    EXPECT_NE(output.find("The data_parameter is to long. Consider using writeDataByIdentifier method\n"), std::string::npos);
    testing::internal::CaptureStdout();
    /* Send frame */
    g1->readDtcInformationResponse02(id,0xff, {{0x44, 0x44},{0x44, 0x44},{0x44, 0x44}});
    output = testing::internal::GetCapturedStdout();
    EXPECT_NE(output.find("The data_parameter is to long. Consider using readDtcInformationResponse02Long method\n"), std::string::npos);
    testing::internal::CaptureStdout();
    /* Send frame */
    g1->readDtcInformationResponse02Long(id,0xff, {{0x12, 0x34}}, false);
    output = testing::internal::GetCapturedStdout();
    EXPECT_NE(output.find("The data_parameter is to long. Consider using readDtcInformationResponse02 method\n"), std::string::npos);
    testing::internal::CaptureStdout();
    /* Send frame */
    std::vector<uint8_t> data = {0x01,0x02};
    g1->transferDataLong(id,0x20,data,false);
    output = testing::internal::GetCapturedStdout();
    EXPECT_NE(output.find("The data_parameter is to long. Consider using transferData method\n"), std::string::npos);
}

/* Test for Service request UpdateStatus */
TEST_F(GenerateFramesTest, ReqStatus) 
{
    /* Send frame */
    bool response = g1->requestUpdateStatusResponse(id,{0x10, 0x12});
    /* TEST */
    EXPECT_EQ(response, true);
}

/* Test for Service request UpdateStatus */
TEST_F(GenerateFramesTest, ReqStatus2) 
{
    /* Send frame */
    bool response = g1->requestUpdateStatusResponse(id,{0x10, 0x7F});
    /* TEST */
    EXPECT_EQ(response, false);
}

int main(int argc, char* argv[])
{
    s1 = createSocket();
    s2 = createSocket();
    testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}