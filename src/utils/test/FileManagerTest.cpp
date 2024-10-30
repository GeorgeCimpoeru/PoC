/* To successfully run these tests, ensure the following prerequisites:
 * - Create an "output" folder in the src/mcu directory.
 * - Place the "ECU_BATTERY_SW_VERSION_2.3.zip" file in the same directory (src/mcu).
 */

#include <gtest/gtest.h>
#include "../include/FileManager.h"
#include <filesystem>
#include <cstdio>

class FileManagerTest : public testing::Test
{
protected:
    FileManager fileManager;
    Logger logger;
    const std::string testFile = "test_map.txt";
    const std::string testDTCFile = "test_dtc.txt";

    void SetUp() override
    {
        /* Clean up before each test */
        std::remove(testFile.c_str());
        std::remove(testDTCFile.c_str());
    }

    void TearDown() override
    {
        /* Clean up after each test */
        std::remove(testFile.c_str());
        std::remove(testDTCFile.c_str());
    }
};

TEST_F(FileManagerTest, WriteMapToFile_FileOpenError)
{
    std::string invalid_file_name = "/invalid_path/invalid_file.txt";
    std::unordered_map<uint16_t, std::vector<uint8_t>> data_map = {
        {0x01, {0xAA, 0xBB, 0xCC}},
        {0x02, {0xDD, 0xEE}}
    };

    EXPECT_THROW({
        fileManager.writeMapToFile(invalid_file_name, data_map);
    }, std::runtime_error);
}

TEST_F(FileManagerTest, ReadMapFromFile_FileOpenError)
{
    std::string invalid_file_name = "/invalid_path/invalid_file.txt";

    EXPECT_THROW({
        fileManager.readMapFromFile(invalid_file_name);
    }, std::runtime_error);
}


TEST_F(FileManagerTest, WriteAndReadMapToFile)
{
    std::unordered_map<uint16_t, std::vector<uint8_t>> data_map = {
        {0x012C, {0x01, 0x02, 0x03}},
        {0x0456, {0x04, 0x05, 0x06}}
    };

    /* Write to file */
    fileManager.writeMapToFile(testFile, data_map);

    /* Read from file */
    auto read_map = fileManager.readMapFromFile(testFile);

    EXPECT_EQ(data_map.size(), read_map.size());
    for (const auto& [key, value] : data_map) {
        EXPECT_EQ(value, read_map[key]);
    }
}

TEST_F(FileManagerTest, ContainsStringInFile_FileOpenError)
{
    std::string filePath = "invalid/path/to/file.txt";
    std::string strToWrite = "test string";

    testing::internal::CaptureStderr();
    fileManager.writeStringToFile(filePath, strToWrite);
    std::string output = testing::internal::GetCapturedStderr();

    EXPECT_NE(output.find("Failed to open the file: " + filePath), std::string::npos);
}


TEST_F(FileManagerTest, ContainsStringInFile)
{
    fileManager.writeStringToFile(testDTCFile, "DTC: 1234");

    EXPECT_TRUE(fileManager.containsStringInFile(testDTCFile, "DTC: 1234"));
    EXPECT_FALSE(fileManager.containsStringInFile(testDTCFile, "DTC: 5678"));
}

TEST_F(FileManagerTest, WriteStringToFile) {
    fileManager.writeStringToFile(testDTCFile, "Error: 0x01");

    EXPECT_TRUE(fileManager.containsStringInFile(testDTCFile, "Error: 0x01"));
}

TEST_F(FileManagerTest, WriteStringToFile_FileOpenError)
{
    std::string invalid_file_name = "/invalid_path/invalid_file.txt";

    EXPECT_THROW({
        fileManager.readMapFromFile(invalid_file_name);
    }, std::runtime_error);
}

TEST_F(FileManagerTest, WriteDTC_DIDNotFound)
{
    std::unordered_map<uint16_t, std::vector<uint8_t>> did_values;
    std::string filePath = "test_dtc.txt";
    std::string DTCToWrite = "DTC123";

    testing::internal::CaptureStdout();
    fileManager.writeDTC(did_values, filePath, 0x1234, 10, 100, DTCToWrite);
    std::string output = testing::internal::GetCapturedStdout();

    EXPECT_NE(output.find("DID not found in the map."), std::string::npos);
}

