/**
 * @file EcuResetTest.cpp
 * @author Theodor Stoica
 * @brief Unit test for ECU Reset Service
 * @version 0.1
 * @date 2024-10-11
 */
#include <gtest/gtest.h>
#include "../include/EcuReset.h"
#include "../../../utils/include/ReceiveFrames.h"

int socket1;
int socket2;

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

struct EcuResetTest : testing::Test
{
    SecurityAccess* security;
    CaptureFrame* c1;
    Logger* logger;
    EcuResetTest()
    {
        logger = new Logger();
        security = new SecurityAccess(socket2, *logger);
        c1 = new CaptureFrame();
    }
    ~EcuResetTest()
    {
        delete security;
        delete c1;
        delete logger;
    }
};

TEST_F(EcuResetTest, ConstructorInitializesFieldsCorrectly)
{
    EcuReset *ecuReset;
    EXPECT_NO_THROW(
    {
        ecuReset = new EcuReset(0xFA10, 0x01, socket2, *logger);
    });
    delete ecuReset;
}

TEST_F(EcuResetTest, IncorrectMessageLength)
{
    EcuReset *ecuReset;
    ecuReset = new EcuReset(0xFA10, 0x01, socket2, *logger);
    struct can_frame result_frame = createFrame(0x10FA, {0x03, 0x7F, 0x11, NegativeResponse::IMLOIF});
    ecuReset->ecuResetRequest({0x01, 0x11});
    c1->capture();
    testFrames(result_frame, *c1);
    delete ecuReset;
}

TEST_F(EcuResetTest, SubFunctionNotSupported)
{
    EcuReset *ecuReset;
    ecuReset = new EcuReset(0xFA10, 0x03, socket2, *logger);
    struct can_frame result_frame = createFrame(0x10FA, {0x03, 0x7F, 0x11, NegativeResponse::SFNS});
    ecuReset->ecuResetRequest({0x02, 0x11,0x03});
    c1->capture();
    testFrames(result_frame, *c1);
    delete ecuReset;
}

TEST_F(EcuResetTest, MCUSecurity)
{
    EcuReset *ecuReset;
    ecuReset = new EcuReset(0xFA10, 0x01, socket2, *logger);
    struct can_frame result_frame = createFrame(0x10FA, {0x03, 0x7F, 0x11, NegativeResponse::SAD});
    ecuReset->ecuResetRequest({0x02, 0x11,0x01});
    c1->capture();
    testFrames(result_frame, *c1);
    delete ecuReset;
}

TEST_F(EcuResetTest, ECUSecurity)
{
    /* Battery */
    EcuReset *ecuReset;
    ecuReset = new EcuReset(0xFA11, 0x01, socket2, *logger);
    struct can_frame result_frame = createFrame(0x11FA, {0x03, 0x7F, 0x11, NegativeResponse::SAD});
    ecuReset->ecuResetRequest({0x02, 0x11,0x01});
    c1->capture();
    testFrames(result_frame, *c1);
    delete ecuReset;

    /* Engine */
    ecuReset = new EcuReset(0xFA12, 0x01, socket2, *logger);
    result_frame = createFrame(0x12FA, {0x03, 0x7F, 0x11, NegativeResponse::SAD});
    ecuReset->ecuResetRequest({0x02, 0x11,0x01});
    c1->capture();
    testFrames(result_frame, *c1);
    delete ecuReset;

    /* Doors */
    ecuReset = new EcuReset(0xFA13, 0x01, socket2, *logger);
    result_frame = createFrame(0x13FA, {0x03, 0x7F, 0x11, NegativeResponse::SAD});
    ecuReset->ecuResetRequest({0x02, 0x11,0x01});
    c1->capture();
    testFrames(result_frame, *c1);
    delete ecuReset;

    /* HVAC */
    ecuReset = new EcuReset(0xFA14, 0x01, socket2, *logger);
    result_frame = createFrame(0x14FA, {0x03, 0x7F, 0x11, NegativeResponse::SAD});
    ecuReset->ecuResetRequest({0x02, 0x11,0x01});
    c1->capture();
    testFrames(result_frame, *c1);
    delete ecuReset;
}

TEST_F(EcuResetTest, HardResetMCU)
{
    EcuReset *ecuReset;

    security->securityAccess(0xFA10, {0x02, 0x27, 0x01});

    c1->capture();
    if (c1->frame.can_dlc >= 4)
    {
        seed.clear();
        /* from 3 to pci_length we have the seed generated in response */
        for (int i = 3; i <= c1->frame.data[0]; i++)
        {
            seed.push_back(c1->frame.data[i]);
        }
    }
    /* Compute key from seed */
    for (auto &elem : seed)
    {
        elem = computeKey(elem);
    }
    std::vector<uint8_t> data_frame = {static_cast<uint8_t>(seed.size() + 2), 0x27, 0x02};
    data_frame.insert(data_frame.end(), seed.begin(), seed.end());
    security->securityAccess(0xFA10, data_frame);
    c1->capture();

    ecuReset = new EcuReset(0xFA10, 0x01, socket2, *logger);
    struct can_frame result_frame = createFrame(0x10FA, {0x02, 0x51,0x01});
    ecuReset->ecuResetRequest({0x02, 0x11,0x01});
    c1->capture();
    testFrames(result_frame, *c1);
    delete ecuReset;
}

