/**
 * @file SecurityAccessTest.cpp
 * @author Theodor Stoica
 * @brief Unit test for SecurityAccess Service
 * @version 0.1
 * @date 2024-07-16
 */
#include "../include/SecurityAccess.h"

#include <cstring>
#include <string>
#include <thread>

#include <fcntl.h>
#include <sys/ioctl.h>
#include <gtest/gtest.h>
#include <net/if.h>

int socket_;
int socket2_;
const int id = 0x10FA;

std::vector<uint8_t> seed;

class CaptureFrame
{
    public:
        struct can_frame frame;
        void capture()
        {
            read(socket_, &frame, sizeof(struct can_frame));
        }
};

struct can_frame createFrame(std::vector<uint8_t> test_data)
{
    struct can_frame result_frame;
    result_frame.can_id = id;
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
    EXPECT_EQ(expected_frame.can_id && 0xFFFF, c1.frame.can_id && 0xFFFF);
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

struct SecurityTest : testing::Test
{
    SecurityAccess* r;
    CaptureFrame* c1;
    Logger* logger;
    SecurityTest()
    {
        logger = new Logger();
        r = new SecurityAccess(socket2_, *logger);
        c1 = new CaptureFrame();
    }
    ~SecurityTest()
    {
        delete r;
        delete c1;
        delete logger;
    }
};

/* MCU is locked by default */
TEST_F(SecurityTest, DefaultMCUStateTest)
{
    EXPECT_FALSE(SecurityAccess::getMcuState(*logger));
}

TEST_F(SecurityTest, IncorrectMesssageLength)
{
    struct can_frame result_frame = createFrame({0x03, 0x7F, 
                    SecurityAccess::SECURITY_ACCESS_SID, NegativeResponse::IMLOIF});

    r->securityAccess(0xFA10, {0x03, 0x27, 0x03, 0x3F, 0x01, 0x00, 0x02});
    c1->capture();
    testFrames(result_frame, *c1);
}

/* We support only sf 0x01 and 0x02*/
TEST_F(SecurityTest, SubFunctionNotSupported)
{
    struct can_frame result_frame = createFrame({0x03, 0x7F, 
                    SecurityAccess::SECURITY_ACCESS_SID, NegativeResponse::SFNS});

    r->securityAccess(0xFA10, {0x06, SecurityAccess::SECURITY_ACCESS_SID, 
                    0x03, 0x3F, 0x01, 0x00, 0x02});
    c1->capture();
    testFrames(result_frame, *c1);
}

/* Send Key Request without RequestSeed */
TEST_F(SecurityTest, SendKeyBeforeRequestSeed)
{
    struct can_frame result_frame = createFrame({0x03, 0x7F, 
                    SecurityAccess::SECURITY_ACCESS_SID, NegativeResponse::RSE});
    r->securityAccess(0xFA10, {0x04, 0x27, 0x02, 0xCA, 0xA5});
    c1->capture();
    testFrames(result_frame, *c1);
}

TEST_F(SecurityTest, RequestSeed)
{
    std::string output_security = "";
    std::string searchLine = "Seed was sent.";

    testing::internal::CaptureStdout();
    r->securityAccess(0xFA10, {0x02, 0x27, 0x01});
    output_security = testing::internal::GetCapturedStdout();

    c1->capture();
    EXPECT_TRUE(containsLine(output_security, searchLine));
    if (c1->frame.can_dlc >= 4)
    {
        seed.clear();
        /* from 3 to pci_length we have the seed generated in response */
        for (int i = 3; i <= c1->frame.data[0]; i++)
        {
            seed.push_back(c1->frame.data[i]);
        }
    }
}

TEST_F(SecurityTest, InvalidKey)
{
    struct can_frame result_frame = createFrame({0x03, 0x7F, 
                    SecurityAccess::SECURITY_ACCESS_SID, NegativeResponse::IK});
    /* Send 3 wrong key */
    std::vector<uint8_t> data_frame = {static_cast<uint8_t>(seed.size() + 2), 0x27, 0x02};

    /** We now add the key to the sendKey frame
     * If we send the key == seed, the key will be wrong
    */
    data_frame.insert(data_frame.end(), seed.begin(), seed.end());
    for (int i = 0; i < 3; i++)
    {
        r->securityAccess(0xFA10, data_frame);
        c1->capture();
        testFrames(result_frame, *c1);
    }
}

TEST_F(SecurityTest, ExceededNrOfAttempts)
{
    struct can_frame result_frame = createFrame({0x03, 0x7F, 
                    SecurityAccess::SECURITY_ACCESS_SID, NegativeResponse::ENOA});
    /* Send another wrong key */
    std::vector<uint8_t> data_frame = {static_cast<uint8_t>(seed.size() + 2), 0x27, 0x02};
    
    /** We now add the key to the sendKey frame
     * If we send the key == seed, the key will be wrong
    */
    data_frame.insert(data_frame.end(), seed.begin(), seed.end());
    r->securityAccess(0xFA10, data_frame);
    c1->capture();
    testFrames(result_frame, *c1);
}

TEST_F(SecurityTest, RequiredTimeDelayNotExpired)
{
    /* Send another wrong key while delay timer activated */
    std::string output_security = "";
    std::string searchLine = "Please wait";

    testing::internal::CaptureStdout();
    std::vector<uint8_t> data_frame = {static_cast<uint8_t>(seed.size() + 2), 0x27, 0x02};
    data_frame.insert(data_frame.end(), seed.begin(), seed.end());
    r->securityAccess(0xFA10, data_frame);
    output_security = testing::internal::GetCapturedStdout();

    c1->capture();
    EXPECT_TRUE(containsLine(output_security, searchLine));

    /* Wait until delay timer expires. */
    sleep(SecurityAccess::TIMEOUT_IN_SECONDS);
}

TEST_F(SecurityTest, SendCorrectKey)
{
    struct can_frame result_frame = createFrame({0x02, 0x67, 0x02});
    /* Compute key from seed */
    for (auto &elem : seed)
    {
        elem = computeKey(elem);
    }
    std::vector<uint8_t> data_frame = {static_cast<uint8_t>(seed.size() + 2), 0x27, 0x02};
    data_frame.insert(data_frame.end(), seed.begin(), seed.end());
    r->securityAccess(0xFA10, data_frame);
    c1->capture();
    /* Security should be unlocked now */
    EXPECT_TRUE(SecurityAccess::getMcuState(*logger));

    testFrames(result_frame, *c1);
}

TEST_F(SecurityTest, RequestNotFromAPI)
{
    std::string output_security = "";
    std::string searchLine = "Security service can be accessed only from API.";
    testing::internal::CaptureStdout();
    r->securityAccess(0x1011, {0x02, 0x27, 0x01});
    output_security = testing::internal::GetCapturedStdout();
    EXPECT_TRUE(containsLine(output_security, searchLine));
}

TEST_F(SecurityTest, SendKeyAfterUnlock)
{
    std::string output_security = "";
    std::string searchLine = "Server is already unlocked.";
    testing::internal::CaptureStdout();
    /* Send a key, no matter if it is valid or not. */
    std::vector<uint8_t> data_frame = {static_cast<uint8_t>(seed.size() + 2), 0x27, 0x02};
    data_frame.insert(data_frame.end(), seed.begin(), seed.end());
    r->securityAccess(0xFA10, data_frame);
    output_security = testing::internal::GetCapturedStdout();
    EXPECT_TRUE(containsLine(output_security, searchLine));
}

TEST_F(SecurityTest, SecurityAccessExpires)
{
    sleep(3);
    /* Security expires */
    EXPECT_FALSE(SecurityAccess::getMcuState(*logger));
}

int main(int argc, char* argv[])
{
    socket_ = createSocket();
    socket2_ = createSocket();
    testing::InitGoogleTest(&argc, argv);
    int result = RUN_ALL_TESTS();
    if (socket_ > 0)
    {
        close(socket_);
    }
    if (socket2_ > 0)
    {
        close(socket2_);
    }
    return result;
}
