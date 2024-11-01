
#include <cstring>
#include <string>
#include <thread>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <gtest/gtest.h>
#include <net/if.h>

#include "../include/ECU.h"
#include "../include/GenerateFrames.h"

int socket1;
int socket2;

class CaptureFrame
{
    public:
        struct can_frame frame;
        void capture()
        {
            read(socket1, &frame, sizeof(struct can_frame));
        }
};

struct ECUTest : testing::Test
{
    Logger* logger;
    ECU* ecu;
    CaptureFrame* c1;
    GenerateFrames* g;
    ECUTest()
    {
        logger = new Logger();
        ecu = new ECU(0x11, *logger);
        c1 = new CaptureFrame();
        g = new GenerateFrames(socket1, *logger);
    }
    ~ECUTest()
    {
        delete ecu;
        delete g;
        delete c1;
        delete logger;
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

TEST_F(ECUTest, NotificationToMcu)
{
    std::cerr << "Running NotificationToMcu" << std::endl;

    struct can_frame result_frame = createFrame(0x1110, {0x01, 0xD9});
    ecu->sendNotificationToMCU();
    c1->capture();
    testFrames(result_frame, *c1);

    std::cerr << "Finished NotificationToMcu" << std::endl;
}

TEST_F(ECUTest, StartFrames)
{
    std::cerr << "Running StartFrames" << std::endl;

    testing::internal::CaptureStdout();
    std::thread processor_thread([this] {
        ecu->startFrames();
    });
    ecu->stopFrames();
    processor_thread.join();
    std::string output = testing::internal::GetCapturedStdout();
    EXPECT_NE(output.find("11 starts the frame receiver"), std::string::npos);

    std::cerr << "Finished StartFrames" << std::endl;
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