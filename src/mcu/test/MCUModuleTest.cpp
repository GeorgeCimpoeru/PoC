#include "gtest/gtest.h"
#include "../include/MCUModule.h"
#include <linux/can.h>
#include "../../utils/include/Logger.h"
#include "../../utils/include/GenerateFrames.h"
#include "../../ecu_simulation/BatteryModule/include/BatteryModule.h"

int socket_canbus = -1;
int socket_api = -1;

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

class MCUModuleTest : public ::testing::Test {
protected:
    Logger* mockLogger;
    MCUModuleTest()
    {
        mockLogger = new Logger;
        battery = new BatteryModule(0x00, 0x11);
    }
    ~MCUModuleTest()
    {
        delete mockLogger;
        battery = nullptr;
    }
};

/* Start and stop module Test  */
TEST_F(MCUModuleTest, StartModuleTest) {

    /* Cover default constructor */
    MCU::mcu = new MCU::MCUModule();
    /* Cover parameterized constructor */
    testing::internal::CaptureStdout();
    MCU::mcu = new MCU::MCUModule(0x01);
    MCU::mcu->StartModule();
    std::string output = testing::internal::GetCapturedStdout();
    /* Expect mcu module to start */
    EXPECT_NE(output.find("Diagnostic Session Control (0x10) started."), std::string::npos);
    MCU::mcu->StopModule();
    MCU::mcu = nullptr;

}

/* Get mcu_api socket test  */
TEST_F(MCUModuleTest, GetMcuApiSocketTest) {

    /* Cover parameterized constructor */
    MCU::mcu = new MCU::MCUModule(0x01);
    int socket = MCU::mcu->getMcuApiSocket();
    EXPECT_TRUE(socket > 0);
    MCU::mcu = nullptr;

}

/* Get mcu_ecu socket test  */
TEST_F(MCUModuleTest, GetMcuEcuSocketTest) {

    /* Cover parameterized constructor */
    MCU::mcu = new MCU::MCUModule(0x01);
    int socket = MCU::mcu->getMcuEcuSocket();
    EXPECT_TRUE(socket > 0);
    MCU::mcu = nullptr;

}

/* Set mcu_api socket test  */
TEST_F(MCUModuleTest, SetMcuApiSocketTest) {

    /* Cover parameterized constructor */
    MCU::mcu = new MCU::MCUModule(0x01);
    MCU::mcu->setMcuApiSocket(0x01);
    int socket = MCU::mcu->getMcuApiSocket();
    EXPECT_TRUE(socket > 0);
    MCU::mcu = nullptr;
}

/* Set mcu_ecu socket test  */
TEST_F(MCUModuleTest, SetMcuEcuSocketTest) {

    /* Cover parameterized constructor */
    MCU::mcu = new MCU::MCUModule(0x01);
    MCU::mcu->setMcuEcuSocket(0x01);
    int socket = MCU::mcu->getMcuEcuSocket();
    EXPECT_TRUE(socket > 0);
    MCU::mcu = nullptr;

}
/* Receive Frames test */
TEST_F(MCUModuleTest, receiveFramesTest) {

    /* Initialize the MCU module and interfaces */
    MCU::mcu = new MCU::MCUModule(0x01);
    MCU::mcu->StartModule();
    CreateInterface* interface = CreateInterface::getInstance(0x01, *mockLogger);
    interface->startInterface();
    /* Thread to process the frames */
    
    std::thread receiver_thread([this] {
        testing::internal::CaptureStdout();
        MCU::mcu->recvFrames();
        std::string output = testing::internal::GetCapturedStdout();
        /* expect a frame to be received */
        EXPECT_NE(output.find("Captured a frame on the CANBus socket"), std::string::npos);

    });
    GenerateFrames* generate_frames = new GenerateFrames(socket_canbus, *mockLogger);
    generate_frames->sendFrame(0x1110, {0x00, 0x01, 0x02}, DATA_FRAME);
    sleep(1);
    MCU::mcu->StopModule();
    /* Join the threads to ensure completion */
    receiver_thread.join();
    MCU::mcu = nullptr;
}

int main(int argc, char* argv[])
{
    socket_canbus = createSocket();
    socket_api = createSocket();
    testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}