#include <gtest/gtest.h>
#include "src/ecu_simulation/BatteryModule/include/BatteryModule.h"

/* Helper function to create a socket for sending CAN frames */
int createSocket()
{
    /* Create socket */
    std::string nameInterface = "vcan0";
    struct sockaddr_can addr;
    struct ifreq ifr;
    int s = socket(PF_CAN, SOCK_RAW, CAN_RAW);
    if (s < 0)
    {
        std::cout << "Error trying to create the socket\n";
        return 1;
    }

    /* Giving name and index to the interface created */
    strcpy(ifr.ifr_name, nameInterface.c_str());
    ioctl(s, SIOCGIFINDEX, &ifr);

    /* Set addr structure with info */
    addr.can_family = AF_CAN;
    addr.can_ifindex = ifr.ifr_ifindex;

    /* Bind the socket to the CAN interface */
    if (bind(s, (struct sockaddr *)&addr, sizeof(addr)) < 0)
    {
        std::cout << "Error binding\n";
        return 1;
    }

    return s;
}

/* Mock class for testing fetchBatteryData */
class BatteryModuleMock : public BatteryModule
{
public:
    /* Override the exec function to return a mock battery data string */
    std::string exec(const char *cmd) override
    {
        return "energy: 50.0\nvoltage: 12.5\npercentage: 80.0\nstate: Discharging\n";
    }
};

/* Mock class for testing popen failure */
class BatteryModulePopenFailMock : public BatteryModule
{
public:
    /* Override the exec function to simulate popen failure */
    std::string exec(const char *cmd) override
    {
        throw std::runtime_error("popen() failed!");
    }
};

/* Mock class for testing fetchBatteryData exception handling */
class BatteryModuleExecExceptionMock : public BatteryModule
{
public:
    /* Override the exec function to throw an exception */
    std::string exec(const char *cmd) override
    {
        throw std::runtime_error("Simulated exec failure");
    }
};

/* Test default constructor */
TEST(BatteryModuleTest, DefaultConstructor)
{
    BatteryModule batteryModule;
    /* Check initial values of member variables */
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
    /* Check initial values of member variables */
    EXPECT_EQ(batteryModule.getEnergy(), 0.0);
    EXPECT_EQ(batteryModule.getVoltage(), 0.0);
    EXPECT_EQ(batteryModule.getPercentage(), 0.0);
    EXPECT_EQ(batteryModule.getLinuxBatteryState(), "");
}

/* Test fetchBatteryData function */
TEST(BatteryModuleTest, FetchBatteryData)
{
    BatteryModuleMock batteryModule;
    batteryModule.fetchBatteryData();
    /* Check if values are correctly fetched and parsed */
    EXPECT_EQ(batteryModule.getEnergy(), 50.0);
    EXPECT_EQ(batteryModule.getVoltage(), 12.5);
    EXPECT_EQ(batteryModule.getPercentage(), 80.0);
    EXPECT_EQ(batteryModule.getLinuxBatteryState(), "Discharging");
}

/* Test exception throwing in exec function */
TEST(BatteryModuleTest, ExecFunctionThrows)
{
    BatteryModulePopenFailMock batteryModule;
    /* Expect the exec function to throw std::runtime_error */
    EXPECT_THROW(batteryModule.exec("some_command"), std::runtime_error);
}

/* Test fetchBatteryData exception handling */
TEST(BatteryModuleTest, FetchBatteryDataExceptionHandling)
{
    BatteryModuleExecExceptionMock batteryModule;
    /* Expect no throw, but error should be logged */
    EXPECT_NO_THROW(batteryModule.fetchBatteryData());
}

/* Test receiveFrames function (without Google Mock, simple implementation) */
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

    /* Allow some time for the thread to start */
    std::this_thread::sleep_for(std::chrono::milliseconds(500));

    /* Send the frame */
    write(s, &frame, sizeof(frame));

    /* Allow some time for the frame to be received */
    std::this_thread::sleep_for(std::chrono::milliseconds(500));

    /* Stop the frame receiver */
    batteryModule.stopFrames();

    /* Join the thread */
    receive_thread.join();
}

/* Test getter functions */
TEST(BatteryModuleTest, GetterFunctions)
{
    BatteryModule batteryModule;
    /* Check initial values of member variables */
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
    BatteryModule *batteryModule = new BatteryModule();
    delete batteryModule;
    /* Check if logger has logged the deletion (requires logger mock, not implemented here) */
}

/* Test parseBatteryInfo function */
TEST(BatteryModuleTest, ParseBatteryInfo)
{
    BatteryModule batteryModule;

    /* Variables to hold parsed data */
    float energy = 0.0, voltage = 0.0, percentage = 0.0;
    std::string state;

    /* Mock battery data string */
    std::string data = "energy: 50.0\nvoltage: 12.5\npercentage: 80.0\nstate: Discharging\n";

    /* Parse the battery data */
    batteryModule.parseBatteryInfo(data, energy, voltage, percentage, state);

    /* Check if values are correctly parsed */
    EXPECT_EQ(energy, 50.0);
    EXPECT_EQ(voltage, 12.5);
    EXPECT_EQ(percentage, 80.0);
    EXPECT_EQ(state, "Discharging");
}

/* Test exec function */
TEST(BatteryModuleTest, ExecFunction)
{
    BatteryModule batteryModule;
    /* Execute a simple command and check the result */
    std::string result = batteryModule.exec("echo Hello");
    EXPECT_EQ(result, "Hello\n");
}

/* Main function to run all tests */
int main(int argc, char **argv)
{
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}