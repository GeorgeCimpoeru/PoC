#include "src/ecu_simulation/BatteryModule/include/ReceiveFrames.h"
#include "src/ecu_simulation/BatteryModule/include/HandleFrames.h"
#include "src/utils/include/GenerateFrames.h"
#include <gtest/gtest.h>
#include <unistd.h>
#include <linux/can.h>
#include <sys/socket.h>
#include <vector>
#include <sstream>
#include <net/if.h>
#include <cstring>
#include <string>
#include <sys/ioctl.h>
#include <thread>
#include <fcntl.h>
#include <errno.h>

canid_t MODULE_ID = 0x101;
int s1;
int s2;

class ReceiveFramesConstructorTest : public ::testing::Test {
protected:
    const int validSocket = 10;
    const int validModuleID = 0x101;
};

class ReceiveFramesConstructorDeathTest : public ::testing::Test {
protected:
    const int invalidSocket = -1;
    const int invalidModuleID = -1;
};

class MockReceiveFrames : public ReceiveFrames {
public:
    MockReceiveFrames(int socket) : ReceiveFrames(socket, MODULE_ID) {}
};

class ReceiveFramesTest : public ::testing::Test {
protected:
    MockReceiveFrames* receiveFrames;
    
    ReceiveFramesTest()
    {
        receiveFrames = new MockReceiveFrames(s1);
    }
    ~ReceiveFramesTest()
    {
        delete receiveFrames;
    }
};

struct can_frame createFrame(std::vector<uint8_t> test_data)
{
    struct can_frame resultFrame = {};

    resultFrame.can_id = MODULE_ID;
    uint8_t index = 0;
    for (auto data : test_data)
        resultFrame.data[index++] = data;
    resultFrame.can_dlc = test_data.size();
    return resultFrame;
}

void testFrames(struct can_frame sent_frame, struct can_frame received_frame )
{
    EXPECT_EQ(sent_frame.can_id, received_frame.can_id);
    EXPECT_EQ(sent_frame.can_dlc, received_frame.can_dlc);
    for (int i = 0; i < sent_frame.can_dlc; ++i) {
        EXPECT_EQ(sent_frame.data[i], received_frame.data[i]);
    }
}

bool containsLine(const std::string& output, const std::string& line)
{
    return output.find(line) != std::string::npos;
}

int createSocket()
{
    /*Create socket*/
    std::string nameInterface = "vcan0";
    struct sockaddr_can addr = {};
    struct ifreq ifr = {};
    int s;

    s = socket(PF_CAN, SOCK_RAW, CAN_RAW);
    if (s < 0)
    {
        std::cout<<"Error trying to create the socket\n";
        return 1;
    }
    /*Giving name and index to the interface created*/
    strcpy(ifr.ifr_name, nameInterface.c_str() );
    ioctl(s, SIOCGIFINDEX, &ifr);
    /*Set addr structure with info. of the CAN interface*/
    addr.can_family = AF_CAN;
    addr.can_ifindex = ifr.ifr_ifindex;
    /*Bind the socket to the CAN interface*/
    int b = bind(s, (struct sockaddr*)&addr, sizeof(addr));
    if (b < 0)
    {
        std::cout<<"Error binding\n";
        return 1;
    }
    int flags = fcntl(s, F_GETFL, 0);
    if (flags == -1)
    {
        std::cerr << "Error getting socket flags: " << strerror(errno) << std::endl;
        return 1;
    }
    // Set the O_NONBLOCK flag to make the socket non-blocking
    flags |= O_NONBLOCK;
    if (fcntl(s, F_SETFL, flags) == -1)
    {
        std::cerr << "Error setting flags: " << strerror(errno) << std::endl;
        return -1;
    }
    return s;
}
canid_t extractCANID(const std::string& input)
{
    std::istringstream iss(input);
    std::string line;
    
    while (std::getline(iss, line))
    {
        if (line.find("Module ID:") != std::string::npos)
        {
            size_t pos = line.find("0x");
            canid_t canID = std::stoul(line.substr(pos + 2), nullptr, 16);
            return canID;
        }
    }
    return 0;
}

uint8_t extractDataLength(const std::string& input)
{
    std::istringstream iss(input);
    std::string line;
    const std::string searchedStr = "Data Length:";
    while (std::getline(iss, line)) {
        if (line.find(searchedStr) != std::string::npos)
        {
            size_t pos = line.find("Data Length:");
            uint8_t dataLength = static_cast<uint8_t>(std::stoi(line.substr(pos + searchedStr.size())));
            return dataLength;
        }
    }
    return 0;
}

std::vector<uint8_t> extractData(const std::string& input) {
    std::vector<uint8_t> data;
    std::istringstream iss(input);
    std::string line;
    const std::string searchedStr = "Data:";
    
    while (std::getline(iss, line)) {
        if (line.find(searchedStr) != std::string::npos) {
            size_t pos = line.find(searchedStr);
            std::string dataStr = line.substr(pos + searchedStr.size());
            std::istringstream dataStream(dataStr);
            std::string byteStr;

            while (dataStream >> byteStr) {
                if (byteStr.substr(0, 2) == "0x") {
                    byteStr = byteStr.substr(2);
                }
                uint8_t byte = static_cast<uint8_t>(std::stoi(byteStr, nullptr, 16));
                data.push_back(byte);
            }
            break;
        }
    }
    return data;
}
struct can_frame createReceivedFrame(const std::string& output_receive)
{
    struct can_frame resultFrame = {};
    canid_t canID = extractCANID(output_receive);
    uint8_t dataLength = extractDataLength(output_receive);
    std::vector<uint8_t> data = extractData(output_receive);
    resultFrame.can_id = canID;
    uint8_t index = 0;
    for (index = 0; index < dataLength; index++)
        resultFrame.data[index] = data[index];
    resultFrame.can_dlc = dataLength;
    return resultFrame;
}

TEST_F(ReceiveFramesConstructorTest, ValidArguments)
{
    EXPECT_NO_THROW({
        ReceiveFrames receiver(validSocket, validModuleID);
    });
}

TEST_F(ReceiveFramesConstructorDeathTest, InvalidSocket)
{
    int invalidSocket = -1; // Invalid socket value
    
    EXPECT_DEATH({
        ReceiveFrames receiver(invalidSocket, invalidModuleID);
    }, "");
}

