#include <cstring>
#include <string>
#include <thread>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <gtest/gtest.h>
#include <net/if.h>

#include "../include/AccessTimingParameter.h"
#include "../../../ecu_simulation/BatteryModule/include/BatteryModule.h"
#include "../../../ecu_simulation/EngineModule/include/EngineModule.h"
#include "../../../ecu_simulation/DoorsModule/include/DoorsModule.h"
#include "../../../ecu_simulation/HVACModule/include/HVACModule.h"

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

struct AccessTimingParameterTest : testing::Test
{
    AccessTimingParameter* atp;
    CaptureFrame* c1;
    Logger* logger;
    AccessTimingParameterTest()
    {
        logger = new Logger();
        atp = new AccessTimingParameter(*logger, socket2);
        c1 = new CaptureFrame();
    }
    ~AccessTimingParameterTest()
    {
        delete atp;
        delete c1;
        delete logger;
    }
};

/* Test for Read Extended Timing Parameter */
TEST_F(AccessTimingParameterTest, ReadExtendedTimingParameter) {
    std::cerr << "Running ReadExtendedTimingParameter" << std::endl;
    
    struct can_frame result_frame = createFrame(0x10FA, {0x06, 0xC3, 0x01, 0x00, 0x28, 0x01, 0x90});

    atp->handleRequest(0xFA10, 0x01, {});
    c1->capture();
    testFrames(result_frame, *c1);
    std::cerr << "Finished ReadExtendedTimingParameter" << std::endl;
}

/* Test for Read Currently Active Timing Parameter */
TEST_F(AccessTimingParameterTest, ReadCurrentlyActiveTimingParameter) {
    std::cerr << "Running ReadCurrentlyActiveTimingParameter" << std::endl;
    
    struct can_frame result_frame = createFrame(0x10FA, {0x06, 0xC3, 0x03, 0x00, 0x28, 0x01, 0x90});

    atp->handleRequest(0xFA10, 0x03, {});
    c1->capture();
    testFrames(result_frame, *c1);
    std::cerr << "Finished ReadCurrentlyActiveTimingParameter" << std::endl;
}

/* Test for Set Timing Parameter To Default */
TEST_F(AccessTimingParameterTest, SetTimingParameterToDefault) {
    std::cerr << "Running SetTimingParameterToDefault" << std::endl;
    
    struct can_frame result_frame = createFrame(0x10FA, {0x02, 0xC3, 0x02});

    atp->handleRequest(0xFA10, 0x02, {});
    c1->capture();
    testFrames(result_frame, *c1);
    std::cerr << "Finished SetTimingParameterToDefault" << std::endl;
}

/* Test for Set Timing Parameter */
TEST_F(AccessTimingParameterTest, SetTimingParameter) {
    std::cerr << "Running SetTimingParameter" << std::endl;
    
    struct can_frame result_frame = createFrame(0x10FA, {0x02, 0xC3, 0x04});

    atp->handleRequest(0xFA10, 0x04, {0x00, 0x50, 0x03, 0x20});
    c1->capture();
    testFrames(result_frame, *c1);
    std::cerr << "Finished SetTimingParameter" << std::endl;
}

/* Test for Set Timing Parameter Wrong Lenght */
TEST_F(AccessTimingParameterTest, SetTimingParameterWrongLenght) {
    std::cerr << "Running SetTimingParameterWrongLenght" << std::endl;
    
    struct can_frame result_frame = createFrame(0x10FA, {0x03, 0x7F, 0x83, 0x13});

    atp->handleRequest(0xFA10, 0x04, {0x00, 0x50});
    c1->capture();
    testFrames(result_frame, *c1);
    std::cerr << "Finished SetTimingParameterWrongLenght" << std::endl;
}

/* Test for Unsupported Subfunction */
TEST_F(AccessTimingParameterTest, UnsupportedSubfunction) {
    std::cerr << "Running UnsupportedSubfunction" << std::endl;
    
    struct can_frame result_frame = createFrame(0x10FA, {0x03, 0x7F, 0x83, 0x12});

    atp->handleRequest(0xFA10, 0x05, {});
    c1->capture();
    testFrames(result_frame, *c1);
    std::cerr << "Finished UnsupportedSubfunction" << std::endl;
}

/* Test for Stop TimingFlag for battery */
TEST_F(AccessTimingParameterTest, StopTimingFlagBattery) {
    std::cerr << "Running StopTimingFlagBattery" << std::endl;
    
    battery->_ecu->stop_flags[0x83] = true;
    atp->stopTimingFlag(0x11, 0x83);

    EXPECT_FALSE(battery->_ecu->stop_flags[0x83]);
    std::cerr << "Finished StopTimingFlagBattery" << std::endl;
}

/* Test for Stop TimingFlag for engine */
TEST_F(AccessTimingParameterTest, StopTimingFlagEngine) {
    std::cerr << "Running StopTimingFlagEngine" << std::endl;
    
    engine->_ecu->stop_flags[0x83] = true;
    atp->stopTimingFlag(0x12, 0x83);

    EXPECT_FALSE(engine->_ecu->stop_flags[0x83]);
    std::cerr << "Finished StopTimingFlagEngine" << std::endl;
}

/* Test for Stop TimingFlag for dooors */
TEST_F(AccessTimingParameterTest, StopTimingFlagDoors) {
    std::cerr << "Running StopTimingFlagDoors" << std::endl;
    
    doors->_ecu->stop_flags[0x83] = true;
    atp->stopTimingFlag(0x13, 0x83);

    EXPECT_FALSE(doors->_ecu->stop_flags[0x83]);
    std::cerr << "Finished StopTimingFlagDoors" << std::endl;
}

/* Test for Stop TimingFlag for HVAC */
TEST_F(AccessTimingParameterTest, StopTimingFlagHVAC) {
    std::cerr << "Running StopTimingFlagHVAC" << std::endl;
    
    hvac->_ecu->stop_flags[0x83] = true;
    atp->stopTimingFlag(0x14, 0x83);

    EXPECT_FALSE(hvac->_ecu->stop_flags[0x83]);
    std::cerr << "Finished StopTimingFlagHVAC" << std::endl;
}

/* Test for Stop TimingFlag for default case */
TEST_F(AccessTimingParameterTest, StopTimingFlagDefault) {
    std::cerr << "Running StopTimingFlagDefault" << std::endl;
    
    atp->stopTimingFlag(0x15, 0x83);

    std::cerr << "Finished StopTimingFlagDefault" << std::endl;
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