TEST_F(FileManagerTest, WriteDTCWhenValueOutOfBounds)
{
    std::unordered_map<uint16_t, std::vector<uint8_t>> did_values = {
        /* Value less than min_value */
        {0x012C, {0x00}}
    };

    fileManager.writeDTC(did_values, testDTCFile, 0x012C, 1, 2, "DTC: 1234");
    EXPECT_TRUE(fileManager.containsStringInFile(testDTCFile, "DTC: 1234"));
}

TEST_F(FileManagerTest, WriteDTCWhenValueInBounds)
{
    std::unordered_map<uint16_t, std::vector<uint8_t>> did_values = {
        /* Value within bounds */
        {0x012C, {0x02}}
    };

    fileManager.writeDTC(did_values, testDTCFile, 0x012C, 1, 2, "DTC: 1234");
    EXPECT_FALSE(fileManager.containsStringInFile(testDTCFile, "DTC: 1234"));
}

TEST_F(FileManagerTest, GetEcuPathInvalidParam)
{
    std::string ecu_path;
    EXPECT_FALSE(fileManager.getEcuPath(0x10, ecu_path, 4, logger, "1.0"));
}

TEST_F(FileManagerTest, GetEcuPathMCU0Param)
{
    std::string ecu_path;
    std::string corret_path = std::string(PROJECT_PATH) + "/MCU_SW_VERSION_1.0.zip";
    EXPECT_TRUE(fileManager.getEcuPath(0x10, ecu_path, 0, logger, "1.0"));
    EXPECT_EQ(ecu_path, corret_path);
}

TEST_F(FileManagerTest, GetEcuPathMCU1Param)
{
    std::string ecu_path;
    std::string corret_path = std::string(PROJECT_PATH) + "/main_mcu_new.zip";
    EXPECT_TRUE(fileManager.getEcuPath(0x10, ecu_path, 1, logger, "1.0"));
    EXPECT_EQ(ecu_path, corret_path);
}

TEST_F(FileManagerTest, GetEcuPathBattery0Param)
{
    std::string ecu_path;
    std::string correct_path = std::string(PROJECT_PATH) + "/ECU_BATTERY_SW_VERSION_1.0.zip";
    EXPECT_TRUE(fileManager.getEcuPath(0x11, ecu_path, 0, logger, "1.0"));
    EXPECT_EQ(ecu_path, correct_path);
}

TEST_F(FileManagerTest, GetEcuPathBattery1Param)
{
    std::string ecu_path;
    std::string correct_path = std::string(PROJECT_PATH) + "/main_battery_new.zip";
    EXPECT_TRUE(fileManager.getEcuPath(0x11, ecu_path, 1, logger, "1.0"));
    EXPECT_EQ(ecu_path, correct_path);
}

TEST_F(FileManagerTest, GetEcuPathEngine0Param)
{
    std::string ecu_path;
    std::string correct_path = std::string(PROJECT_PATH) + "/ECU_ENGINE_SW_VERSION_1.0.zip";
    EXPECT_TRUE(fileManager.getEcuPath(0x12, ecu_path, 0, logger, "1.0"));
    EXPECT_EQ(ecu_path, correct_path);
}

TEST_F(FileManagerTest, GetEcuPathEngine1Param)
{
    std::string ecu_path;
    std::string correct_path = std::string(PROJECT_PATH) + "/main_engine_new.zip";
    EXPECT_TRUE(fileManager.getEcuPath(0x12, ecu_path, 1, logger, "1.0"));
    EXPECT_EQ(ecu_path, correct_path);
}

TEST_F(FileManagerTest, GetEcuPathDoors0Param)
{
    std::string ecu_path;
    std::string correct_path = std::string(PROJECT_PATH) + "/ECU_DOORS_SW_VERSION_1.0.zip";
    EXPECT_TRUE(fileManager.getEcuPath(0x13, ecu_path, 0, logger, "1.0"));
    EXPECT_EQ(ecu_path, correct_path);
}