TEST_F(ReceiveFramesConstructorDeathTest, InvalidModuleID)
{
    int validSocket = 10; // Random valid socket
    
    EXPECT_DEATH({
        ReceiveFrames receiver(validSocket, invalidModuleID);
    }, "");
}
TEST_F(ReceiveFramesTest, EmptyFrameTest)
{
    HandleFrames handleFrames;
    std::string output_receive = "";
    std::thread receiverThread([&]() {
        testing::internal::CaptureStdout();
        receiveFrames->receive(handleFrames);
    }); 
    struct can_frame sent_frame = createFrame({});
    ASSERT_EQ(write(s2, &sent_frame, sizeof(sent_frame)), sizeof(sent_frame));
    
    std::this_thread::sleep_for(std::chrono::milliseconds(500));
    receiveFrames->stop();
    receiverThread.join();

    const std::string expected_out = 
        "Received empty frame\n";
    output_receive = testing::internal::GetCapturedStdout();
    EXPECT_TRUE(containsLine(output_receive, expected_out));
}
TEST_F(ReceiveFramesTest, ReceivedFrameNotForThisModuleTest)
{
    HandleFrames handleFrames;
    std::string output_receive = "";
    std::thread receiverThread([&]() {
        testing::internal::CaptureStdout();
        receiveFrames->receive(handleFrames);
    });
    struct can_frame sent_frame = createFrame({0x02,0x10,0x01});
    sent_frame.can_id = 0;
    ASSERT_EQ(write(s2, &sent_frame, sizeof(sent_frame)), sizeof(sent_frame));
    
    std::this_thread::sleep_for(std::chrono::milliseconds(500));
    receiveFrames->stop();
    receiverThread.join();

    const std::string expected_out = 
        "Received frame is not for this module\n";
    output_receive = testing::internal::GetCapturedStdout();
    EXPECT_TRUE(containsLine(output_receive, expected_out));
}

TEST_F(ReceiveFramesTest, CanIDRightBitsZero)
{
    HandleFrames handleFrames;
    ReceiveFrames *receiveFrames = new ReceiveFrames(s1,1);
    std::string output_receive = "";

    std::thread receiverThread([&]() {
        testing::internal::CaptureStdout();
        receiveFrames->receive(handleFrames);
    });
    struct can_frame sent_frame = createFrame({0x02,0x99,0x01});
    sent_frame.can_id = 0x1;
    ASSERT_EQ(write(s2, &sent_frame, sizeof(sent_frame)), sizeof(sent_frame));

    std::this_thread::sleep_for(std::chrono::milliseconds(500));
    receiveFrames->stop();
    receiverThread.join();

    output_receive = testing::internal::GetCapturedStdout();
    const std::string expected_out = 
        "Invalid CAN ID: upper 8 bits are zero\n";
    EXPECT_TRUE(containsLine(output_receive, expected_out));
    delete receiveFrames;
}

TEST_F(ReceiveFramesTest, SessionControlRequestTest)
{
    HandleFrames handleFrames;

    std::string output_receive = "";

    std::thread receiverThread([&]() {
        testing::internal::CaptureStdout();
        receiveFrames->receive(handleFrames);
    });
    struct can_frame sent_frame = createFrame({0x02,0x10,0x01});
    ASSERT_EQ(write(s2, &sent_frame, sizeof(sent_frame)), sizeof(sent_frame));

    std::this_thread::sleep_for(std::chrono::milliseconds(500));
    receiveFrames->stop();
    receiverThread.join();

    output_receive = testing::internal::GetCapturedStdout();
    struct can_frame received_frame = createReceivedFrame(output_receive);
    testFrames(sent_frame,received_frame);
}

TEST_F(ReceiveFramesTest, RequestUpdateStatusTest)
{
    HandleFrames handleFrames;

    std::string output_receive = "";

    std::thread receiverThread([&]() {
        testing::internal::CaptureStdout();
        receiveFrames->receive(handleFrames);
    });
    struct can_frame sent_frame = createFrame({0x02,0x32,0x01});
    ASSERT_EQ(write(s2, &sent_frame, sizeof(sent_frame)), sizeof(sent_frame));

    std::this_thread::sleep_for(std::chrono::milliseconds(500));
    receiveFrames->stop();
    receiverThread.join();

    output_receive = testing::internal::GetCapturedStdout();
    struct can_frame received_frame = createReceivedFrame(output_receive);
    testFrames(sent_frame,received_frame);
}

TEST_F(ReceiveFramesTest, SessionControlResponseTest)
{
    HandleFrames handleFrames;

    std::string output_receive = "";

    std::thread receiverThread([&]() {
        testing::internal::CaptureStdout();
        receiveFrames->receive(handleFrames);
    });
    struct can_frame sent_frame = createFrame({0x02,0x50,0x01});
    ASSERT_EQ(write(s2, &sent_frame, sizeof(sent_frame)), sizeof(sent_frame));

    std::this_thread::sleep_for(std::chrono::milliseconds(500));
    receiveFrames->stop();
    receiverThread.join();

    output_receive = testing::internal::GetCapturedStdout();
    struct can_frame received_frame = createReceivedFrame(output_receive);
    testFrames(sent_frame,received_frame);
}

TEST_F(ReceiveFramesTest, EcuResetRequestTest)
{
    HandleFrames handleFrames;

    std::string output_receive = "";

    std::thread receiverThread([&]() {
        testing::internal::CaptureStdout();
        receiveFrames->receive(handleFrames);
    });
    struct can_frame sent_frame = createFrame({0x02,0x11,0x03});
    ASSERT_EQ(write(s2, &sent_frame, sizeof(sent_frame)), sizeof(sent_frame));

    std::this_thread::sleep_for(std::chrono::milliseconds(500));
    receiveFrames->stop();
    receiverThread.join();

    output_receive = testing::internal::GetCapturedStdout();
    struct can_frame received_frame = createReceivedFrame(output_receive);
    testFrames(sent_frame,received_frame);
}

TEST_F(ReceiveFramesTest, EcuResetResponseTest)
{
    HandleFrames handleFrames;

    std::string output_receive = "";

    std::thread receiverThread([&]() {
        testing::internal::CaptureStdout();
        receiveFrames->receive(handleFrames);
    });
    struct can_frame sent_frame = createFrame({0x02,0x51,0x03});
    ASSERT_EQ(write(s2, &sent_frame, sizeof(sent_frame)), sizeof(sent_frame));

    std::this_thread::sleep_for(std::chrono::milliseconds(500));
    receiveFrames->stop();
    receiverThread.join();

    output_receive = testing::internal::GetCapturedStdout();
    struct can_frame received_frame = createReceivedFrame(output_receive);
    testFrames(sent_frame,received_frame);
}

TEST_F(ReceiveFramesTest, AuthRequestSeedResponseTest)
{
    HandleFrames handleFrames;

    std::string output_receive = "";

    std::thread receiverThread([&]() {
        testing::internal::CaptureStdout();
        receiveFrames->receive(handleFrames);
    });
    struct can_frame sent_frame = createFrame({0x05,0x69,0x1,0x23,0x34,0x35});
    ASSERT_EQ(write(s2, &sent_frame, sizeof(sent_frame)), sizeof(sent_frame));

    std::this_thread::sleep_for(std::chrono::milliseconds(500));
    receiveFrames->stop();
    receiverThread.join();

    output_receive = testing::internal::GetCapturedStdout();
    struct can_frame received_frame = createReceivedFrame(output_receive);
    testFrames(sent_frame,received_frame);
}

