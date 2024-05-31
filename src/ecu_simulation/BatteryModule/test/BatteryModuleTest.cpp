#include <gtest/gtest.h>
#include "../include/BatteryModule.h"

/* Test fixture for BatteryModule */
class BatteryModuleTest : public ::testing::Test {
protected:
    /* This function runs before each test */
    void SetUp() override {
        batteryDefault = new BatteryModule();
        batteryParam = new BatteryModule(10, 0x202);
    }

    /* This function runs after each test */
    void TearDown() override {
        delete batteryDefault;
        delete batteryParam;
    }

    BatteryModule* batteryDefault;
    BatteryModule* batteryParam;
};

/* Test for default constructor */
TEST_F(BatteryModuleTest, DefaultConstructor) {
    EXPECT_EQ(batteryDefault->getVoltage(), 12.5);
    EXPECT_EQ(batteryDefault->getCurrent(), 5.0);
    EXPECT_EQ(batteryDefault->getTemperature(), 20.0);
}

/* Test for parameterized constructor */
TEST_F(BatteryModuleTest, ParameterizedConstructor) {
    /* Check if the values are correctly set */
    EXPECT_EQ(batteryParam->getVoltage(), 12.5);
    EXPECT_EQ(batteryParam->getCurrent(), 5.0);
    EXPECT_EQ(batteryParam->getTemperature(), 20.0);
}

/* Test updateParamenters function */
TEST_F(BatteryModuleTest, UpdateParameters) {
    /* Initial values */
    float initialVoltage = batteryDefault->getVoltage();
    float initialCurrent = batteryDefault->getCurrent();
    float initialTemperature = batteryDefault->getTemperature();

    /* Update parameters */
    batteryDefault->updateParamenters();

    /* Check if the values are updated correctly */
    EXPECT_FLOAT_EQ(batteryDefault->getVoltage(), initialVoltage + BATTERY_MODULE_PARAM_INCREMENT);
    EXPECT_FLOAT_EQ(batteryDefault->getCurrent(), initialCurrent + BATTERY_MODULE_PARAM_INCREMENT);
    EXPECT_FLOAT_EQ(batteryDefault->getTemperature(), initialTemperature + BATTERY_MODULE_PARAM_INCREMENT);
}