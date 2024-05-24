#include <gtest/gtest.h>
#include "../include/BatteryModule.h"

// Test the default constructor
TEST(BatteryModuleTest, DefaultConstructor)
{
    BatteryModule battery;

    EXPECT_EQ(battery.getVoltage(), 12.5);
    EXPECT_EQ(battery.getCurrent(), 5.0);
    EXPECT_EQ(battery.getTemperature(), 20.0);
}

// Test the custom constructor
TEST(BatteryModuleTest, CustomConstructor)
{
    BatteryModule battery(1, 0x102);

    EXPECT_EQ(battery.getVoltage(), 12.5);
    EXPECT_EQ(battery.getCurrent(), 5.0);
    EXPECT_EQ(battery.getTemperature(), 20.0);
}

// The following tests are targeting some private methods of the class
// this has to be re-vised in future...
/*
// Test the updateParameters method
TEST(BatteryModuleTest, UpdateParameters)
{
    BatteryModule battery;

    battery.updateParamenters();
    EXPECT_NEAR(battery.getVoltage(), 12.51, 0.001);
    EXPECT_NEAR(battery.getCurrent(), 5.01, 0.001);
    EXPECT_NEAR(battery.getTemperature(), 20.1, 0.001);
}

// Test the simulate and stopBatteryModule methods
TEST(BatteryModuleTest, SimulateAndStop)
{
    BatteryModule battery;

    battery.simulate();
    std::this_thread::sleep_for(std::chrono::seconds(3)); // Let it run for a bit
    battery.stopBatteryModule();

    // Ensure the parameters have been updated
    EXPECT_NE(battery.getVoltage(), 12.5);
    EXPECT_NE(battery.getCurrent(), 5.0);
    EXPECT_NE(battery.getTemperature(), 20.0);
}*/