#include <gtest/gtest.h>
#include "../include/BatteryModule.h"

// Test default constructor
TEST(BatteryModuleTest, DefaultConstructor) {
    BatteryModule batteryModule;
    EXPECT_EQ(batteryModule.getEnergy(), 0.0);
    EXPECT_EQ(batteryModule.getVoltage(), 0.0);
    EXPECT_EQ(batteryModule.getPercentage(), 0.0);
    EXPECT_FALSE(batteryModule.isRunning());
}

// Test parameterized constructor
TEST(BatteryModuleTest, ParameterizedConstructor) {
    int interfaceNumber = 1;
    int moduleId = 0x102;
    BatteryModule batteryModule(interfaceNumber, moduleId);
    EXPECT_EQ(batteryModule.getEnergy(), 0.0);
    EXPECT_EQ(batteryModule.getVoltage(), 0.0);
    EXPECT_EQ(batteryModule.getPercentage(), 0.0);
    EXPECT_FALSE(batteryModule.isRunning());
}

// Test start and stop simulation
TEST(BatteryModuleTest, SimulationStartStop) {
    BatteryModule batteryModule;
    batteryModule.simulate();
    std::this_thread::sleep_for(std::chrono::seconds(1)); // Allow some time for the thread to start
    EXPECT_TRUE(batteryModule.isRunning());
    batteryModule.stopBatteryModule();
    EXPECT_FALSE(batteryModule.isRunning());
}

// Mock the exec function to test fetchBatteryData
class BatteryModuleMock : public BatteryModule {
public:
    std::string exec(const char *cmd) override {
        return "energy: 50.0\nvoltage: 12.5\npercentage: 80.0\nstate: Discharging\n";
    }
};

// Test fetchBatteryData
TEST(BatteryModuleTest, FetchBatteryData) {
    BatteryModuleMock batteryModule;
    batteryModule.fetchBatteryData();
    EXPECT_EQ(batteryModule.getEnergy(), 50.0);
    EXPECT_EQ(batteryModule.getVoltage(), 12.5);
    EXPECT_EQ(batteryModule.getPercentage(), 80.0);
}

/* To Do : make this work.. */
/* Test receiveFrames (without Google Mock, we assume a simple implementation) */
TEST(BatteryModuleTest, ReceiveFrames) {
    /* BatteryModule batteryModule; */
    /** Need a way to simulate receiving frames here; 
      * since we don't have gmock, we can't easily mock `ReceiveFrames`.
      * This test case is a placeholder to show where such a test would go. */
    /* batteryModule.receiveFrames(); */
}

// Test getter functions
TEST(BatteryModuleTest, GetterFunctions) {
    BatteryModule batteryModule;
    EXPECT_EQ(batteryModule.getEnergy(), 0.0);
    EXPECT_EQ(batteryModule.getVoltage(), 0.0);
    EXPECT_EQ(batteryModule.getPercentage(), 0.0);
}

int main(int argc, char **argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