TEST_F(ReceiveFramesTest, AuthRequestSeedRequestTest)
{
    HandleFrames handleFrames;

    std::string output_receive = "";

    std::thread receiverThread([&]() {
        testing::internal::CaptureStdout();
        receiveFrames->receive(handleFrames);
    });
    struct can_frame sent_frame = createFrame({0x03,0x29,0x1});
    ASSERT_EQ(write(s2, &sent_frame, sizeof(sent_frame)), sizeof(sent_frame));

    std::this_thread::sleep_for(std::chrono::milliseconds(500));
    receiveFrames->stop();
    receiverThread.join();

    output_receive = testing::internal::GetCapturedStdout();
    struct can_frame received_frame = createReceivedFrame(output_receive);
    testFrames(sent_frame,received_frame);
}

TEST_F(ReceiveFramesTest, AuthSendKeyRequestTest)
{
    HandleFrames handleFrames;

    std::string output_receive = "";

    std::thread receiverThread([&]() {
        testing::internal::CaptureStdout();
        receiveFrames->receive(handleFrames);
    });
    struct can_frame sent_frame = createFrame({0x05,0x29,0x2,0x23,0x34,0x35});
    ASSERT_EQ(write(s2, &sent_frame, sizeof(sent_frame)), sizeof(sent_frame));

    std::this_thread::sleep_for(std::chrono::milliseconds(500));
    receiveFrames->stop();
    receiverThread.join();

    output_receive = testing::internal::GetCapturedStdout();
    struct can_frame received_frame = createReceivedFrame(output_receive);
    testFrames(sent_frame,received_frame);
}

TEST_F(ReceiveFramesTest, AuthSendKeyResponseTest)
{
    HandleFrames handleFrames;

    std::string output_receive = "";

    std::thread receiverThread([&]() {
        testing::internal::CaptureStdout();
        receiveFrames->receive(handleFrames);
    });
    struct can_frame sent_frame = createFrame({0x2,0x69,0x02});
    ASSERT_EQ(write(s2, &sent_frame, sizeof(sent_frame)), sizeof(sent_frame));

    std::this_thread::sleep_for(std::chrono::milliseconds(500));
    receiveFrames->stop();
    receiverThread.join();

    output_receive = testing::internal::GetCapturedStdout();
    struct can_frame received_frame = createReceivedFrame(output_receive);
    testFrames(sent_frame,received_frame);
}

TEST_F(ReceiveFramesTest, RoutineControlFrameRequestTest)
{
    HandleFrames handleFrames;

    std::string output_receive = "";

    std::thread receiverThread([&]() {
        testing::internal::CaptureStdout();
        receiveFrames->receive(handleFrames);
    });
    struct can_frame sent_frame = createFrame({0x04,0x31,0x02,0x34,0x1A});
    ASSERT_EQ(write(s2, &sent_frame, sizeof(sent_frame)), sizeof(sent_frame));

    std::this_thread::sleep_for(std::chrono::milliseconds(500));
    receiveFrames->stop();
    receiverThread.join();

    output_receive = testing::internal::GetCapturedStdout();
    struct can_frame received_frame = createReceivedFrame(output_receive);
    testFrames(sent_frame,received_frame);
}

TEST_F(ReceiveFramesTest, RoutineControlFrameResponseTest)
{
    HandleFrames handleFrames;

    std::string output_receive = "";

    std::thread receiverThread([&]() {
        testing::internal::CaptureStdout();
        receiveFrames->receive(handleFrames);
    });
    struct can_frame sent_frame = createFrame({0x04,0x71,0x02,0x34,0x1A});
    ASSERT_EQ(write(s2, &sent_frame, sizeof(sent_frame)), sizeof(sent_frame));

    std::this_thread::sleep_for(std::chrono::milliseconds(500));
    receiveFrames->stop();
    receiverThread.join();

    output_receive = testing::internal::GetCapturedStdout();
    struct can_frame received_frame = createReceivedFrame(output_receive);
    testFrames(sent_frame,received_frame);
}

TEST_F(ReceiveFramesTest, TesterPresentTest)
{
    HandleFrames handleFrames;

    std::string output_receive = "";

    std::thread receiverThread([&]() {
        testing::internal::CaptureStdout();
        receiveFrames->receive(handleFrames);
    });
    struct can_frame sent_frame = createFrame({0x02,0x3E,0x00});
    ASSERT_EQ(write(s2, &sent_frame, sizeof(sent_frame)), sizeof(sent_frame));

    std::this_thread::sleep_for(std::chrono::milliseconds(500));
    receiveFrames->stop();
    receiverThread.join();

    output_receive = testing::internal::GetCapturedStdout();
    struct can_frame received_frame = createReceivedFrame(output_receive);
    testFrames(sent_frame,received_frame);
}

TEST_F(ReceiveFramesTest, TesterPresentResponseTest)
{
    HandleFrames handleFrames;

    std::string output_receive = "";

    std::thread receiverThread([&]() {
        testing::internal::CaptureStdout();
        receiveFrames->receive(handleFrames);
    });
    struct can_frame sent_frame = createFrame({0x02,0x7E,0x00});
    ASSERT_EQ(write(s2, &sent_frame, sizeof(sent_frame)), sizeof(sent_frame));

    std::this_thread::sleep_for(std::chrono::milliseconds(500));
    receiveFrames->stop();
    receiverThread.join();

    output_receive = testing::internal::GetCapturedStdout();
    struct can_frame received_frame = createReceivedFrame(output_receive);
    testFrames(sent_frame,received_frame);
}

TEST_F(ReceiveFramesTest, ReadByIdentResponseTest)
{
    HandleFrames handleFrames;

    std::string output_receive = "";

    std::thread receiverThread([&]() {
        testing::internal::CaptureStdout();
        receiveFrames->receive(handleFrames);
    });
    struct can_frame sent_frame = createFrame({0x05,0x62,0x33,0x22,0x32,0x11});
    ASSERT_EQ(write(s2, &sent_frame, sizeof(sent_frame)), sizeof(sent_frame));

    std::this_thread::sleep_for(std::chrono::milliseconds(500));
    receiveFrames->stop();
    receiverThread.join();

    output_receive = testing::internal::GetCapturedStdout();
    struct can_frame received_frame = createReceivedFrame(output_receive);
    testFrames(sent_frame,received_frame);
}

TEST_F(ReceiveFramesTest, ReadByIdentRequestTest)
{
    HandleFrames handleFrames;

    std::string output_receive = "";

    std::thread receiverThread([&]() {
        testing::internal::CaptureStdout();
        receiveFrames->receive(handleFrames);
    });
    struct can_frame sent_frame = createFrame({0x03,0x22,0x33,0x22});
    ASSERT_EQ(write(s2, &sent_frame, sizeof(sent_frame)), sizeof(sent_frame));

    std::this_thread::sleep_for(std::chrono::milliseconds(500));
    receiveFrames->stop();
    receiverThread.join();
 
    output_receive = testing::internal::GetCapturedStdout();
    struct can_frame received_frame = createReceivedFrame(output_receive);
    testFrames(sent_frame,received_frame);
}

