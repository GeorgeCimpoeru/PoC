#include <cstring>
#include <string>
#include <thread>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <gtest/gtest.h>
#include <net/if.h>

#include "../include/DiagnosticSessionControl.h"

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

struct DiagnosticSessionControlTest : testing::Test
{
    DiagnosticSessionControl * dsc;
    CaptureFrame* c1;
    Logger* logger;
    DiagnosticSessionControlTest()
    {
        logger = new Logger();
        dsc = new DiagnosticSessionControl(*logger, socket2);
        c1 = new CaptureFrame();
    }
    ~DiagnosticSessionControlTest()
    {
        delete dsc;
        delete c1;
        delete logger;
    }
};

/* Test for get Current Session To String method */
TEST_F(DiagnosticSessionControlTest, GetCurrentSessionToStringUnknownSession) {
    dsc->getCurrentSessionToString();
}

/* Test for Switching to Default Session */
TEST_F(DiagnosticSessionControlTest, SwitchToDefaultSession) {
    std::cerr << "Running SwitchToDefaultSession" << std::endl;
    
    struct can_frame result_frame = createFrame(0x10FA, {0x02, 0x50, 0x01});

    dsc->sessionControl(0xFA10, 0x01);
    c1->capture();
    testFrames(result_frame, *c1);
    std::cerr << "Finished SwitchToDefaultSession" << std::endl;
}

/* Test for get Current Session method */
TEST_F(DiagnosticSessionControlTest, GetCurrentSession) {
    EXPECT_EQ(dsc->getCurrentSession(), DEFAULT_SESSION);
}

/* Test for get Current Session To String method in Default Session*/
TEST_F(DiagnosticSessionControlTest, GetCurrentSessionToStringDefaultSession) {
    EXPECT_EQ(dsc->getCurrentSessionToString(), "DEFAULT_SESSION");
}


/* Test for Switching to Programming Session */
TEST_F(DiagnosticSessionControlTest, SwitchToProgrammingSession) {
    std::cerr << "Running SwitchToProgrammingSession" << std::endl;
    
    struct can_frame result_frame = createFrame(0x10FA, {0x02, 0x50, 0x02});

    dsc->sessionControl(0xFA10, 0x02);
    c1->capture();
    testFrames(result_frame, *c1);
    std::cerr << "Finished SwitchToProgrammingSession" << std::endl;
}

/* Test for get Current Session To String method in Programming Session*/
TEST_F(DiagnosticSessionControlTest, GetCurrentSessionToStringProgrammingSession) {
    EXPECT_EQ(dsc->getCurrentSessionToString(), "PROGRAMMING_SESSION");
}

/* Test for Switching to Extended Session */
TEST_F(DiagnosticSessionControlTest, SwitchToExtendedSession) {
    std::cerr << "Running SwitchToExtendedSession" << std::endl;
    
    struct can_frame result_frame = createFrame(0x10FA, {0x02, 0x50, 0x03});

    dsc->sessionControl(0xFA10, 0x03);
    c1->capture();
    testFrames(result_frame, *c1);
    std::cerr << "Finished SwitchToExtendedSession" << std::endl;
}

/* Test for get Current Session To String method in Extended Session */
TEST_F(DiagnosticSessionControlTest, GetCurrentSessionToStringExtendedSession) {
    EXPECT_EQ(dsc->getCurrentSessionToString(), "EXTENDED_DIAGNOSTIC_SESSION");
}

/* Test for Unsupported Subfunction */
TEST_F(DiagnosticSessionControlTest, UnsupportedSubfunction) {
    std::cerr << "Running UnsupportedSubfunction" << std::endl;
    
    struct can_frame result_frame = createFrame(0x10FA, {0x03, 0x7F, 0x10, 0x12});

    dsc->sessionControl(0xFA10, 0x04);
    c1->capture();
    testFrames(result_frame, *c1);
    std::cerr << "Finished UnsupportedSubfunction" << std::endl;
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
