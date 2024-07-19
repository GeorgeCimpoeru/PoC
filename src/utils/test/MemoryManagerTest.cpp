/**
 * @file MemoryManagerTest.cpp
 * @author Mujei Ruben
 * @brief 
 * @version 0.1
 * @date 2024-07-18
 * 
 * @copyright Copyright (c) 2024
 * This test works if a sd simulated card exists in the /dev/loop21 of 300 Mb
 */
#include "../include/MemoryManager.h"

#include <gtest/gtest.h>

off_t address = 230481 * 512;
Logger* logger = new Logger("test","test_memory.log");

/* Create object for all tests */
struct MemoryManagerTest : testing::Test
{
    MemoryManager* memory;
    MemoryManagerTest()
    {
        memory = MemoryManager::getInstance(address,"/dev/loop21", logger);
    }
    ~MemoryManagerTest()
    {
        memory->resetInstance();
    }
};

TEST_F(MemoryManagerTest, VerifySingleton)
{
    MemoryManager* m_test = MemoryManager::getInstance();
    EXPECT_EQ(memory->getAddress(), m_test->getAddress());
    EXPECT_EQ(memory->getPath(), m_test->getPath());
}

TEST_F(MemoryManagerTest, VerifySetters) 
{
    off_t new_address = 230482 * 512;
    std::string new_path = "/new/path";
    memory->setAddress(new_address);
    memory->setPath(new_path);
    EXPECT_EQ(memory->getAddress(), new_address);
    EXPECT_EQ(memory->getPath(), new_path);
    /* set back the correct path */
    memory->setPath("/dev/loop21");
    memory->setAddress(address);
}

TEST_F(MemoryManagerTest, WriteReadInAddress) 
{
    std::vector<uint8_t> data = {97,98,99,100};
    memory->writeToAddress(data);
    std::vector<uint8_t> data_from_address = memory->readFromAddress(memory->getPath(),address,data.size());
    EXPECT_EQ(data, data_from_address);
}

TEST_F(MemoryManagerTest, WriteReadInFile) 
{
    std::vector<uint8_t> data = {97,98,99,100};
    memory->writeToFile(data, "test_memory.txt");
    std::vector<uint8_t> data_from_address = memory->readBinary("test_memory.txt");
    EXPECT_EQ(data, data_from_address);
}

/* Exception cases */

TEST_F(MemoryManagerTest, WriteInAddress_BootFail) 
{
    int new_address = 300;
    std::vector<uint8_t> data = {97,98,99,100};
    memory->setAddress(new_address);
    testing::internal::CaptureStdout();
    memory->writeToAddress(data);
    std::string output = testing::internal::GetCapturedStdout();
    EXPECT_NE(output.find("Error:Address in boot partition."), std::string::npos);
}

TEST_F(MemoryManagerTest, WriteInAddress_AddresFail) 
{
    off_t new_address = -1;
    std::vector<uint8_t> data = {97,98,99,100};
    memory->setAddress(new_address);
    testing::internal::CaptureStdout();
    memory->writeToAddress(data);
    std::string output = testing::internal::GetCapturedStdout();
    EXPECT_NE(output.find("Error: the address was not initialize correctly."), std::string::npos);
}

TEST_F(MemoryManagerTest, WriteInAddress_MemoryFail) 
{
    off_t new_address = 400000000;
    std::vector<uint8_t> data = {97,98,99,100};
    memory->setAddress(new_address);
    testing::internal::CaptureStdout();
    memory->writeToAddress(data);
    std::string output = testing::internal::GetCapturedStdout();
    EXPECT_NE(output.find("Error: Not enough memory."), std::string::npos);
}

int main(int argc, char* argv[])
{
    testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}