TEST_F(ReceiveFramesTest, ReadByIdentLongRespTest)
{
    HandleFrames handleFrames;

    std::string output_receive = "";

    std::thread receiverThread([&]() {
        testing::internal::CaptureStdout();
        receiveFrames->receive(handleFrames);
    });
    GenerateFrames g = GenerateFrames(s2);
    std::vector<uint8_t> response = {1,2,3,4,5,6,7,8,9};
    g.readDataByIdentifierLongResponse(0x101,0x1234,response);
    g.readDataByIdentifierLongResponse(0x101,0x1234,response,false);
    std::this_thread::sleep_for(std::chrono::milliseconds(500));
    receiveFrames->stop();
    receiverThread.join();

    output_receive = testing::internal::GetCapturedStdout();
    std::string searchLine = "All frames received";
    EXPECT_TRUE(containsLine(output_receive, searchLine));
    searchLine = "Response 0x62 for ReadDataByIdentifierLong";
    EXPECT_TRUE(containsLine(output_receive, searchLine));
}

TEST_F(ReceiveFramesTest, ReadMemByAddressResponseTest)
{
    HandleFrames handleFrames;

    std::string output_receive = "";

    std::thread receiverThread([&]() {
        testing::internal::CaptureStdout();
        receiveFrames->receive(handleFrames);
    });
    struct can_frame sent_frame = createFrame({0x07,0x63,0x21,0x01,0x23,0x45,1,2});
    ASSERT_EQ(write(s2, &sent_frame, sizeof(sent_frame)), sizeof(sent_frame));

    std::this_thread::sleep_for(std::chrono::milliseconds(500));
    receiveFrames->stop();
    receiverThread.join();
 
    output_receive = testing::internal::GetCapturedStdout();
    struct can_frame received_frame = createReceivedFrame(output_receive);
    testFrames(sent_frame,received_frame);
}

TEST_F(ReceiveFramesTest, ReadMemByAddressRequestTest)
{
    HandleFrames handleFrames;

    std::string output_receive = "";

    std::thread receiverThread([&]() {
        testing::internal::CaptureStdout();
        receiveFrames->receive(handleFrames);
    });
    struct can_frame sent_frame = createFrame({0x05,0x23,0x21,0x01,0x23,0x45});
    ASSERT_EQ(write(s2, &sent_frame, sizeof(sent_frame)), sizeof(sent_frame));

    std::this_thread::sleep_for(std::chrono::milliseconds(500));
    receiveFrames->stop();
    receiverThread.join();
 
    output_receive = testing::internal::GetCapturedStdout();
    struct can_frame received_frame = createReceivedFrame(output_receive);
    testFrames(sent_frame,received_frame);
}

TEST_F(ReceiveFramesTest, ReadMemoryByAddressLongTest)
{
    HandleFrames handleFrames;

    std::string output_receive = "";

    std::thread receiverThread([&]() {
        testing::internal::CaptureStdout();
        receiveFrames->receive(handleFrames);
    });
    GenerateFrames g = GenerateFrames(s2);
    g.readMemoryByAddressLongResponse(0x101,0x2345,0x01,{1,2,3,4,5,6});
    g.readMemoryByAddressLongResponse(0x101,0x2345,0x01,{1,2,3,4,5,6},false);
    std::this_thread::sleep_for(std::chrono::milliseconds(500));
    receiveFrames->stop();
    receiverThread.join();

    output_receive = testing::internal::GetCapturedStdout();
    std::string searchLine = "All frames received";
    EXPECT_TRUE(containsLine(output_receive, searchLine));
    searchLine = "Reponse 0x63 for ReadMemoryByAddressLong";
    EXPECT_TRUE(containsLine(output_receive, searchLine));
}

TEST_F(ReceiveFramesTest, WriteDataByIdentifierResponseTest)
{
    HandleFrames handleFrames;

    std::string output_receive = "";

    std::thread receiverThread([&]() {
        testing::internal::CaptureStdout();
        receiveFrames->receive(handleFrames);
    });
    struct can_frame sent_frame = createFrame({0x03,0x6E,0x23,0x45});
    ASSERT_EQ(write(s2, &sent_frame, sizeof(sent_frame)), sizeof(sent_frame));

    std::this_thread::sleep_for(std::chrono::milliseconds(500));
    receiveFrames->stop();
    receiverThread.join();
 
    output_receive = testing::internal::GetCapturedStdout();
    struct can_frame received_frame = createReceivedFrame(output_receive);
    testFrames(sent_frame,received_frame);
}

TEST_F(ReceiveFramesTest, WriteDataByIdentifierRequestTest)
{
    HandleFrames handleFrames;

    std::string output_receive = "";

    std::thread receiverThread([&]() {
        testing::internal::CaptureStdout();
        receiveFrames->receive(handleFrames);
    });
    struct can_frame sent_frame = createFrame({0x05,0x2E,0x23,0x45,0x1,0x2});
    ASSERT_EQ(write(s2, &sent_frame, sizeof(sent_frame)), sizeof(sent_frame));

    std::this_thread::sleep_for(std::chrono::milliseconds(500));
    receiveFrames->stop();
    receiverThread.join();
 
    output_receive = testing::internal::GetCapturedStdout();
    struct can_frame received_frame = createReceivedFrame(output_receive);
    testFrames(sent_frame,received_frame);
}

TEST_F(ReceiveFramesTest, WriteDataByIdentifierLongRequestTest)
{
    HandleFrames handleFrames;

    std::string output_receive = "";

    std::thread receiverThread([&]() {
        testing::internal::CaptureStdout();
        receiveFrames->receive(handleFrames);
    });
    GenerateFrames g = GenerateFrames(s2);
    g.writeDataByIdentifierLongData(0x101,0x2345,{1,2,3,4,5});
    g.writeDataByIdentifierLongData(0x101,0x2345,{1,2,3,4,5},false);
    std::this_thread::sleep_for(std::chrono::milliseconds(500));
    receiveFrames->stop();
    receiverThread.join();

    output_receive = testing::internal::GetCapturedStdout();
    std::string searchLine = "All frames received";
    EXPECT_TRUE(containsLine(output_receive, searchLine));
    searchLine = "Service 0x2E WriteDataByIdentifierLongRequest";
    EXPECT_TRUE(containsLine(output_receive, searchLine));
}