TEST_F(EcuResetTest, HardResetECU)
{
    /* Battery */
    EcuReset *ecuReset;
    ReceiveFrames* receiveFrames = new ReceiveFrames(socket2, 0x11, *logger);
    receiveFrames->setEcuState(true);
    ecuReset = new EcuReset(0xFA11, 0x01, socket2, *logger);
    struct can_frame result_frame = createFrame(0x11FA, {0x02, 0x51,0x01});
    ecuReset->ecuResetRequest({0x02, 0x11,0x01});
    c1->capture();
    testFrames(result_frame, *c1);
    delete ecuReset;
    delete receiveFrames;

    /* Engine */
    ecuReset = new EcuReset(0xFA12, 0x01, socket2, *logger);
    result_frame = createFrame(0x12FA, {0x02, 0x51,0x01});
    ecuReset->ecuResetRequest({0x02, 0x11,0x01});
    c1->capture();
    testFrames(result_frame, *c1);
    delete ecuReset;

    /* Doors */
    ecuReset = new EcuReset(0xFA13, 0x01, socket2, *logger);
    result_frame = createFrame(0x13FA, {0x02, 0x51,0x01});
    ecuReset->ecuResetRequest({0x02, 0x11,0x01});
    c1->capture();
    testFrames(result_frame, *c1);
    delete ecuReset;

    /* HVAC */
    ecuReset = new EcuReset(0xFA14, 0x01, socket2, *logger);
    result_frame = createFrame(0x14FA, {0x02, 0x51,0x01});
    ecuReset->ecuResetRequest({0x02, 0x11,0x01});
    c1->capture();
    testFrames(result_frame, *c1);
    delete ecuReset;

    /* Other ECU */
    ecuReset = new EcuReset(0xFA15, 0x01, socket2, *logger);
    ecuReset->ecuResetRequest({0x02, 0x11,0x01});
    c1->capture();
    delete ecuReset;
}

TEST_F(EcuResetTest, SoftResetMCU)
{
    EcuReset *ecuReset;
    ecuReset = new EcuReset(0xFA10, 0x02, socket2, *logger);
    struct can_frame result_frame = createFrame(0x10FA, {0x02, 0x51,0x02});
    ecuReset->ecuResetRequest({0x02, 0x11, 0x02});
    c1->capture();
    testFrames(result_frame, *c1);
    delete ecuReset;
}

TEST_F(EcuResetTest, SoftResetECU)
{
    /* Battery */
    EcuReset *ecuReset;
    ecuReset = new EcuReset(0xFA11, 0x02, socket2, *logger);
    struct can_frame result_frame = createFrame(0x11FA, {0x02, 0x51,0x02});
    ecuReset->ecuResetRequest({0x02, 0x11, 0x02});
    c1->capture();
    testFrames(result_frame, *c1);
    delete ecuReset;

    /* Engine */
    ecuReset = new EcuReset(0xFA12, 0x02, socket2, *logger);
    result_frame = createFrame(0x12FA, {0x02, 0x51,0x02});
    ecuReset->ecuResetRequest({0x02, 0x11, 0x02});
    c1->capture();
    testFrames(result_frame, *c1);
    delete ecuReset;

    /* Doors */
    ecuReset = new EcuReset(0xFA13, 0x02, socket2, *logger);
    result_frame = createFrame(0x13FA, {0x02, 0x51,0x02});
    ecuReset->ecuResetRequest({0x02, 0x11, 0x02});
    c1->capture();
    testFrames(result_frame, *c1);
    delete ecuReset;

    /* HVAC */
    ecuReset = new EcuReset(0xFA14, 0x02, socket2, *logger);
    result_frame = createFrame(0x14FA, {0x02, 0x51,0x02});
    ecuReset->ecuResetRequest({0x02, 0x11, 0x02});
    c1->capture();
    testFrames(result_frame, *c1);
    delete ecuReset;

    /* Other ECU */
    ecuReset = new EcuReset(0xFA15, 0x02, socket2, *logger);
    result_frame = createFrame(0x15FA, {0x02, 0x51,0x02});
    ecuReset->ecuResetRequest({0x02, 0x11, 0x02});
    c1->capture();
    testFrames(result_frame, *c1);
    delete ecuReset;
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