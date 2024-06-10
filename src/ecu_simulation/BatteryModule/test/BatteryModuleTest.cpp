#include <gtest/gtest.h>
#include "../include/BatteryModule.h"

/* Helper function to create a socket for sending CAN frames */
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

/* Test default constructor */
TEST(BatteryModuleTest, DefaultConstructor)
{
    BatteryModule batteryModule;
    EXPECT_EQ(batteryModule.getEnergy(), 0.0);
    EXPECT_EQ(batteryModule.getVoltage(), 0.0);
    EXPECT_EQ(batteryModule.getPercentage(), 0.0);
    EXPECT_EQ(batteryModule.getLinuxBatteryState(), "");
}

/* Test parameterized constructor */
TEST(BatteryModuleTest, ParameterizedConstructor)
{
    int interfaceNumber = 1;
    int moduleId = 0x102;
    BatteryModule batteryModule(interfaceNumber, moduleId);
    EXPECT_EQ(batteryModule.getEnergy(), 0.0);
    EXPECT_EQ(batteryModule.getVoltage(), 0.0);
    EXPECT_EQ(batteryModule.getPercentage(), 0.0);
    EXPECT_EQ(batteryModule.getLinuxBatteryState(), "");
}

/* 
 * Mock the exec function to test fetchBatteryData
 * This class inherits from BatteryModule and overrides the exec function 
 */
class BatteryModuleMock : public BatteryModule
{
public:
    std::string exec(const char *cmd) override
    {
        return "energy: 50.0\nvoltage: 12.5\npercentage: 80.0\nstate: Discharging\n";
    }
};

/* Test fetchBatteryData */
TEST(BatteryModuleTest, FetchBatteryData)
{
    BatteryModuleMock batteryModule;
    batteryModule.fetchBatteryData();
    EXPECT_EQ(batteryModule.getEnergy(), 50.0);
    EXPECT_EQ(batteryModule.getVoltage(), 12.5);
    EXPECT_EQ(batteryModule.getPercentage(), 80.0);
    EXPECT_EQ(batteryModule.getLinuxBatteryState(), "Discharging");
}

/* 
 * Test receiveFrames (without Google Mock, we assume a simple implementation) 
 * This test sends a CAN frame to the BatteryModule and checks if it is received correctly 
 */
TEST(BatteryModuleTest, ReceiveFrames)
{
    BatteryModule batteryModule;

    /* Create a CAN frame */
    struct can_frame frame;
    frame.can_id = 0x101;
    frame.can_dlc = 0x1;
    frame.data[0] = 0x5;

    /* Create socket for sending the frame */
    int s = createSocket();

    /* Start the receiveFrames thread */
    std::thread receive_thread([&batteryModule]()
                               { batteryModule.receiveFrames(); });

    /* Send the frame */
    write(s, &frame, sizeof(frame));

    /* Allow some time for the thread to start */
    std::this_thread::sleep_for(std::chrono::seconds(1));

    batteryModule.stopFrames();

    receive_thread.join();
}

/* Test getter functions */
TEST(BatteryModuleTest, GetterFunctions)
{
    BatteryModule batteryModule;
    EXPECT_EQ(batteryModule.getEnergy(), 0.0);
    EXPECT_EQ(batteryModule.getVoltage(), 0.0);
    EXPECT_EQ(batteryModule.getPercentage(), 0.0);
    EXPECT_EQ(batteryModule.getLinuxBatteryState(), "");
}

/* Test getLinuxBatteryState function */
TEST(BatteryModuleTest, GetLinuxBatteryState)
{
    BatteryModuleMock batteryModule;

    /* Fetch battery data to set the state */
    batteryModule.fetchBatteryData();

    /* Check the state returned by getLinuxBatteryState */
    EXPECT_EQ(batteryModule.getLinuxBatteryState(), "Discharging");
}

/* Test BatteryModule destructor */
TEST(BatteryModuleTest, Destructor)
{
    BatteryModule* batteryModule = new BatteryModule();
    delete batteryModule;
}

/* Test parseBatteryInfo function */
TEST(BatteryModuleTest, ParseBatteryInfo)
{
    BatteryModule batteryModule;

    float energy = 0.0;
    float voltage = 0.0;
    float percentage = 0.0;
    std::string state;

    std::string data = "energy: 50.0\nvoltage: 12.5\npercentage: 80.0\nstate: Discharging\n";
    batteryModule.parseBatteryInfo(data, energy, voltage, percentage, state);

    EXPECT_EQ(energy, 50.0);
    EXPECT_EQ(voltage, 12.5);
    EXPECT_EQ(percentage, 80.0);
    EXPECT_EQ(state, "Discharging");
}

/* Test exec function */
TEST(BatteryModuleTest, ExecFunction)
{
    BatteryModule batteryModule;
    std::string result = batteryModule.exec("echo Hello");
    EXPECT_EQ(result, "Hello\n");
}

int main(int argc, char **argv)
{
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