TEST_F(ReceiveFramesTest, FlowControlTest)
{
    HandleFrames handleFrames;

    std::string output_receive = "";

    std::thread receiverThread([&]() {
        testing::internal::CaptureStdout();
        receiveFrames->receive(handleFrames);
    });
    struct can_frame sent_frame = createFrame({0x30,0x0,0x0,0x0});
    ASSERT_EQ(write(s2, &sent_frame, sizeof(sent_frame)), sizeof(sent_frame));

    std::this_thread::sleep_for(std::chrono::milliseconds(500));
    receiveFrames->stop();
    receiverThread.join();
 
    output_receive = testing::internal::GetCapturedStdout();
    struct can_frame received_frame = createReceivedFrame(output_receive);
    testFrames(sent_frame,received_frame);
}

TEST_F(ReceiveFramesTest, ReadDTCRequestTest)
{
    HandleFrames handleFrames;

    std::string output_receive = "";

    std::thread receiverThread([&]() {
        testing::internal::CaptureStdout();
        receiveFrames->receive(handleFrames);
    });
    struct can_frame sent_frame = createFrame({0x3,0x19,0x01,0x12});
    ASSERT_EQ(write(s2, &sent_frame, sizeof(sent_frame)), sizeof(sent_frame));

    std::this_thread::sleep_for(std::chrono::milliseconds(500));
    receiveFrames->stop();
    receiverThread.join();
 
    output_receive = testing::internal::GetCapturedStdout();
    struct can_frame received_frame = createReceivedFrame(output_receive);
    testFrames(sent_frame,received_frame);
}

TEST_F(ReceiveFramesTest, ReadDTCResponseTest)
{
    HandleFrames handleFrames;

    std::string output_receive = "";

    std::thread receiverThread([&]() {
        testing::internal::CaptureStdout();
        receiveFrames->receive(handleFrames);
    });
    struct can_frame sent_frame = createFrame({0x3,0x59,0x01,0x2,0x3,0x4});
    ASSERT_EQ(write(s2, &sent_frame, sizeof(sent_frame)), sizeof(sent_frame));

    std::this_thread::sleep_for(std::chrono::milliseconds(500));
    receiveFrames->stop();
    receiverThread.join();
 
    output_receive = testing::internal::GetCapturedStdout();
    struct can_frame received_frame = createReceivedFrame(output_receive);
    testFrames(sent_frame,received_frame);
}

TEST_F(ReceiveFramesTest, ClearDTCRequestTest)
{
    HandleFrames handleFrames;

    std::string output_receive = "";

    std::thread receiverThread([&]() {
        testing::internal::CaptureStdout();
        receiveFrames->receive(handleFrames);
    });
    struct can_frame sent_frame = createFrame({0x04,0x14,0xFF,0xFF,0xFF});
    ASSERT_EQ(write(s2, &sent_frame, sizeof(sent_frame)), sizeof(sent_frame));

    std::this_thread::sleep_for(std::chrono::milliseconds(500));
    receiveFrames->stop();
    receiverThread.join();
 
    output_receive = testing::internal::GetCapturedStdout();
    struct can_frame received_frame = createReceivedFrame(output_receive);
    testFrames(sent_frame,received_frame);
}

TEST_F(ReceiveFramesTest, ClearDTCResponseTest)
{
    HandleFrames handleFrames;

    std::string output_receive = "";

    std::thread receiverThread([&]() {
        testing::internal::CaptureStdout();
        receiveFrames->receive(handleFrames);
    });
    struct can_frame sent_frame = createFrame({0x01,0x54});
    ASSERT_EQ(write(s2, &sent_frame, sizeof(sent_frame)), sizeof(sent_frame));

    std::this_thread::sleep_for(std::chrono::milliseconds(500));
    receiveFrames->stop();
    receiverThread.join();
 
    output_receive = testing::internal::GetCapturedStdout();
    struct can_frame received_frame = createReceivedFrame(output_receive);
    testFrames(sent_frame,received_frame);
}

TEST_F(ReceiveFramesTest, AccessTimeParamRequestTest)
{
    HandleFrames handleFrames;

    std::string output_receive = "";

    std::thread receiverThread([&]() {
        testing::internal::CaptureStdout();
        receiveFrames->receive(handleFrames);
    });
    struct can_frame sent_frame = createFrame({0x02,0x83,0x1});
    ASSERT_EQ(write(s2, &sent_frame, sizeof(sent_frame)), sizeof(sent_frame));

    std::this_thread::sleep_for(std::chrono::milliseconds(500));
    receiveFrames->stop();
    receiverThread.join();
 
    output_receive = testing::internal::GetCapturedStdout();
    struct can_frame received_frame = createReceivedFrame(output_receive);
    testFrames(sent_frame,received_frame);
}

TEST_F(ReceiveFramesTest, AccessTimeParamResponseTest)
{
    HandleFrames handleFrames;

    std::string output_receive = "";

    std::thread receiverThread([&]() {
        testing::internal::CaptureStdout();
        receiveFrames->receive(handleFrames);
    });
    struct can_frame sent_frame = createFrame({0x02,0xC3,0x1});
    ASSERT_EQ(write(s2, &sent_frame, sizeof(sent_frame)), sizeof(sent_frame));

    std::this_thread::sleep_for(std::chrono::milliseconds(500));
    receiveFrames->stop();
    receiverThread.join();
 
    output_receive = testing::internal::GetCapturedStdout();
    struct can_frame received_frame = createReceivedFrame(output_receive);
    testFrames(sent_frame,received_frame);
}

TEST_F(ReceiveFramesTest, NegativeResponseUnknownResponseTest)
{
    HandleFrames handleFrames;

    std::string output_receive = "";

    std::thread receiverThread([&]() {
        testing::internal::CaptureStdout();
        receiveFrames->receive(handleFrames);
    });
    struct can_frame sent_frame = createFrame({0x03,0x7F,0x10,0x12});
    ASSERT_EQ(write(s2, &sent_frame, sizeof(sent_frame)), sizeof(sent_frame));

    std::this_thread::sleep_for(std::chrono::milliseconds(500));
    receiveFrames->stop();
    receiverThread.join();
 
    output_receive = testing::internal::GetCapturedStdout();
    struct can_frame received_frame = createReceivedFrame(output_receive);
    testFrames(sent_frame,received_frame);
}

TEST_F(ReceiveFramesTest, RequestDownloadRequestTest)
{
    HandleFrames handleFrames;

    std::string output_receive = "";

    std::thread receiverThread([&]() {
        testing::internal::CaptureStdout();
        receiveFrames->receive(handleFrames);
    });
    struct can_frame sent_frame = createFrame({0x06,0x34, 0x00, 0x12, 0x34,0x45, 0x10});
    ASSERT_EQ(write(s2, &sent_frame, sizeof(sent_frame)), sizeof(sent_frame));

    std::this_thread::sleep_for(std::chrono::milliseconds(500));
    receiveFrames->stop();
    receiverThread.join();
 
    output_receive = testing::internal::GetCapturedStdout();
    struct can_frame received_frame = createReceivedFrame(output_receive);
    testFrames(sent_frame,received_frame);
}

