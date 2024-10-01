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

    static void writeDTC(std::unordered_map<uint16_t, std::vector<uint8_t>>& did_values, const std::string& filePath, const uint16_t& did, int min_value, int max_value, const std::string& DTCToWrite);

    static bool getEcuPath(uint8_t ecu_id, std::string& ecu_path, uint8_t param, Logger& rc_logger, const std::string& version = "");
};

#endif