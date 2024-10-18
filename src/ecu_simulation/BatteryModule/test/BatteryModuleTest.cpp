#include <gtest/gtest.h>
#include "../include/BatteryModule.h"

std::string hexToString(uint8_t hexValue)
{
    std::stringstream ss;
    ss << std::hex << hexValue;
    return ss.str();
}

struct BatteryModuleTest : testing::Test
{
    BatteryModule* battery;
    BatteryModuleTest()
    {
        battery = new BatteryModule();
    }
    ~BatteryModuleTest()
    {
        delete battery;
    }
};

TEST_F(BatteryModuleTest, ExecFunctionThrow)
{
    EXPECT_THROW(battery->exec("ls","wrongmode"), std::runtime_error);
}

TEST_F(BatteryModuleTest, ParseBatteryInfo)
{
    auto data_map = FileManager::readMapFromFile("battery_data.txt");

        for (const auto& [key, value] : data_map) {
        // Print the key
        std::cout << "Key: " << std::hex << std::setw(4) << std::setfill('0') << key << " | Values: ";
        
        // Print the vector values
        for (const auto& byte : value) {
            std::cout << std::hex << std::setw(2) << std::setfill('0') << static_cast<int>(byte) << " ";
        }
        
        std::cout << std::endl; // Move to the next line
    }
    std::vector<uint8_t> response = data_map[0x01A0];

    std::ostringstream oss;
    for (uint8_t byte : response)
    {
        oss << std::hex << std::setw(2) << std::setfill('0') << static_cast<int>(byte);
    }
    std::string result = oss.str();
    std::string energy_fetch = std::to_string(static_cast<uint8_t>(battery->getEnergy()));
    EXPECT_EQ(result, energy_fetch);

    oss.str("");
    oss.clear();
    response = data_map[0x01B0];
    for (uint8_t byte : response)
    {
        oss << std::hex << std::setw(2) << std::setfill('0') << static_cast<int>(byte);
    }
    result = oss.str();
    std::string voltage_fetch = std::to_string(static_cast<uint8_t>(battery->getVoltage()));
    EXPECT_EQ(result, voltage_fetch);


    oss.str("");
    oss.clear();
    response = data_map[0x01C0];
    for (uint8_t byte : response)
    {
        oss << std::hex << std::setw(2) << std::setfill('0') << static_cast<int>(byte);
    }
    std::cerr << response.size() << "PULA" << response[0];
    result = oss.str();
    std::string percentage_fetch = std::to_string(static_cast<uint8_t>(battery->getPercentage()));
    /* EXPECT_EQ(result, percentage_fetch); */
}

/* Main function to run all tests */
int main(int argc, char **argv)
{
    testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}