TEST_F(ReceiveFramesTest, RequestDownloadResponseTest)
{
    HandleFrames handleFrames;

    std::string output_receive = "";

    std::thread receiverThread([&]() {
        testing::internal::CaptureStdout();
        receiveFrames->receive(handleFrames);
    });
    struct can_frame sent_frame = createFrame({0x04,0x74, 0x20,0x22, 0x34});
    ASSERT_EQ(write(s2, &sent_frame, sizeof(sent_frame)), sizeof(sent_frame));

    std::this_thread::sleep_for(std::chrono::milliseconds(500));
    receiveFrames->stop();
    receiverThread.join();
 
    output_receive = testing::internal::GetCapturedStdout();
    struct can_frame received_frame = createReceivedFrame(output_receive);
    testFrames(sent_frame,received_frame);
}

TEST_F(ReceiveFramesTest, TransferDataRequestTest)
{
    HandleFrames handleFrames;

    std::string output_receive = "";

    std::thread receiverThread([&]() {
        testing::internal::CaptureStdout();
        receiveFrames->receive(handleFrames);
    });
    struct can_frame sent_frame = createFrame({0x07,0x36, 0x20, 1,2,3,4,5});
    ASSERT_EQ(write(s2, &sent_frame, sizeof(sent_frame)), sizeof(sent_frame));

    std::this_thread::sleep_for(std::chrono::milliseconds(500));
    receiveFrames->stop();
    receiverThread.join();
 
    output_receive = testing::internal::GetCapturedStdout();
    struct can_frame received_frame = createReceivedFrame(output_receive);
    testFrames(sent_frame,received_frame);
}

TEST_F(ReceiveFramesTest, TransferDataResponseTest)
{
    HandleFrames handleFrames;

    std::string output_receive = "";

    std::thread receiverThread([&]() {
        testing::internal::CaptureStdout();
        receiveFrames->receive(handleFrames);
    });
    struct can_frame sent_frame = createFrame({0x02,0x76, 0x20});
    ASSERT_EQ(write(s2, &sent_frame, sizeof(sent_frame)), sizeof(sent_frame));

    std::this_thread::sleep_for(std::chrono::milliseconds(500));
    receiveFrames->stop();
    receiverThread.join();
 
    output_receive = testing::internal::GetCapturedStdout();
    struct can_frame received_frame = createReceivedFrame(output_receive);
    testFrames(sent_frame,received_frame);
}

TEST_F(ReceiveFramesTest, TransferDataLongRequestTest)
{
    HandleFrames handleFrames;

    std::string output_receive = "";

    std::thread receiverThread([&]() {
        testing::internal::CaptureStdout();
        receiveFrames->receive(handleFrames);
    });
    GenerateFrames g = GenerateFrames(s2);
    std::vector<uint8_t> data = {1,2,3,4,5,6,7};
    g.transferDataLong(0x101,0x20,data);
    g.transferDataLong(0x101,0x20,data,false);
    std::this_thread::sleep_for(std::chrono::milliseconds(500));
    receiveFrames->stop();
    receiverThread.join();

    output_receive = testing::internal::GetCapturedStdout();
    std::string searchLine = "All frames received";
    EXPECT_TRUE(containsLine(output_receive, searchLine));
    searchLine = "Service 0x36 TransferDataLongRequest";
    EXPECT_TRUE(containsLine(output_receive, searchLine));
}

TEST_F(ReceiveFramesTest, TransferDataRequest1Test)
{
    HandleFrames handleFrames;

    std::string output_receive = "";

    std::thread receiverThread([&]() {
        testing::internal::CaptureStdout();
        receiveFrames->receive(handleFrames);
    });
    struct can_frame sent_frame = createFrame({0x05,0x36,0x20,1,2,3});
    ASSERT_EQ(write(s2, &sent_frame, sizeof(sent_frame)), sizeof(sent_frame));

    std::this_thread::sleep_for(std::chrono::milliseconds(500));
    receiveFrames->stop();
    receiverThread.join();
 
    output_receive = testing::internal::GetCapturedStdout();
    struct can_frame received_frame = createReceivedFrame(output_receive);
    testFrames(sent_frame,received_frame);
}

TEST_F(ReceiveFramesTest, ReqTransferExitRequestTest)
{
    HandleFrames handleFrames;

    std::string output_receive = "";

    std::thread receiverThread([&]() {
        testing::internal::CaptureStdout();
        receiveFrames->receive(handleFrames);
    });
    struct can_frame sent_frame = createFrame({0x01,0x37});
    ASSERT_EQ(write(s2, &sent_frame, sizeof(sent_frame)), sizeof(sent_frame));

    std::this_thread::sleep_for(std::chrono::milliseconds(500));
    receiveFrames->stop();
    receiverThread.join();
 
    output_receive = testing::internal::GetCapturedStdout();
    struct can_frame received_frame = createReceivedFrame(output_receive);
    testFrames(sent_frame,received_frame);
}

TEST_F(ReceiveFramesTest, ReqTransferExitResponseTest)
{
    HandleFrames handleFrames;

    std::string output_receive = "";

    std::thread receiverThread([&]() {
        testing::internal::CaptureStdout();
        receiveFrames->receive(handleFrames);
    });
    struct can_frame sent_frame = createFrame({0x01,0x77});
    ASSERT_EQ(write(s2, &sent_frame, sizeof(sent_frame)), sizeof(sent_frame));

    std::this_thread::sleep_for(std::chrono::milliseconds(500));
    receiveFrames->stop();
    receiverThread.join();
 
    output_receive = testing::internal::GetCapturedStdout();
    struct can_frame received_frame = createReceivedFrame(output_receive);
    testFrames(sent_frame,received_frame);
}

TEST_F(ReceiveFramesTest, UnknownServiceTest)
{
    HandleFrames handleFrames;

    std::string output_receive = "";

    std::thread receiverThread([&]() {
        testing::internal::CaptureStdout();
        receiveFrames->receive(handleFrames);
    });
    struct can_frame sent_frame = createFrame({0x02,0x99,0x01});
    ASSERT_EQ(write(s2, &sent_frame, sizeof(sent_frame)), sizeof(sent_frame));

    std::this_thread::sleep_for(std::chrono::milliseconds(500));
    receiveFrames->stop();
    receiverThread.join();

    output_receive = testing::internal::GetCapturedStdout();
    const std::string expected_out = 
        "Unknown service\n";
    EXPECT_TRUE(containsLine(output_receive, expected_out));
}

TEST_F(ReceiveFramesTest, MCUNotifyUp)
{
    HandleFrames handleFrames;
    ReceiveFrames *receiveFrames = new ReceiveFrames(s1,0x12211);
    std::string output_receive = "";

    std::thread receiverThread([&]() {
        testing::internal::CaptureStdout();
        receiveFrames->receive(handleFrames);
    });
    struct can_frame sent_frame = createFrame({0x01,0x01,0x01});
    sent_frame.can_id = 0x12211;
    ASSERT_EQ(write(s2, &sent_frame, sizeof(sent_frame)), sizeof(sent_frame));

    std::this_thread::sleep_for(std::chrono::milliseconds(500));
    receiveFrames->stop();
    receiverThread.join();

    output_receive = testing::internal::GetCapturedStdout();
    const std::string expected_out = 
        "Response sent to MCU\n";
    EXPECT_TRUE(containsLine(output_receive, expected_out));
    delete receiveFrames;
}

