/**
 * @file HVACModuleTest.cpp
 * @author Theodor Stoica
 * @brief Unit test for HVAC Module
 * @version 0.1
 * @date 2024-10-24
 */

#include <gtest/gtest.h>
#include "../include/HVACModule.h"

bool containsLine(const std::string& output, const std::string& line)
{
    return output.find(line) != std::string::npos;
}

struct HVACModuleTest : testing::Test
{
    HVACModule* hvac;
    HVACModuleTest()
    {
        hvac = new HVACModule();
    }
    ~HVACModuleTest()
    {
        delete hvac;
    }
};

TEST_F(HVACModuleTest, HVACSocket)
{
    EXPECT_NO_THROW({
        hvac->getSocket();
    });
}

TEST_F(HVACModuleTest, CheckOldDataFile)
{
    std::ofstream outfile("old_hvac_data.txt");
    hvac->writeDataToFile();
    outfile.close();
}

TEST_F(HVACModuleTest, HVACDataFailed)
{
    std::string path = "hvac_data.txt";
    std::ofstream outfile(path);
    chmod(path.c_str(), 0);
    EXPECT_THROW(
    {
        hvac->writeDataToFile();
    }, std::runtime_error);
    path = "hvac_data.txt";
    chmod(path.c_str(), 0666);
    outfile.close();
}

TEST_F(HVACModuleTest, HVACInfo)
{
    EXPECT_NO_THROW({
        hvac->printHvacInfo();
    });
}

/* Main function to run all tests */
int main(int argc, char **argv)
{
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}