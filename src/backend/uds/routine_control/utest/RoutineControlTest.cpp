#include "../include/RoutineControl.h"
#include "../../diagnostic_session_control/include/DiagnosticSessionControl.h"
#include "../../utils/include/FileManager.h"

#include <cstring>
#include <string>
#include <thread>

#include <fcntl.h>
#include <sys/ioctl.h>
#include <gtest/gtest.h>
#include <net/if.h>

int socket_;
int socket2_;
std::vector<uint8_t> seed;
std::string file_path = std::string(PROJECT_PATH) + "/src/mcu/mcu_data.txt";

/* Class to capture the frame sin the can-bus */
class CaptureFrame
{
    public:
        struct can_frame frame;
        void capture()
        {
            int nbytes = read(socket_, &frame, sizeof(struct can_frame));
            if(nbytes < 0)
            {
                return;
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
    int flags = fcntl(s, F_GETFL, 0);
    if (flags == -1) {
        return 1;
    }
    /* Set the O_NONBLOCK flag to make the socket non-blocking */
    flags |= O_NONBLOCK;
    if (fcntl(s, F_SETFL, flags) == -1) {
        return -1;
    }
    return s;
}

struct can_frame createFrame(uint32_t can_id ,std::vector<uint8_t> test_data)
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

void testFrames(struct can_frame expected_frame, CaptureFrame &c1 )
{
    EXPECT_EQ(expected_frame.can_id && 0xFFFF, c1.frame.can_id && 0xFFFF);
    EXPECT_EQ(expected_frame.can_dlc, c1.frame.can_dlc);
    for (int i = 0; i < expected_frame.can_dlc; ++i) {
        EXPECT_EQ(expected_frame.data[i], c1.frame.data[i]);
    }
}

uint8_t computeKey(uint8_t& seed)
{
    return ~seed + 1;
}

/* Create object for all tests */
struct RoutineControlTest : testing::Test
{
    RoutineControl* routine_control;
    SecurityAccess* r;
    DiagnosticSessionControl* dsc;
    CaptureFrame* capture_frame;
    Logger logger;
    RoutineControlTest()
    {
        routine_control = new RoutineControl(socket2_,logger);
        r = new SecurityAccess(socket2_, logger);
        dsc = new DiagnosticSessionControl(logger, socket2_);
        capture_frame = new CaptureFrame();
    }
    ~RoutineControlTest()
    {
        delete routine_control;
        delete r;
        delete dsc;
        delete capture_frame;
    }

    void checkSecurity()
    {
        /* Check the security */
        /* Request seed */
        r->securityAccess(0xFA10, {0x02, 0x27, 0x01});

        capture_frame->capture();
        if (capture_frame->frame.can_dlc >= 4)
        {
            seed.clear();
            /* from 3 to pci_length we have the seed generated in response */
            for (int i = 3; i <= capture_frame->frame.data[0]; i++)
            {
                seed.push_back(capture_frame->frame.data[i]);
            }
        }
        /* Compute key from seed */
        for (auto &elem : seed)
        {
            elem = computeKey(elem);
        }

        std::vector<uint8_t> data_frame = {static_cast<uint8_t>(seed.size() + 2), 0x27, 0x02};
        data_frame.insert(data_frame.end(), seed.begin(), seed.end());
        r->securityAccess(0xFA10, data_frame);
        capture_frame->capture();
    }
};

TEST_F(RoutineControlTest, IncorrectMessageLength)
{
     struct can_frame expected_frame = createFrame(0x001010FA, {0x03, 0x7F, 0x31, NegativeResponse::IMLOIF});

    routine_control->routineControl(0x0010fa10, {0x03, 0x31, 0x01, 0x02});
    capture_frame->capture();
    testFrames(expected_frame, *capture_frame);
}

TEST_F(RoutineControlTest, SubFunctionNotSupported)
{
    struct can_frame expected_frame = createFrame(0x001010FA, {0x03, 0x7F, 0x31, 0x12});

    routine_control->routineControl(0x0010fa10, {0x05, 0x31, 0x00, 0x01, 0x03, 0x03});
    capture_frame->capture();
    testFrames(expected_frame, *capture_frame);
}

TEST_F(RoutineControlTest, MCUSecurity)
{
    struct can_frame expected_frame = createFrame(0x001010FA, {0x03, 0x7F, 0x31, NegativeResponse::SAD});

    dsc->sessionControl(0x0010fa10, 0x03);
    capture_frame->capture();
    routine_control->routineControl(0x0010FA10, {0x05, 0x31, 0x01, 0x02, 0x01, 0x10});
    capture_frame->capture();
    testFrames(expected_frame, *capture_frame);
}

TEST_F(RoutineControlTest, ECUSecurity)
{
    struct can_frame expected_frame = createFrame(0x001011FA, {0x03, 0x7F, 0x31, NegativeResponse::SAD});

    routine_control->routineControl(0x0010FA11, {0x05, 0x31, 0x01, 0x02, 0x01, 0x10});
    capture_frame->capture();
    testFrames(expected_frame, *capture_frame);
}

TEST_F(RoutineControlTest, RequestOutOfRange)
{

    struct can_frame expected_frame = createFrame(0x001010FA, {0x03, 0x7F, 0x31, NegativeResponse::ROOR});

    checkSecurity();

    routine_control->routineControl(0x0010FA10, {0x05, 0x31, 0x01, 0x00, 0x01, 0x10});
    capture_frame->capture();
    testFrames(expected_frame, *capture_frame);
}

TEST_F(RoutineControlTest, EraseMemory)
{
    struct can_frame expected_frame = createFrame(0x001010FA, {0x05, 0x71, 0x01, 0x01, 0x01, 0x00});

    checkSecurity();

    routine_control->routineControl(0x0010FA10, {0x05, 0x31, 0x01, 0x01, 0x01, 0x00});
    capture_frame->capture();
    testFrames(expected_frame, *capture_frame);
}

TEST_F(RoutineControlTest, WrongSession)
{
    dsc->sessionControl(0x0010fa10, 0x02);
    capture_frame->capture();

    struct can_frame expected_frame = createFrame(0x001010FA, {0x03, 0x7F, 0x31, NegativeResponse::SFNSIAS});
    checkSecurity();

    routine_control->routineControl(0x0010FA10, {0x05, 0x31, 0x01, 0x02, 0x01, 0x00});
    capture_frame->capture();
    testFrames(expected_frame, *capture_frame);
}

TEST_F(RoutineControlTest, OTAStateAlreadyInitialised)
{
    dsc->sessionControl(0x0010fa10, 0x03);
    capture_frame->capture();
    
    struct can_frame expected_frame = createFrame(0x001010FA, {0x03, 0x7F, 0x31, NegativeResponse::CNC});
    checkSecurity();

    std::unordered_map<uint16_t, std::vector<uint8_t>> data_map = {{0x01E0, {0x40}}};
    FileManager::writeMapToFile(file_path, data_map);

    routine_control->routineControl(0x0010FA10, {0x05, 0x31, 0x01, 0x02, 0x01, 0x00});
    capture_frame->capture();
    testFrames(expected_frame, *capture_frame);
}

TEST_F(RoutineControlTest, VersionNotFound )
{
    std::unordered_map<uint16_t, std::vector<uint8_t>> data_map = {{0x01E0, {0xff}}};
    FileManager::writeMapToFile(file_path, data_map);
    struct can_frame expected_frame = createFrame(0x001010FA, {0x03, 0x7F, 0x31, NegativeResponse::IMLOIF});
    checkSecurity();

    routine_control->routineControl(0x0010FA10, {0x05, 0x31, 0x01, 0x02, 0x01, 0x70});
    capture_frame->capture();
    testFrames(expected_frame, *capture_frame);
}

TEST_F(RoutineControlTest, InvalidEcuPath)
{
    struct can_frame expected_frame = createFrame(0x001015FA, {0x03, 0x7F, 0x31, NegativeResponse::SFNSIAS});
    checkSecurity();

    routine_control->routineControl(0x0010FA15, {0x05, 0x31, 0x01, 0x03, 0x01, 0x10});
    capture_frame->capture();
    testFrames(expected_frame, *capture_frame);
}

TEST_F(RoutineControlTest, PositiveResponse1)
{
    struct can_frame expected_frame = createFrame(0x001010FA, {0x05, 0x71, 0x01, 0x04, 0x01, 0x00});
    checkSecurity();

    routine_control->routineControl(0x0010FA10, {0x05, 0x31, 0x01, 0x04, 0x01, 0x10});
    capture_frame->capture();
    testFrames(expected_frame, *capture_frame);
}

TEST_F(RoutineControlTest, OTARollbackWrongSession)
{ 
    struct can_frame expected_frame = createFrame(0x001010FA, {0x03, 0x7F, 0x31, NegativeResponse::CNC});
    checkSecurity();

    routine_control->routineControl(0x0010FA10, {0x05, 0x31, 0x01, 0x05, 0x01, 0x00});
    capture_frame->capture();
    testFrames(expected_frame, *capture_frame);
}

TEST_F(RoutineControlTest, SaveCurrentSoftwareFailed)
{ 
    struct can_frame expected_frame = createFrame(0x001010FA, {0x03, 0x7F, 0x31, NegativeResponse::IMLOIF});
    checkSecurity();

    routine_control->routineControl(0x0010FA10, {0x05, 0x31, 0x01, 0x06, 0x01, 0x00});
    capture_frame->capture();
    testFrames(expected_frame, *capture_frame);
}

TEST_F(RoutineControlTest, Rollback)
{ 
    EXPECT_EQ(routine_control->activateSoftware(), 0);
}

int main(int argc, char* argv[])
{
    socket_ = createSocket();
    socket2_ = createSocket();
    testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}