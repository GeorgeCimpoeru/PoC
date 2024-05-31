#include <gtest/gtest.h>
#include "../include/BatteryModule.h"

int createSocket()
{
    /* Create socket */
    std::string nameInterface = "vcan0";
    struct can_frame frame;
    struct sockaddr_can addr;
    struct ifreq ifr;
    int s;

    s = socket(PF_CAN, SOCK_RAW, CAN_RAW);
    if (s < 0)
    {
        std::cout << "Error trying to create the socket\n";
        return 1;
    }
    /* Giving name and index to the interface created */
    strcpy(ifr.ifr_name, nameInterface.c_str());
    ioctl(s, SIOCGIFINDEX, &ifr);
    /* Set addr structure with info. */
    addr.can_family = AF_CAN;
    addr.can_ifindex = ifr.ifr_ifindex;
    /* Bind the socket to the CAN interface */
    int b = bind(s, (struct sockaddr *)&addr, sizeof(addr));
    if (b < 0)
    {
        std::cout << "Error binding\n";
        return 1;
    }
    return s;
}

// Test default constructor
TEST(BatteryModuleTest, DefaultConstructor)
{
    BatteryModule batteryModule;
    EXPECT_EQ(batteryModule.getEnergy(), 0.0);
    EXPECT_EQ(batteryModule.getVoltage(), 0.0);
    EXPECT_EQ(batteryModule.getPercentage(), 0.0);
    EXPECT_FALSE(batteryModule.isRunning());
}

// Test parameterized constructor
TEST(BatteryModuleTest, ParameterizedConstructor)
{
    int interfaceNumber = 1;
    int moduleId = 0x102;
    BatteryModule batteryModule(interfaceNumber, moduleId);
    EXPECT_EQ(batteryModule.getEnergy(), 0.0);
    EXPECT_EQ(batteryModule.getVoltage(), 0.0);
    EXPECT_EQ(batteryModule.getPercentage(), 0.0);
    EXPECT_FALSE(batteryModule.isRunning());
}

// Test start and stop simulation
TEST(BatteryModuleTest, SimulationStartStop)
{
    BatteryModule batteryModule;
    batteryModule.simulate();
    std::this_thread::sleep_for(std::chrono::seconds(1)); // Allow some time for the thread to start
    EXPECT_TRUE(batteryModule.isRunning());
    batteryModule.stopBatteryModule();
    EXPECT_FALSE(batteryModule.isRunning());
}

// Mock the exec function to test fetchBatteryData
class BatteryModuleMock : public BatteryModule
{
public:
    std::string exec(const char *cmd) override
    {
        return "energy: 50.0\nvoltage: 12.5\npercentage: 80.0\nstate: Discharging\n";
    }
};

// Test fetchBatteryData
TEST(BatteryModuleTest, FetchBatteryData)
{
    BatteryModuleMock batteryModule;
    batteryModule.fetchBatteryData();
    EXPECT_EQ(batteryModule.getEnergy(), 50.0);
    EXPECT_EQ(batteryModule.getVoltage(), 12.5);
    EXPECT_EQ(batteryModule.getPercentage(), 80.0);
}

/* To Do : make this work.. */
/* Test receiveFrames (without Google Mock, we assume a simple implementation) */
TEST(BatteryModuleTest, ReceiveFrames)
{
    BatteryModule batteryModule;

    struct can_frame frame;
    frame.can_id = 0x101;
    frame.can_dlc = 0x1;
    frame.data[0] = 0x5;

    int s = createSocket();

    std::thread receive_thread([&batteryModule]()
                               { batteryModule.receiveFrames(); });

    std::this_thread::sleep_for(std::chrono::seconds(1));

    write(s, &frame, sizeof(frame));

    batteryModule.stopFrames();

    receive_thread.join();
}

// Test getter functions
TEST(BatteryModuleTest, GetterFunctions)
{
    BatteryModule batteryModule;
    EXPECT_EQ(batteryModule.getEnergy(), 0.0);
    EXPECT_EQ(batteryModule.getVoltage(), 0.0);
    EXPECT_EQ(batteryModule.getPercentage(), 0.0);
}

int main(int argc, char **argv)
{
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
