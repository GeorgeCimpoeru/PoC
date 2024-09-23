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

/* Function to check if the string exists on any line in the file */
bool FileManager::containsStringInFile(const std::string& filePath, const std::string& searchString)
{
    /* Open the file for reading */
    std::ifstream file(filePath);
    if (!file.is_open())
    {
        std::cerr << "Failed to open the file: " << filePath << std::endl;
        return false;
    }

    std::string line;
    /* Read each line of the file */
    while (std::getline(file, line))
    {
        /* Check if the string is found on the current line */
        if (line.find(searchString) != std::string::npos)
        {
            return true;
        }
    }

    file.close();
    return false;
}

/* Function to write a string to the file on a new line */
void FileManager::writeStringToFile(const std::string& filePath, const std::string& strToWrite)
{
    std::ofstream file(filePath, std::ios_base::app);
    if (!file.is_open())
    {
        std::cerr << "Failed to open the file: " << filePath << std::endl;
        return;
    }

    /* Write the string to a new line in the file */
    file << strToWrite << std::endl;
    file.close();
}

void FileManager::writeDTC(std::unordered_map<uint16_t, std::vector<uint8_t>>& did_values, const std::string& filePath, const uint16_t& did, int min_value, int max_value, const std::string& DTCToWrite)
{
    /* Check if DID exists in the map */
    if (did_values.find(did) != did_values.end())
    {
        /* Access the first byte from the vector associated with DID 0x012C */
        uint8_t value = did_values[did][0];

        /* Compare the value with the min and max values */
        if (value < min_value || value > max_value)
        {
            /* Check if the DTC its already active */
            if(!FileManager::containsStringInFile(filePath, DTCToWrite))
            {
                /* Write the DTC in the file */
                FileManager::writeStringToFile(filePath, DTCToWrite);
            }
        }
    }
    else
    {
        std::cout << "DID not found in the map." << std::endl;
    }
}

