/**
 * @file ClearDtcTest.cpp
 * @author Mujdei Ruben
 * @brief 
 * @version 0.1
 * @date 2024-07-24
 * 
 * @copyright Copyright (c) 2024
 * 
 * PS: Test work with the next data in the file './dtcs.txt':
 *          P0A9B-17 24
 *          P0805-11 2F
 *          C0805-11 2F
 *          B1234-00 00
 *          P0B12-01 10
 *          C0B12-01 10
 * The file is created automatically!!
 */
#include <cstring>
#include <string>
#include <thread>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <gtest/gtest.h>
#include <net/if.h>

#include "../include/ClearDtc.h"
#include "../../../utils/include/ReceiveFrames.h"

int socket1;
int socket2;
const int id = 0x10FA;

std::vector<uint8_t> seed;

class CaptureFrame
{
    public:
        struct can_frame frame;
        void capture()
        {
            read(socket1, &frame, sizeof(struct can_frame));
        }
};

struct can_frame createFrame(uint16_t can_id ,std::vector<uint8_t> test_data)
{
    struct can_frame result_frame;
    result_frame.can_id = can_id;
    int i=0;
    for (auto d : test_data)
    {
        result_frame.data[i++] = d;
    }
    result_frame.can_dlc = test_data.size();
    return result_frame;
}

int createSocket()
{
    /* Create socket */
    std::string name_interface = "vcan1";
    struct sockaddr_can addr;
    struct ifreq ifr;
    int s;

    s = socket(PF_CAN, SOCK_RAW, CAN_RAW);
    if (s < 0)
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
    int b = bind(s, (struct sockaddr*)&addr, sizeof(addr));
    if( b < 0 )
    {
        std::cout<<"Error binding\n";
        return 1;
    }
    int flags = fcntl(s, F_GETFL, 0);
    if (flags == -1)
    {
        return 1;
    }
    /* Set the O_NONBLOCK flag to make the socket non-blocking */
    flags |= O_NONBLOCK;
    if (fcntl(s, F_SETFL, flags) == -1)
    {
        return -1;
    }
    return s;
}

void testFrames(struct can_frame expected_frame, CaptureFrame &c1 )
{
    EXPECT_EQ(expected_frame.can_id & 0xFFFF, c1.frame.can_id & 0xFFFF);
    EXPECT_EQ(expected_frame.can_dlc, c1.frame.can_dlc);
    for (int i = 0; i < expected_frame.can_dlc; ++i)
    {
        EXPECT_EQ(expected_frame.data[i], c1.frame.data[i]);
    }
}

bool containsLine(const std::string& output, const std::string& line)
{
    return output.find(line) != std::string::npos;
}

uint8_t computeKey(uint8_t& seed)
{
    return ~seed + 1;
}

struct ClearDtcTest : testing::Test
{
    ClearDtc* c;
    SecurityAccess* r;
    CaptureFrame* c1;
    Logger* logger;
    ClearDtcTest()
    {
        logger = new Logger();
        std::string dtc_file_path = std::string(PROJECT_PATH) + "/src/ecu_simulation/EngineModule/dtcs.txt";
        c = new ClearDtc(dtc_file_path ,*logger, socket2);
        r = new SecurityAccess(socket2, *logger);
        c1 = new CaptureFrame();
    }
    ~ClearDtcTest()
    {
        delete c;
        delete r;
        delete c1;
        delete logger;
    }
};

/* Test for Incorrect message length */
TEST_F(ClearDtcTest, IncorrectMessageLength) {
    std::cerr << "Running IncorrectMessageLength" << std::endl;

    struct can_frame result_frame = createFrame(0x10FA, {0x03, 0x7F, 0x14, NegativeResponse::IMLOIF});
    c->clearDtc(0xFA10, {0x02, 0x14, 0x10});
    c1->capture();
    testFrames(result_frame, *c1);

    std::cerr << "Finished IncorrectMessageLength" << std::endl;
}

/* Test for Invalid Group */
TEST_F(ClearDtcTest, InvalidGroup) {
    std::cerr << "Running InvalidGroup" << std::endl;

    struct can_frame result_frame = createFrame(0x10FA, {0x03, 0x7F, 0x14, NegativeResponse::ROOR});
    c->clearDtc(0xFA10, {0x04, 0x14, 0x01, 0x0D, 0xDD});
    c1->capture();
    testFrames(result_frame, *c1);

    std::cerr << "Finished InvalidGroup" << std::endl;
}

/* Test for Clear Body DTCs */
TEST_F(ClearDtcTest, ClearBodyDTCs) {
    std::cerr << "Running ClearBodyDTCs" << std::endl;

    struct can_frame result_frame = createFrame(0x10FA, {0x01, 0x54});
    c->clearDtc(0xFA10, {0x04, 0x14, 0x02, 0x0A, 0xAA});
    c1->capture();
    testFrames(result_frame, *c1);

    std::cerr << "Finished ClearBodyDTCs" << std::endl;
}

/* Test for Clear Powertrain DTCs */
TEST_F(ClearDtcTest, ClearPowertrainDTCs) {
    std::cerr << "Running ClearPowertrainDTCs" << std::endl;

    struct can_frame result_frame = createFrame(0x10FA, {0x01, 0x54});
    c->clearDtc(0xFA10, {0x04, 0x14, 0x01, 0x0A, 0xAA});
    c1->capture();
    testFrames(result_frame, *c1);

    std::cerr << "Finished ClearPowertrainDTCs" << std::endl;
}

/* Test for Clear all DTCs */
TEST_F(ClearDtcTest, ClearAllDTCs) {
    std::cerr << "Running ClearAllDTCs" << std::endl;

    struct can_frame result_frame = createFrame(0x10FA, {0x01, 0x54});
    c->clearDtc(0xFA10, {0x04, 0x14, 0xFF, 0xFF, 0xFF});
    c1->capture();
    testFrames(result_frame, *c1);

    std::cerr << "Finished ClearAllDTCs" << std::endl;
}

/* Test for Wrong Path */
TEST_F(ClearDtcTest, WrongPath) {
    std::cerr << "Running WrongPath" << std::endl;

    std::string dtc_file_path = std::string(PROJECT_PATH) + "/src/ecu_simulation/dtcs.txt";
    c = new ClearDtc(dtc_file_path ,*logger, socket2);

    struct can_frame result_frame = createFrame(0x10FA, {0x03, 0x7F, 0x14, 0x22});
    c->clearDtc(0xFA10, {0x04, 0x14, 0x01, 0x0A, 0xAA});
    c1->capture();
    testFrames(result_frame, *c1);

    std::cerr << "Finished WrongPath" << std::endl;
}


int main(int argc, char* argv[])
{
    socket1 = createSocket();
    socket2 = createSocket();
    testing::InitGoogleTest(&argc, argv);
    int result = RUN_ALL_TESTS();
    if (socket1 > 0)
    {
        close(socket1);
    }
    if (socket2 > 0)
    {
        close(socket2);
    }
    return result;
}