#include "gtest/gtest.h"
#include <sys/prctl.h>
#include "../include/MCUModule.h"
#include <linux/can.h>
#include "../../utils/include/Logger.h"
#include "../../utils/include/GenerateFrames.h"
#include "../../ecu_simulation/BatteryModule/include/BatteryModule.h"

int socket_canbus = -1;

void createMCUProcess()
{
    pid_t pid = fork();

    if (pid == 0)
    {
        prctl(PR_SET_NAME, "main_mcu", 0, 0, 0);
    }
    else if (pid > 0)
    {
        std::cout << "Parent process, launched main_mcu_process with PID " << pid << "\n";
    }
    else
    {
        std::cerr << "Fork failed\n";
    }
}

bool containsLine(const std::string& output, const std::string& line)
{
    return output.find(line) != std::string::npos;
}

void testByteVectors(const std::vector<uint8_t>& expected, const std::vector<uint8_t>& actual)
{
    EXPECT_EQ(expected.size(), actual.size()) << "Vectors have different sizes";

    /* Compare the contents of the vectors */
    for (size_t i = 0; i < expected.size(); ++i) {
        EXPECT_EQ(expected[i], actual[i]) << "Vectors differ at index " << i;
    }
}

int createSocket()
{
    /* Create socket */
    std::string name_interface = "vcan1";
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
    }
    ~MCUModuleTest()
    {
        delete mockLogger;
    }
};

TEST_F(MCUModuleTest, ErrorKillMCUProcess)
{
    MCU::mcu = new MCU::MCUModule(0x01);
    MCU::mcu->StartModule();
    MCU::mcu->StopModule();
    testing::internal::CaptureStdout();
    delete MCU::mcu;
    std::string output = testing::internal::GetCapturedStdout();
    EXPECT_TRUE(containsLine(output, "Error when trying to kill main_mcu process"));
}

/* Start and stop module Test  */
TEST_F(MCUModuleTest, StartModuleTest)
{
    /* Cover default constructor */
    createMCUProcess();
    MCU::mcu = new MCU::MCUModule();
    delete MCU::mcu;
    createMCUProcess();
    /* Cover parameterized constructor */
    testing::internal::CaptureStdout();
    MCU::mcu = new MCU::MCUModule(0x01);
    MCU::mcu->StartModule();
    std::string output = testing::internal::GetCapturedStdout();
    /* Expect mcu module to start */
    EXPECT_NE(output.find("Diagnostic Session Control (0x10) started."), std::string::npos);
    MCU::mcu->StopModule();
    delete MCU::mcu;
}

/* Get mcu_api socket test  */
TEST_F(MCUModuleTest, GetMcuApiSocketTest)
{
    MCU::mcu = new MCU::MCUModule(0x01);
    createMCUProcess();
    int socket = MCU::mcu->getMcuApiSocket();
    EXPECT_TRUE(socket > 0);
    delete MCU::mcu;
}

/* Get mcu_ecu socket test  */
TEST_F(MCUModuleTest, GetMcuEcuSocketTest)
{
    MCU::mcu = new MCU::MCUModule(0x01);
    createMCUProcess();
    int socket = MCU::mcu->getMcuEcuSocket();
    EXPECT_TRUE(socket > 0);
    delete MCU::mcu;
}

/* Set mcu_api socket test  */
TEST_F(MCUModuleTest, SetMcuApiSocketTest)
{
    MCU::mcu = new MCU::MCUModule(0x01);
    createMCUProcess();
    MCU::mcu->setMcuApiSocket(0x01);
    int socket = MCU::mcu->getMcuApiSocket();
    EXPECT_TRUE(socket > 0);
    delete MCU::mcu;
}

/* Set mcu_ecu socket test  */
TEST_F(MCUModuleTest, SetMcuEcuSocketTest)
{
    MCU::mcu = new MCU::MCUModule(0x01);
    createMCUProcess();
    MCU::mcu->setMcuEcuSocket(0x01);
    int socket = MCU::mcu->getMcuEcuSocket();
    EXPECT_TRUE(socket > 0);
    delete MCU::mcu;
}

TEST_F(MCUModuleTest, receiveFramesTest)
{

    /* Initialize the MCU module and interfaces */
    MCU::mcu = new MCU::MCUModule(0x01);
    createMCUProcess();
    MCU::mcu->StartModule();
    CreateInterface* interface = CreateInterface::getInstance(0x01, *mockLogger);
    interface->startInterface();
    testing::internal::CaptureStdout();
    std::thread receiver_thread([this] {
        MCU::mcu->recvFrames();
    });
    GenerateFrames* generate_frames = new GenerateFrames(socket_canbus, *mockLogger);
    generate_frames->sendFrame(0x1110, {0x00, 0x01, 0x02}, DATA_FRAME);
    sleep(1);
    MCU::mcu->StopModule();
    /* Join the threads to ensure completion */
    receiver_thread.join();
    std::string output = testing::internal::GetCapturedStdout();
    EXPECT_TRUE(containsLine(output, "Frame processing method invoked!"));
    delete MCU::mcu;
}

TEST_F(MCUModuleTest, SetDIDValue)
{
    MCU::mcu = new MCU::MCUModule(0x01);
    createMCUProcess();
    MCU::mcu->setMcuEcuSocket(0x01);
    /* (OTA_UPDATE_STATUS_DID, {INIT}) */
    MCU::mcu->setDidValue(0x01E0,{16});
    delete MCU::mcu;
}

TEST_F(MCUModuleTest, SetDIDValueUnknown)
{
    MCU::mcu = new MCU::MCUModule(0x01);
    createMCUProcess();
    MCU::mcu->setMcuEcuSocket(0x01);
    /* (OTA_UPDATE_STATUS_DID, {INIT}) */
    testing::internal::CaptureStdout();
    MCU::mcu->setDidValue(0x01E5,{16});
    std::string output = testing::internal::GetCapturedStdout();
    EXPECT_TRUE(containsLine(output, "not found when trying to set value"));
    delete MCU::mcu;
}

TEST_F(MCUModuleTest, GetDIDValue)
{
    MCU::mcu = new MCU::MCUModule(0x01);
    createMCUProcess();
    MCU::mcu->setMcuEcuSocket(0x01);
    /* (OTA_UPDATE_STATUS_DID, {INIT}) */
    testing::internal::CaptureStdout();
    MCU::mcu->setDidValue(0x01E0,{16});
    std::vector<uint8_t> result;
    result = MCU::mcu->getDidValue(0x01E0);
    testByteVectors(result,{16});
    delete MCU::mcu;
}

TEST_F(MCUModuleTest, WriteFailMCUData)
{
    std::ofstream outfile("old_mcu_data.txt");
    MCU::mcu = new MCU::MCUModule(0x01);
    createMCUProcess();
    MCU::mcu->writeDataToFile();
    delete MCU::mcu;
    outfile.close();
}

TEST_F(MCUModuleTest, WriteExceptionThrown)
{
    MCU::mcu = new MCU::MCUModule(0x01);
    createMCUProcess();
    std::string path = "mcu_data.txt";
    std::ofstream outfile(path);
    chmod(path.c_str(), 0);
    EXPECT_THROW(
    {
        MCU::mcu->writeDataToFile();
    }, std::runtime_error);
    path = "mcu_data.txt";
    chmod(path.c_str(), 0666);
    outfile.close();
    delete MCU::mcu;
    /* Restore the file */
    MCU::mcu = new MCU::MCUModule(0x01);
    delete MCU::mcu;
}

int main(int argc, char* argv[])
{
    socket_canbus = createSocket();
    testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}