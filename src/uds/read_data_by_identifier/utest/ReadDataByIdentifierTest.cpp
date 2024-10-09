#include <gtest/gtest.h>
#include "../include/ReadDataByIdentifier.h"
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

// TEST_F(ReadDataByIdentifierTest, InvalidMessageLength) {
//     canid_t can_id = 0x10; 
//     MCU::mcu->mcu_data[0x1234] = {0x01, 0x02, 0x03, 0x04};
//     /* ReadDataByIdentifier request for data identifier 0x1234 */
//     std::vector<uint8_t> request = {0x22, 0x01, 0x12};

//     std::vector<uint8_t> expected_response = {0x03, 0x7F, 0x22, 0x13};
//     std::vector<uint8_t> actual_response = rdbi->readDataByIdentifier(can_id, request, true);

//     EXPECT_EQ(expected_response, actual_response);
// }

// TEST_F(ReadDataByIdentifierTest, ValidRequestForMCUData) {
//     canid_t can_id = 0x10; 
//     MCU::mcu->mcu_data[0x1234] = {0x01, 0x02, 0x03, 0x04};
//     /* ReadDataByIdentifier request for data identifier 0x1234 */
//     std::vector<uint8_t> request = {0x22, 0x01, 0x12, 0x34};

//     std::vector<uint8_t> expected_response = {0x01, 0x02, 0x03, 0x04};
//     std::vector<uint8_t> actual_response = rdbi->readDataByIdentifier(can_id, request, true);

//     EXPECT_EQ(expected_response, actual_response);
// }


// TEST_F(ReadDataByIdentifierTest, ValidRequestForBatteryData) {
//     ASSERT_NE(battery, nullptr);
//     canid_t can_id = 0x1011;
//     battery->ecu_data[0x01A0] = {0x01, 0x02, 0x03, 0x04};
//     /* ReadDataByIdentifier request for data identifier 0x5678 */
//     std::vector<uint8_t> request = {0x22, 0x01, 0x01, 0xA0};

//     std::vector<uint8_t> expected_response = {0x01, 0x02, 0x03, 0x04};
//     std::vector<uint8_t> actual_response = rdbi->readDataByIdentifier(can_id, request, false);

//     EXPECT_EQ(expected_response, actual_response);
// }

// TEST_F(ReadDataByIdentifierTest, InvalidRequestLength) {
//     canid_t can_id = 0x1000;
//     /* Invalid request (length < 4) */
//     std::vector<uint8_t> request = {0x22, 0x01};  

//     std::vector<uint8_t> expected_response = {0x03, 0x7F, 0x22, 0x13};
//     std::vector<uint8_t> actual_response = rdbi->readDataByIdentifier(can_id, request, false);

//     EXPECT_EQ(expected_response, actual_response);
// }

// TEST_F(ReadDataByIdentifierTest, DataIdentifierNotFound) {
//     canid_t can_id = 0x1000;
//     /* Data identifier 0x9999 not in MCU data */
//     std::vector<uint8_t> request = {0x22, 0x01, 0x99, 0x99};

//     std::vector<uint8_t> expected_response = {};
//     EXPECT_NO_THROW({
//         std::vector<uint8_t> actual_response = rdbi->readDataByIdentifier(can_id, request, false);
//         EXPECT_EQ(expected_response, actual_response);
//     });
// }

// TEST_F(ReadDataByIdentifierTest, CheckAPIFrameGeneration) {
//     canid_t can_id = 0xFA00;
//     std::vector<uint8_t> request = {0x22, 0x01, 0x12, 0x34};

//     EXPECT_NO_THROW({
//         rdbi->readDataByIdentifier(can_id, request, false);
//     });
// }

// TEST_F(ReadDataByIdentifierTest, CheckNonAPIFrameGeneration) {
//     canid_t can_id = 0x1000;
//     std::vector<uint8_t> request = {0x22, 0x01, 0x12, 0x34};

//     EXPECT_NO_THROW({
//         rdbi->readDataByIdentifier(can_id, request, false);
//     });
// }
// TEST_F(ReadDataByIdentifierTest, LongResponseForAPIFrame) {
//     canid_t can_id = 0xFA10;
//     std::vector<uint8_t> request = {0x22, 0x01, 0x12, 0x34};
//     /* 10 bytes of data */
//     std::vector<uint8_t> long_data(10, 0xBB);
//     MCU::mcu->mcu_data[0x1234] = long_data;

//     std::vector<uint8_t> actual_response = rdbi->readDataByIdentifier(can_id, request, true);

//     std::vector<uint8_t> expected_response;
//     expected_response.insert(expected_response.end(), long_data.begin(), long_data.end());

//     EXPECT_EQ(expected_response.size(), actual_response.size());

//     for (size_t i = 0; i < expected_response.size(); ++i) {
//         EXPECT_EQ(expected_response[i], actual_response[i]) << "Mismatch at index " << i;
//     }

//     EXPECT_GT(actual_response.size(), 8);
// }

// TEST_F(ReadDataByIdentifierTest, LongResponseForNonAPIFrame) {
//     canid_t can_id = 0x1010;
//     std::vector<uint8_t> request = {0x22, 0x01, 0x12, 0x34};
//     create_interface = nullptr;
//     /* Populate MCU data with a long response */
//     std::vector<uint8_t> long_data(10, 0xBB);
//     MCU::mcu->mcu_data[0x1234] = long_data;

//     std::vector<uint8_t> actual_response = rdbi->readDataByIdentifier(can_id, request, false);

//     std::vector<uint8_t> expected_response;
//     expected_response.insert(expected_response.end(), long_data.begin(), long_data.end());

//     EXPECT_EQ(expected_response.size(), actual_response.size());

//     for (size_t i = 0; i < expected_response.size(); ++i) {
//         EXPECT_EQ(expected_response[i], actual_response[i]) << "Mismatch at index " << i;
//     }

//     /* Assert that the size of the actual response is greater than 8 bytes */
//     EXPECT_GT(actual_response.size(), 8);
// }

int main(int argc, char* argv[])
{
    testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}