#include "../include/FileManager.h"

void FileManager::writeMapToFile(const std::string& file_name, const std::unordered_map<uint16_t, std::vector<uint8_t>>& data_map)
{
    /* Open in truncate mode to overwrite the file */
    std::ofstream outfile(file_name, std::ios::trunc);

    if (!outfile.is_open())
    {
        throw std::runtime_error("Failed to open file: " + file_name);
    }

    for (const auto& [data_identifier, data] : data_map)
    {
        outfile << std::hex << std::setw(4) << std::setfill('0') << std::uppercase << data_identifier << " ";
        for (uint8_t byte : data)
        {
            outfile << std::hex << std::setw(2) << std::setfill('0') << std::uppercase << static_cast<int>(byte) << " ";
        }
        outfile << "\n";
    }

    outfile.close();
}

std::unordered_map<uint16_t, std::vector<uint8_t>> FileManager::readMapFromFile(const std::string& file_name)
{
    std::unordered_map<uint16_t, std::vector<uint8_t>> data_map;
    std::ifstream infile(file_name);
    if (!infile.is_open())
    {
        throw std::runtime_error("Failed to open file: " + file_name);
    }

    std::string line;
    while (std::getline(infile, line))
    {
        std::istringstream iss(line);
        uint16_t data_identifier;
        iss >> std::hex >> data_identifier;

        std::vector<uint8_t> data;
        int byte;
        while (iss >> std::hex >> byte)
        {
            data.push_back(static_cast<uint8_t>(byte));
        }
        data_map[data_identifier] = data;
    }

    infile.close();
    return data_map;
}