TEST_F(FileManagerTest, GetEcuPathDoors1Param)
{
    std::string ecu_path;
    std::string correct_path = std::string(PROJECT_PATH) + "/main_doors_new.zip";
    EXPECT_TRUE(fileManager.getEcuPath(0x13, ecu_path, 1, logger, "1.0"));
    EXPECT_EQ(ecu_path, correct_path);
}

TEST_F(FileManagerTest, GetEcuPathHVAC0Param)
{
    std::string ecu_path;
    std::string correct_path = std::string(PROJECT_PATH) + "/ECU_HVAC_SW_VERSION_1.0.zip";
    EXPECT_TRUE(fileManager.getEcuPath(0x14, ecu_path, 0, logger, "1.0"));
    EXPECT_EQ(ecu_path, correct_path);
}

TEST_F(FileManagerTest, GetEcuPathHVAC1Param)
{
    std::string ecu_path;
    std::string correct_path = std::string(PROJECT_PATH) + "/main_hvac_new.zip";
    EXPECT_TRUE(fileManager.getEcuPath(0x14, ecu_path, 1, logger, "1.0"));
    EXPECT_EQ(ecu_path, correct_path);
}

TEST_F(FileManagerTest, WrongECU)
{
    std::string ecu_path;
    std::string correct_path = std::string(PROJECT_PATH) + "/wrong_ecu.zip";
    EXPECT_FALSE(fileManager.getEcuPath(0x15, ecu_path, 1, logger, "1.0"));
}

TEST_F(FileManagerTest, ValidateData)
{
    /* Check ELF signature validation */
    std::vector<uint8_t> valid_data = {0x7F, 0x45, 0x4C, 0x46};
    /* ELF_SIGNATURE */
    EXPECT_TRUE(fileManager.validateData(valid_data, FileType::ELF_FILE));

    std::vector<uint8_t> invalid_data = {0x00, 0x00, 0x00, 0x00};
    /* Not ELF_SIGNATURE */
    EXPECT_FALSE(fileManager.validateData(invalid_data, FileType::ELF_FILE));
}

TEST_F(FileManagerTest, ValidateData_ZipFileValidSignature)
{
    /* Test ZIP file validation with correct signature */
    std::vector<uint8_t> data = { (ZIP_SIGNATURE >> 24) & 0xFF, (ZIP_SIGNATURE >> 16) & 0xFF, 
                                  (ZIP_SIGNATURE >> 8) & 0xFF, ZIP_SIGNATURE & 0xFF };
    EXPECT_TRUE(fileManager.validateData(data, FileType::ZIP_FILE));
}

TEST_F(FileManagerTest, ValidateData_ZipFileInvalidSignature)
{
    /* Test ZIP file validation with incorrect signature */
    std::vector<uint8_t> data = { 0x00, 0x00, 0x00, 0x00 };  // Semnătură incorectă
    EXPECT_FALSE(fileManager.validateData(data, FileType::ZIP_FILE));
}

TEST_F(FileManagerTest, ValidateData_DefaultCaseInvalidFileType)
{
    std::vector<uint8_t> data = { 0x7F, 0x45, 0x4C, 0x46 };
    /* Invalid type file */
    EXPECT_FALSE(fileManager.validateData(data, static_cast<FileType>(-1)));
}

TEST_F(FileManagerTest, ExtractZipFileWrongZipPath)
{
    std::string zipFilePath = "invalid/path/test.zip";
    std::string outputDir = "./output";

    EXPECT_FALSE(fileManager.extractZipFile(0x10, zipFilePath, outputDir, logger));
}

TEST_F(FileManagerTest, ExtractZipFileWrongFolderPath)
{
    std::string zipFilePath = "ECU_BATTERY_SW_VERSION_2.3.zip";
    std::string outputDir = "./wrongFolder";

    EXPECT_FALSE(fileManager.extractZipFile(0x10, zipFilePath, outputDir, logger));
}

TEST_F(FileManagerTest, ExtractZipFile)
{
    std::string zipFilePath = "ECU_BATTERY_SW_VERSION_2.3.zip";
    std::string outputDir = "./output";

    EXPECT_TRUE(fileManager.extractZipFile(0x10, zipFilePath, outputDir, logger));
}

int main(int argc, char* argv[]) {
    testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}