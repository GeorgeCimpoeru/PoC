#include <gtest/gtest.h>
#include "../include/BatteryModule.h"

bool containsLine(const std::string& output, const std::string& line)
{
    return output.find(line) != std::string::npos;
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
    result = oss.str();
    std::string percentage_fetch = std::to_string(static_cast<uint8_t>(battery->getPercentage()));
    /* EXPECT_EQ(result, percentage_fetch); */
}

TEST_F(BatteryModuleTest, GetLinuxBatteryStateNoThrow)
{
    EXPECT_NO_THROW({
        std::string state = battery->getLinuxBatteryState();
    });
}

TEST_F(BatteryModuleTest, GetBatterySocket)
{
    EXPECT_NO_THROW({
        battery->getBatterySocket();
    });
}

TEST_F(BatteryModuleTest, CheckEmptyState)
{
    std::string data = "energy: 50\nvoltage: 12.5\npercentage: 75\nstate: empty";
    float energy = 0;
    float voltage = 0;
    float percentage = 0;
    std::string state;
    battery->parseBatteryInfo(data, energy, voltage, percentage, state);
    EXPECT_EQ(state, "empty");
}

TEST_F(BatteryModuleTest, CheckDischargingState)
{
    std::string data = "energy: 50\nvoltage: 12.5\npercentage: 75\nstate: discharging";
    float energy = 0;
    float voltage = 0;
    float percentage = 0;
    std::string state;
    battery->parseBatteryInfo(data, energy, voltage, percentage, state);
    EXPECT_EQ(state, "discharging");
}

TEST_F(BatteryModuleTest, CheckChargingState)
{
    std::string data = "energy: 50\nvoltage: 12.5\npercentage: 75\nstate: charging";
    float energy = 0;
    float voltage = 0;
    float percentage = 0;
    std::string state;
    battery->parseBatteryInfo(data, energy, voltage, percentage, state);
    EXPECT_EQ(state, "charging");
}

TEST_F(BatteryModuleTest, CheckFullyChargedState)
{
    std::string data = "energy: 50\nvoltage: 12.5\npercentage: 75\nstate: fully-charged";
    float energy = 0;
    float voltage = 0;
    float percentage = 0;
    std::string state;
    battery->parseBatteryInfo(data, energy, voltage, percentage, state);
    EXPECT_EQ(state, "fully-charged");
}

TEST_F(BatteryModuleTest, CheckPendingChargeState)
{
    std::string data = "energy: 50\nvoltage: 12.5\npercentage: 75\nstate: pending-charge";
    float energy = 0;
    float voltage = 0;
    float percentage = 0;
    std::string state;
    battery->parseBatteryInfo(data, energy, voltage, percentage, state);
    EXPECT_EQ(state, "pending-charge");
}

TEST_F(BatteryModuleTest, CheckPendingDischargeState)
{
    std::string data = "energy: 50\nvoltage: 12.5\npercentage: 75\nstate: pending-discharge";
    float energy = 0;
    float voltage = 0;
    float percentage = 0;
    std::string state;
    battery->parseBatteryInfo(data, energy, voltage, percentage, state);
    EXPECT_EQ(state, "pending-discharge");
}

TEST_F(BatteryModuleTest, CheckOldDataFile)
{
    std::ofstream outfile("old_battery_data.txt");
    battery->writeDataToFile();
    outfile.close();
}

TEST_F(BatteryModuleTest, checkDTCLogError)
{
    std::string output = "";
    const std::string expected_out = 
        "Failed to create dtcs.txt file.";
    std::string path = std::string(PROJECT_PATH) + "/src/ecu_simulation/BatteryModule/dtcs.txt";
    std::ifstream infile(path);
    chmod(path.c_str(), 0);
    testing::internal::CaptureStdout();
    battery->checkDTC();
    output = testing::internal::GetCapturedStdout();
    infile.close();
    EXPECT_TRUE(containsLine(output, expected_out));
}

TEST_F(BatteryModuleTest, checkDTCCreateFileSuccessfully)
{
    std::string path = std::string(PROJECT_PATH) + "/src/ecu_simulation/BatteryModule/dtcs.txt";
    remove(path.c_str());

    battery->checkDTC();

    std::ifstream infile(path);
    EXPECT_TRUE(infile.is_open()) << "dtcs.txt file should be created successfully.";
    infile.close();
}
TEST_F(BatteryModuleTest, BatteryDataFailed)
{
    std::string path = "battery_data.txt";
    std::ofstream outfile(path);
    chmod(path.c_str(), 0);
    EXPECT_THROW(
    {
        battery->writeDataToFile();
    }, std::runtime_error);
    path = "battery_data.txt";
    chmod(path.c_str(), 0666);
    outfile.close();
}

TEST_F(BatteryModuleTest, FetchException)
{
    battery->fetchBatteryData("wrong");
}

/* Main function to run all tests */
int main(int argc, char **argv)
{
    testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}