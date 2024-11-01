/**
 * @file DoorsModuleTest.cpp
 * @author Theodor Stoica
 * @brief Unit test for Doors Module
 * @version 0.1
 * @date 2024-10-24
 */

#include <gtest/gtest.h>
#include "../include/DoorsModule.h"

bool containsLine(const std::string& output, const std::string& line)
{
    return output.find(line) != std::string::npos;
}

struct DoorsModuleTest : testing::Test
{
    DoorsModule* doors;
    DoorsModuleTest()
    {
        doors = new DoorsModule();
    }
    ~DoorsModuleTest()
    {
        delete doors;
    }
};

TEST_F(DoorsModuleTest, DoorsSocket)
{
    EXPECT_NO_THROW({
        doors->getDoorsSocket();
    });
}

TEST_F(DoorsModuleTest, CheckOldDataFile)
{
    std::ofstream outfile("old_doors_data.txt");
    doors->writeDataToFile();
    outfile.close();
}

TEST_F(DoorsModuleTest, DoorsDataFailed)
{
    std::string path = "doors_data.txt";
    std::ofstream outfile(path);
    chmod(path.c_str(), 0);
    EXPECT_THROW(
    {
        doors->writeDataToFile();
    }, std::runtime_error);
    path = "doors_data.txt";
    chmod(path.c_str(), 0666);
    outfile.close();
}

/* Main function to run all tests */
int main(int argc, char **argv)
{
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}