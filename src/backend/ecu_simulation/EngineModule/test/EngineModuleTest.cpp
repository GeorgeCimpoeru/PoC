#include <gtest/gtest.h>
#include "../include/EngineModule.h"

bool containsLine(const std::string& output, const std::string& line)
{
    return output.find(line) != std::string::npos;
}


struct EngineModuleTest : testing::Test
{
    EngineModule* engine;
    EngineModuleTest()
    {
        engine = new EngineModule();
    }
    ~EngineModuleTest()
    {
        delete engine;
    }
};

TEST_F(EngineModuleTest, EngineSocket)
{
    EXPECT_NO_THROW({
        engine->getEngineSocket();
    });
}

TEST_F(EngineModuleTest, CheckOldDataFile)
{
    std::ofstream outfile("old_engine_data.txt");
    engine->writeDataToFile();
    outfile.close();
}

 TEST_F(EngineModuleTest, checkDTCLogError)
{
    std::string output = "";
    const std::string expected_out = 
        "Failed to create dtcs.txt file.";
    std::string path = std::string(PROJECT_PATH) + "/src/ecu_simulation/EngineModule/dtcs.txt";
    std::ifstream infile(path);
    chmod(path.c_str(), 0);
    testing::internal::CaptureStdout();
    engine->checkDTC();
    output = testing::internal::GetCapturedStdout();
    infile.close();
    EXPECT_TRUE(containsLine(output, expected_out));
}

TEST_F(EngineModuleTest, checkDTCCreateFileSuccessfully)
{
    std::string path = std::string(PROJECT_PATH) + "/src/ecu_simulation/EngineModule/dtcs.txt";
    remove(path.c_str());

    engine->checkDTC();

    std::ifstream infile(path);
    EXPECT_TRUE(infile.is_open()) << "dtcs.txt file should be created successfully.";
    infile.close();
}

TEST_F(EngineModuleTest, BatteryDataFailed)
{
    std::string path = "engine_data.txt";
    std::ofstream outfile(path);
    chmod(path.c_str(), 0);
    EXPECT_THROW(
    {
        engine->writeDataToFile();
    }, std::runtime_error);
    path = "engine_data.txt";
    chmod(path.c_str(), 0666);
    outfile.close();
}


/* Main function to run all tests */
int main(int argc, char **argv)
{
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}