TEST_F(ReceiveFramesTest, MCUNotifyUpAA)
{
    HandleFrames handleFrames;
    ReceiveFrames *receiveFrames = new ReceiveFrames(s1,0x12211);
    std::string output_receive = "";

    std::thread receiverThread([&]() {
        testing::internal::CaptureStdout();
        receiveFrames->receive(handleFrames);
    });
    struct can_frame sent_frame = createFrame({0x02,0xAA,0x01});
    sent_frame.can_id = 0x12211;
    ASSERT_EQ(write(s2, &sent_frame, sizeof(sent_frame)), sizeof(sent_frame));

    std::this_thread::sleep_for(std::chrono::milliseconds(500));
    receiveFrames->stop();
    receiverThread.join();

    output_receive = testing::internal::GetCapturedStdout();
    struct can_frame received_frame = createReceivedFrame(output_receive);
    testFrames(sent_frame,received_frame);
    delete receiveFrames;
}

class HandleFramesTest : public ::testing::Test {
protected:
    HandleFrames handleFrames;
};

TEST_F(HandleFramesTest, CheckReceivedFrame)
{
    HandleFrames handleFrames;
    std::string output = "";
    struct can_frame sent_frame = createFrame({0x02,0x51,0x03});
    errno = EWOULDBLOCK;
    EXPECT_FALSE(handleFrames.checkReceivedFrame(-1,sent_frame));
    
    errno = ENOENT;
    testing::internal::CaptureStdout();
    EXPECT_FALSE(handleFrames.checkReceivedFrame(-1,sent_frame));
    output = testing::internal::GetCapturedStdout();
    std::string expected_out = "Read error:";
    EXPECT_TRUE(containsLine(output, expected_out));
    
    errno = 0;
    testing::internal::CaptureStdout();
    sent_frame = createFrame({0x02,0x51,0x03});
    handleFrames.checkReceivedFrame(1,sent_frame);
    output = testing::internal::GetCapturedStdout();
    expected_out = "Incomplete frame read\n";
    EXPECT_TRUE(containsLine(output, expected_out));  
}

TEST_F(ReceiveFramesTest, SessionControlRequestNegative)
{
    HandleFrames handleFrames;

    std::string output_receive = "";

    std::thread receiverThread([&]() {
        testing::internal::CaptureStdout();
        receiveFrames->receive(handleFrames);
    });
    struct can_frame sent_frame = createFrame({0x03, 0X7F, 0x10, 0x11});
    ASSERT_EQ(write(s2, &sent_frame, sizeof(sent_frame)), sizeof(sent_frame));
    
    sent_frame = createFrame({0x03, 0X7F, 0x10, 0x13});
    ASSERT_EQ(write(s2, &sent_frame, sizeof(sent_frame)), sizeof(sent_frame));
    sent_frame = createFrame({0x03, 0X7F, 0x10, 0x14});
    ASSERT_EQ(write(s2, &sent_frame, sizeof(sent_frame)), sizeof(sent_frame));
    sent_frame = createFrame({0x03, 0X7F, 0x10, 0x25});
    ASSERT_EQ(write(s2, &sent_frame, sizeof(sent_frame)), sizeof(sent_frame));
    sent_frame = createFrame({0x03, 0X7F, 0x10, 0x34});
    ASSERT_EQ(write(s2, &sent_frame, sizeof(sent_frame)), sizeof(sent_frame));
    sent_frame = createFrame({0x03, 0X7F, 0x10, 0x94});
    ASSERT_EQ(write(s2, &sent_frame, sizeof(sent_frame)), sizeof(sent_frame));
    sent_frame = createFrame({0x03, 0X7F, 0x10, 0x70});
    ASSERT_EQ(write(s2, &sent_frame, sizeof(sent_frame)), sizeof(sent_frame));
    sent_frame = createFrame({0x03, 0X7F, 0x10, 0x71});
    ASSERT_EQ(write(s2, &sent_frame, sizeof(sent_frame)), sizeof(sent_frame));
    sent_frame = createFrame({0x03, 0X7F, 0x10, 0x72});
    ASSERT_EQ(write(s2, &sent_frame, sizeof(sent_frame)), sizeof(sent_frame));

    std::this_thread::sleep_for(std::chrono::milliseconds(500));
    receiveFrames->stop();
    receiverThread.join();
    output_receive = testing::internal::GetCapturedStdout();

}

TEST_F(ReceiveFramesTest, EcuResetNegative)
{
    HandleFrames handleFrames;

    std::string output_receive = "";

    std::thread receiverThread([&]() {
        testing::internal::CaptureStdout();
        receiveFrames->receive(handleFrames);
    });
    struct can_frame sent_frame = createFrame({0x03, 0X7F, 0x11, 0x25});
    ASSERT_EQ(write(s2, &sent_frame, sizeof(sent_frame)), sizeof(sent_frame));

    std::this_thread::sleep_for(std::chrono::milliseconds(500));
    receiveFrames->stop();
    receiverThread.join();
    output_receive = testing::internal::GetCapturedStdout();

}

TEST_F(ReceiveFramesTest, ReadDataByIdentNegative)
{
    HandleFrames handleFrames;

    std::string output_receive = "";

    std::thread receiverThread([&]() {
        testing::internal::CaptureStdout();
        receiveFrames->receive(handleFrames);
    });
    struct can_frame sent_frame = createFrame({0x03, 0X7F, 0x22, 0x11});
    ASSERT_EQ(write(s2, &sent_frame, sizeof(sent_frame)), sizeof(sent_frame));

    std::this_thread::sleep_for(std::chrono::milliseconds(500));
    receiveFrames->stop();
    receiverThread.join();
    output_receive = testing::internal::GetCapturedStdout();

}

TEST_F(ReceiveFramesTest, AuthRequestSeeNegative)
{
    HandleFrames handleFrames;

    std::string output_receive = "";

    std::thread receiverThread([&]() {
        testing::internal::CaptureStdout();
        receiveFrames->receive(handleFrames);
    });
    struct can_frame sent_frame = createFrame({0x03, 0X7F, 0x29, 0x11});
    ASSERT_EQ(write(s2, &sent_frame, sizeof(sent_frame)), sizeof(sent_frame));

    std::this_thread::sleep_for(std::chrono::milliseconds(500));
    receiveFrames->stop();
    receiverThread.join();
    output_receive = testing::internal::GetCapturedStdout();

}

