/**
 * @file ReadDataByIdentifierTest.cpp
 * @author Theodor Stoica
 * @brief Unit test for Read Data By Identifier Service
 * @version 0.1
 * @date 2024-10-9
 */
#include <gtest/gtest.h>
#include "../include/ReadDataByIdentifier.h"
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

struct ReadDataByIdentifierTest : testing::Test
{
    ReadDataByIdentifier* rdbi;
    SecurityAccess* security;
    CaptureFrame* c1;
    Logger* logger;
    ReadDataByIdentifierTest()
    {
        logger = new Logger();
        rdbi = new ReadDataByIdentifier(socket2, *logger);
        security = new SecurityAccess(socket2, *logger);
        c1 = new CaptureFrame();
    }
    ~ReadDataByIdentifierTest()
    {
        delete rdbi;
        delete security;
        delete c1;
        delete logger;
    }
};

TEST_F(ReadDataByIdentifierTest, IncorrectMessageLength)
{
    struct can_frame result_frame = createFrame(0x10FA, {0x03, 0x7F, 0x22, NegativeResponse::IMLOIF});
    rdbi->readDataByIdentifier(0xFA10, {0x01, 0x22}, true);
    c1->capture();
    testFrames(result_frame, *c1);
}

TEST_F(ReadDataByIdentifierTest, MCUSecurity)
{
    struct can_frame result_frame = createFrame(0x10FA, {0x03, 0x7F, 0x22, NegativeResponse::SAD});
    rdbi->readDataByIdentifier(0xFA10, {0x04, 0x22, 0xf1, 0x90, 0x11}, true);
    c1->capture();
    testFrames(result_frame, *c1);
}

TEST_F(ReadDataByIdentifierTest, ECUsSecurity)
{
    /* Battery Module */
    struct can_frame result_frame = createFrame(0x11FA, {0x03, 0x7F, 0x22, NegativeResponse::SAD});

    rdbi->readDataByIdentifier(0xFA11, {0x04, 0x22, 0xf1, 0x90, 0x11}, true);
    c1->capture();
    testFrames(result_frame, *c1);

    /* Engine Module */
    result_frame = createFrame(0x12FA, {0x03, 0x7F, 0x22, NegativeResponse::SAD});

    rdbi->readDataByIdentifier(0xFA12, {0x04, 0x22, 0xf1, 0x90, 0x11}, true);
    c1->capture();
    testFrames(result_frame, *c1);

    /* Doors Module */
    result_frame = createFrame(0x13FA, {0x03, 0x7F, 0x22, NegativeResponse::SAD});

    rdbi->readDataByIdentifier(0xFA13, {0x04, 0x22, 0xf1, 0x90, 0x11}, true);
    c1->capture();
    testFrames(result_frame, *c1);

    /* HVAC Module */
    result_frame = createFrame(0x14FA, {0x03, 0x7F, 0x22, NegativeResponse::SAD});
    rdbi->readDataByIdentifier(0xFA14, {0x04, 0x22, 0xf1, 0x90, 0x11}, true);
    c1->capture();
    testFrames(result_frame, *c1);
}

TEST_F(ReadDataByIdentifierTest, RequestOutOfRangeMCU)
{

    struct can_frame result_frame = createFrame(0x10FA, {0x03, 0x7F, 0x22, NegativeResponse::ROOR});

    /* Check the security */
    /* Request seed */
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

    rdbi->readDataByIdentifier(0xFA10, {0x04, 0x22, 0x11, 0x11, 0x11}, true);
    c1->capture();
    testFrames(result_frame, *c1);
}

/* Test Request Out Of Range Battery */
TEST_F(ReadDataByIdentifierTest, RequestOutOfRangeBattery)
{
    ReceiveFrames* receiveFrames = new ReceiveFrames(socket2, 0x11, *logger);
    receiveFrames->setEcuState(true);
    struct can_frame result_frame = createFrame(0x11FA, {0x03, 0x7F, 0x22, NegativeResponse::ROOR});
    rdbi->readDataByIdentifier(0xFA11, {0x04, 0x22, 0x11, 0x11, 0x11}, true);
    c1->capture();
    testFrames(result_frame, *c1);
}

/* Test Request Out Of Range Engine */
TEST_F(ReadDataByIdentifierTest, RequestOutOfRangeEngine)
{
    struct can_frame result_frame = createFrame(0x12FA, {0x03, 0x7F, 0x22, NegativeResponse::ROOR});
    rdbi->readDataByIdentifier(0xFA12, {0x04, 0x22, 0x11, 0x11, 0x11}, true);
    c1->capture();
    testFrames(result_frame, *c1);
}

/* Test Request Out Of Range Doors */
TEST_F(ReadDataByIdentifierTest, RequestOutOfRangeDoors)
{
    struct can_frame result_frame = createFrame(0x13FA, {0x03, 0x7F, 0x22, NegativeResponse::ROOR});
    rdbi->readDataByIdentifier(0xFA13, {0x04, 0x22, 0x11, 0x11, 0x11}, true);
    c1->capture();
    testFrames(result_frame, *c1);
}

/* Test Request Out Of Range HVAC */
TEST_F(ReadDataByIdentifierTest, RequestOutOfRangeHVAC)
{
    struct can_frame result_frame = createFrame(0x13FA, {0x03, 0x7F, 0x22, NegativeResponse::ROOR});
    rdbi->readDataByIdentifier(0xFA13, {0x04, 0x22, 0x11, 0x11, 0x11}, true);
    c1->capture();
    testFrames(result_frame, *c1);
}

/* Test Module not supported */
TEST_F(ReadDataByIdentifierTest, ModuleNotSupported)
{
    struct can_frame result_frame = createFrame(0x15FA, {0x03, 0x7F, 0x22, NegativeResponse::ROOR});

    rdbi->readDataByIdentifier(0xFA15, {0x04, 0x22, 0x11, 0x11, 0x11}, true);
    c1->capture();
    testFrames(result_frame, *c1);
}

TEST_F(ReadDataByIdentifierTest, ErrorReadingFromFile)
{
    std::string file_name = std::string(PROJECT_PATH) + "/src/mcu/mcu_data.txt";
    std::string original_content;
    std::ifstream original_file(file_name);
    if (original_file)
    {
        original_content.assign((std::istreambuf_iterator<char>(original_file)),
                                std::istreambuf_iterator<char>());
        original_file.close();
    }
    std::remove(file_name.c_str());

    struct can_frame result_frame = createFrame(0x10FA, {0x03, 0x7F, 0x22, NegativeResponse::ROOR});
    rdbi->readDataByIdentifier(0xFA10, {0x04, 0x22, 0x11, 0x11, 0x11}, true);
    c1->capture();

    std::ofstream new_file(file_name);
    if (new_file)
    {
        if (!original_content.empty())
        {
            new_file << original_content;
        }
        new_file.close();
    }
    else
    {
        FAIL() << "Error recreating the file.";
    }
    std::ifstream recreated_file(file_name);
    EXPECT_TRUE(recreated_file.good());

    if (!original_content.empty())
    {
        std::string recreated_content((std::istreambuf_iterator<char>(recreated_file)),
                                      std::istreambuf_iterator<char>());
        EXPECT_EQ(recreated_content, original_content);
    }
    testFrames(result_frame, *c1);
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