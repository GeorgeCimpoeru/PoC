/**
 * @file FileManager.h
 * @author Iancu Daniel
 * @brief Utility class for file management
 * @version 0.1
 * @date 2024-09-19
 * 
 * @copyright Copyright (c) 2024
 * 
 */

#ifndef FILEMANAGER_H
#define FILEMANAGER_H

#include <iostream>
#include <cstdint>
#include <vector>
#include <unordered_map>
#include <fstream>
#include <iomanip>
#include <string>
#include "Logger.h"
#include "GenerateFrames.h"

#define ELF_SIGNATURE 0x7F454C46
#define ZIP_SIGNATURE 0x504B0304   

#define DATA_SIGNATURE_SIZE 0x04

enum class FileType
{
    ELF_FILE = 0,
    ZIP_FILE,
    UNKNOWN
};
class FileManager
{
public:
    /**
     * @brief Method used for writing a map to a file
     * 
     * @param file_name 
     * @param data_map 
     */
    static void writeMapToFile(const std::string& file_name, const std::unordered_map<uint16_t, std::vector<uint8_t>>& data_map);
    
    /**
     * @brief Method used for reading the map from a file
     * 
     * @param file_name 
     * @return std::unordered_map<uint16_t, std::vector<uint8_t>> 
     */
    static std::unordered_map<uint16_t, std::vector<uint8_t>> readMapFromFile(const std::string& file_name);

    /**
     * @brief Writes a string to a file on a new line.
     * @param filePath The path of the file where the string will be written.
     * @param strToWrite The string to be written to the file.
     */
    static void writeStringToFile(const std::string& filePath, const std::string& strToWrite);

    /**
     * @brief Checks if a string exists in the file.
     * @param filePath The path of the file to be searched.
     * @param searchString The string to search for in the file.
     * @return true if the string is found on any line in the file, false otherwise.
     */
    static bool containsStringInFile(const std::string& filePath, const std::string& searchString);

    /**
     * @brief 
     * 
     * @param did_values 
     * @param filePath 
     * @param did 
     * @param min_value 
     * @param max_value 
     * @param DTCToWrite 
     */
    static void writeDTC(std::unordered_map<uint16_t, std::vector<uint8_t>>& did_values, const std::string& filePath, const uint16_t& did, int min_value, int max_value, const std::string& DTCToWrite);
    
    /**
     * @brief Method used to get an ecu path(.zip, _new)
     * 
     * @param ecu_id 
     * @param ecu_path 
     * @param param 
     * @param rc_logger 
     * @param version 
     * @return true 
     * @return false 
     */
    static bool getEcuPath(uint8_t ecu_id, std::string& ecu_path, uint8_t param, Logger& rc_logger, const std::string& version = "");

    static bool validateData(std::vector<uint8_t>& file_data, FileType file_type);
    
    /**
     * @brief Method to extract the zipped file.
     * 
     * @param target_id Targeted ecu for file unzipping.
     * @param zipFilePath Path to zip file.
     * @param outputDir Path for the extracted file.
     */
    static bool extractZipFile(uint8_t target_id, const std::string &zipFilePath, const std::string &outputDir, Logger& logger);

    /**
    * @brief Set the Did Value object
    * 
    * @param did 
    * @param value 
    */
    static void setDidValue(const uint16_t did, const std::vector<uint8_t>& value, canid_t can_id, int socket, Logger& logger);

    /**
    * @brief Get the Did Value object
    * 
    * @param did 
    * @return std::vector<uint8_t> 
    */
    static const std::vector<uint8_t> getDidValue(const uint16_t did);
};

#endif