TEST_F(ReceiveFramesTest, RoutineControlNegative)
{
    HandleFrames handleFrames;

    std::string output_receive = "";

    std::thread receiverThread([&]() {
        testing::internal::CaptureStdout();
        receiveFrames->receive(handleFrames);
    });
    struct can_frame sent_frame = createFrame({0x03, 0X7F, 0x31, 0x11});
    ASSERT_EQ(write(s2, &sent_frame, sizeof(sent_frame)), sizeof(sent_frame));

    std::this_thread::sleep_for(std::chrono::milliseconds(500));
    receiveFrames->stop();
    receiverThread.join();
    output_receive = testing::internal::GetCapturedStdout();

}

TEST_F(ReceiveFramesTest, TesterPresentNegative)
{
    HandleFrames handleFrames;

    std::string output_receive = "";

    std::thread receiverThread([&]() {
        testing::internal::CaptureStdout();
        receiveFrames->receive(handleFrames);
    });
    struct can_frame sent_frame = createFrame({0x03, 0X7F, 0x3E, 0x11});
    ASSERT_EQ(write(s2, &sent_frame, sizeof(sent_frame)), sizeof(sent_frame));

    std::this_thread::sleep_for(std::chrono::milliseconds(500));
    receiveFrames->stop();
    receiverThread.join();
    output_receive = testing::internal::GetCapturedStdout();

}

TEST_F(ReceiveFramesTest, ReadMemByAddressNegative)
{
    HandleFrames handleFrames;

    std::string output_receive = "";

    std::thread receiverThread([&]() {
        testing::internal::CaptureStdout();
        receiveFrames->receive(handleFrames);
    });
    struct can_frame sent_frame = createFrame({0x03, 0X7F, 0x23, 0x11});
    ASSERT_EQ(write(s2, &sent_frame, sizeof(sent_frame)), sizeof(sent_frame));

    std::this_thread::sleep_for(std::chrono::milliseconds(500));
    receiveFrames->stop();
    receiverThread.join();
    output_receive = testing::internal::GetCapturedStdout();

}

TEST_F(ReceiveFramesTest, WriteDataByIdentifierNegative)
{
    HandleFrames handleFrames;

    std::string output_receive = "";

    std::thread receiverThread([&]() {
        testing::internal::CaptureStdout();
        receiveFrames->receive(handleFrames);
    });
    struct can_frame sent_frame = createFrame({0x03, 0X7F, 0x2E, 0x11});
    ASSERT_EQ(write(s2, &sent_frame, sizeof(sent_frame)), sizeof(sent_frame));

    std::this_thread::sleep_for(std::chrono::milliseconds(500));
    receiveFrames->stop();
    receiverThread.join();
    output_receive = testing::internal::GetCapturedStdout();

}

TEST_F(ReceiveFramesTest, ReadDtcInformationNegative)
{
    HandleFrames handleFrames;

    std::string output_receive = "";

    std::thread receiverThread([&]() {
        testing::internal::CaptureStdout();
        receiveFrames->receive(handleFrames);
    });
    struct can_frame sent_frame = createFrame({0x03, 0X7F, 0x19, 0x11});
    ASSERT_EQ(write(s2, &sent_frame, sizeof(sent_frame)), sizeof(sent_frame));

    std::this_thread::sleep_for(std::chrono::milliseconds(500));
    receiveFrames->stop();
    receiverThread.join();
    output_receive = testing::internal::GetCapturedStdout();

}

TEST_F(ReceiveFramesTest, ClearDiagnosticInformationNegative)
{
    HandleFrames handleFrames;

    std::string output_receive = "";

    std::thread receiverThread([&]() {
        testing::internal::CaptureStdout();
        receiveFrames->receive(handleFrames);
    });
    struct can_frame sent_frame = createFrame({0x03, 0X7F, 0x14, 0x11});
    ASSERT_EQ(write(s2, &sent_frame, sizeof(sent_frame)), sizeof(sent_frame));

    std::this_thread::sleep_for(std::chrono::milliseconds(500));
    receiveFrames->stop();
    receiverThread.join();
    output_receive = testing::internal::GetCapturedStdout();

}

TEST_F(ReceiveFramesTest, AccessTimingParametersNegative)
{
    HandleFrames handleFrames;

    std::string output_receive = "";

    std::thread receiverThread([&]() {
        testing::internal::CaptureStdout();
        receiveFrames->receive(handleFrames);
    });
    struct can_frame sent_frame = createFrame({0x03, 0X7F, 0x83, 0x11});
    ASSERT_EQ(write(s2, &sent_frame, sizeof(sent_frame)), sizeof(sent_frame));

    std::this_thread::sleep_for(std::chrono::milliseconds(500));
    receiveFrames->stop();
    receiverThread.join();
    output_receive = testing::internal::GetCapturedStdout();

}

TEST_F(ReceiveFramesTest, RequestDownloadNegative)
{
    HandleFrames handleFrames;

    std::string output_receive = "";

    std::thread receiverThread([&]() {
        testing::internal::CaptureStdout();
        receiveFrames->receive(handleFrames);
    });
    struct can_frame sent_frame = createFrame({0x03, 0X7F, 0x34, 0x11});
    ASSERT_EQ(write(s2, &sent_frame, sizeof(sent_frame)), sizeof(sent_frame));

    std::this_thread::sleep_for(std::chrono::milliseconds(500));
    receiveFrames->stop();
    receiverThread.join();
    output_receive = testing::internal::GetCapturedStdout();

}

TEST_F(ReceiveFramesTest, TransferDataNegative)
{
    HandleFrames handleFrames;

    std::string output_receive = "";

    std::thread receiverThread([&]() {
        testing::internal::CaptureStdout();
        receiveFrames->receive(handleFrames);
    });
    struct can_frame sent_frame = createFrame({0x03, 0X7F, 0x36, 0x11});
    ASSERT_EQ(write(s2, &sent_frame, sizeof(sent_frame)), sizeof(sent_frame));

    std::this_thread::sleep_for(std::chrono::milliseconds(500));
    receiveFrames->stop();
    receiverThread.join();
    output_receive = testing::internal::GetCapturedStdout();

}

TEST_F(ReceiveFramesTest, RequestTransferExitNegative)
{
    HandleFrames handleFrames;

    std::string output_receive = "";

    std::thread receiverThread([&]() {
        testing::internal::CaptureStdout();
        receiveFrames->receive(handleFrames);
    });
    struct can_frame sent_frame = createFrame({0x03, 0X7F, 0x37, 0x11});
    ASSERT_EQ(write(s2, &sent_frame, sizeof(sent_frame)), sizeof(sent_frame));

    std::this_thread::sleep_for(std::chrono::milliseconds(500));
    receiveFrames->stop();
    receiverThread.join();
    output_receive = testing::internal::GetCapturedStdout();

}

int main(int argc, char **argv) {
    s1 = createSocket();
    s2 = createSocket();
    system("sudo modprobe vcan");
    system("sudo ip link add type vcan name vcan0");
    system("sudo ip link set vcan0 up");
    testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
