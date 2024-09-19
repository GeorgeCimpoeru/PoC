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
};

#endif