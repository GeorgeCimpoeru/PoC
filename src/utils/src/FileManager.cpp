#include "../include/FileManager.h"
#include <unistd.h>
#include <zip.h>

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

bool FileManager::getEcuPath(uint8_t ecu_id, std::string& ecu_path, uint8_t param, Logger& logger, const std::string& version)
{
    static std::string zip_ecu_path;
    if(param > 3)
    {
        /* Only 0, 1, 2 and 3 valid values */
        return 0;
    }

    if(param == 3 && access((zip_ecu_path).c_str(), F_OK) != -1)
    {
        ecu_path = zip_ecu_path;
        return 1;
    }

    switch(ecu_id)
    {
        case 0x10:
        {
            if(param == 0)
            {
                ecu_path = std::string(PROJECT_PATH) + "/MCU_SW_VERSION_" + version + ".zip";
                zip_ecu_path = ecu_path;
            }
            else
            {
                ecu_path = std::string(PROJECT_PATH) + ((param == 1) ? "/main_mcu_new.zip" : "/src/mcu/");
            }
            break;
        }
        case 0x11:
        {
            if(param == 0)
            {
                ecu_path = std::string(PROJECT_PATH) + "/ECU_BATTERY_SW_VERSION_" + version + ".zip";
                zip_ecu_path = ecu_path;
            }
            else
            {
                ecu_path = std::string(PROJECT_PATH) + ((param == 1) ? "/main_battery_new.zip" : "/src/ecu_simulation/BatteryModule/");
            }        
            break;
        }
        case 0x12:
        {
            if(param == 0)
            {
                ecu_path = std::string(PROJECT_PATH) + "/ECU_ENGINE_SW_VERSION_" + version + ".zip";
                zip_ecu_path = ecu_path;
            }
            else
            {
                ecu_path = std::string(PROJECT_PATH) + ((param == 1) ? "/main_engine_new.zip" : "/src/ecu_simulation/EngineModule/");
            }        
            break;
        }
        case 0x13:
        {
            if(param == 0)
            {
                ecu_path = std::string(PROJECT_PATH) + "/ECU_DOORS_SW_VERSION_" + version + ".zip";
                zip_ecu_path = ecu_path;
            }
            else
            {
                ecu_path = std::string(PROJECT_PATH) + ((param == 1) ? "/main_doors_new.zip" : "/src/ecu_simulation/DoorsModule/");
            }        
            break;
        }
        case 0x14:
        {
            if(param == 0)
            {
                ecu_path = std::string(PROJECT_PATH) + "/ECU_HVAC_SW_VERSION_" + version + ".zip";
                zip_ecu_path = ecu_path;
            }
            else
            {
                ecu_path = std::string(PROJECT_PATH) + ((param == 1) ? "/main_hvac_new.zip" : "/src/ecu_simulation/HVACModule/");
            }        
            break;
        }
        default:
            LOG_ERROR(logger.GET_LOGGER(), "No valid path for main_xxx_new.");
            return 0;
        break;
    }
    /* Checks for valid path here */
    // if(param == 1 && access((ecu_path).c_str(), F_OK) == -1)
    // {
    //     return 0;
    // }
    
    return 1;
}

bool FileManager::validateData(std::vector<uint8_t>& data, FileType file_type)
{   
    uint32_t file_signature = (data[0] << 24) | (data[1] << 16) | (data[2] << 8) | data[3];

    switch(file_type)
    {
        case FileType::ELF_FILE:
        {
            if((file_signature ^ ELF_SIGNATURE) != 0)
            {
                return 0;
            }
            break;
        }
        case FileType::ZIP_FILE:
        {
            if((file_signature ^ ZIP_SIGNATURE) != 0)
            {
                return 0;
            }
            break;
        }
        default:
        {
            return 0;
        }
    }
    return 1;
}

bool FileManager::extractZipFile(uint8_t target_id, const std::string &zipFilePath, const std::string &outputDir, Logger& logger) {
    int err = 0;
    zip *archive = zip_open(zipFilePath.c_str(), 0, &err);

    if (archive == nullptr) {
        LOG_ERROR(logger.GET_LOGGER(), "Error opening ZIP file:" + zipFilePath);
        return false;
    }

    zip_uint64_t numFiles = zip_get_num_entries(archive, 0);
    for (zip_uint64_t i = 0; i < numFiles; ++i) {
        const char *name = zip_get_name(archive, i, 0);
        if (name == nullptr) {
            LOG_ERROR(logger.GET_LOGGER(), "Error getting name of file #" + std::to_string(i) + " in ZIP archive.");
            zip_close(archive);
            return false;
        }

        struct zip_stat st;
        zip_stat_init(&st);
        zip_stat(archive, name, 0, &st);

        zip_file *zf = zip_fopen(archive, name, 0);
        if (!zf) {
            LOG_ERROR(logger.GET_LOGGER(), "Error opening file inside ZIP: " + std::string(name));
            zip_close(archive);
            return false;
        }

        std::string outputFilePath = outputDir + "/" + name + "_new";

        std::ofstream outFile(outputFilePath, std::ios::binary);
        if (!outFile.is_open()) {
            LOG_ERROR(logger.GET_LOGGER(), "Error creating output file: " + outputFilePath);
            zip_fclose(zf);
            zip_close(archive);
            return false;
        }

        char buffer[8192];
        zip_int64_t bytesRead;
        while ((bytesRead = zip_fread(zf, buffer, sizeof(buffer))) > 0) {
            outFile.write(buffer, bytesRead);
        }

        outFile.close();
        zip_fclose(zf);

        if (bytesRead < 0) {
            LOG_ERROR(logger.GET_LOGGER(), "Error reading from ZIP file: " + std::string(name));
            zip_close(archive);
            return false;
        }
    }

    zip_close(archive);
    return true;
}

void FileManager::setDidValue(const uint16_t did, const std::vector<uint8_t>& value, canid_t can_id, int socket, Logger& logger)
    {
        /* Create an instance of GenerateFrames with the CAN socket */
        GenerateFrames generate_frames = GenerateFrames(socket, logger);
        std::string file_path = std::string(PROJECT_PATH);
        /* Extract and switch sender and receiver */
        uint8_t receiver_id = can_id  & 0xFF;        
        uint8_t target_id = (can_id & 0x00FF0000) >> 16;
        
        if (target_id == 0)
        {
            target_id = receiver_id;
        }
        
        switch (receiver_id)
        {
            case 0x10:
            file_path += "/src/mcu/mcu_data.txt";           
            break;
        
            case 0x11:
            file_path += "/src/ecu_simulation/BatteryModule/battery_data.txt";
            break;

            case 0x12:
            file_path += "/src/ecu_simulation/EngineModule/engine_data.txt";
            break;
        
            case 0x13:
            file_path += "/src/ecu_simulation/DoorsModule/doors_data.txt";
            break;

            case 0x14:
            file_path += "/src/ecu_simulation/HVACModule/hvac_data.txt";
            break;
       
            default:
                LOG_ERROR(logger.GET_LOGGER(), "Module with id {:x} not supported.", receiver_id);
                break;
        }

        auto data_map = FileManager::readMapFromFile(file_path);
        auto did_it = data_map.find(did);

        if(did_it == data_map.end())
        {
            LOG_WARN(logger.GET_LOGGER(), "DID {} not found when trying to set value", did);
            return;
        }
        did_it->second = value;

        FileManager::writeMapToFile(file_path, data_map);
        
        canid_t sender_id = (0xFA << 8) + target_id; //fa11
        /* Create the can frame : PCI, SID, DIS MSB, DID LSB, Data */
        std::vector<uint8_t> wdbi_frame;
        wdbi_frame.push_back(0x03); /* PCI */
        wdbi_frame.push_back(0x2e); /* Service ID */
        wdbi_frame.push_back((did >> 8) && 0xFF); /* did 0X01*/
        wdbi_frame.push_back(did & 0xFF); /* did 0XE0*/
        wdbi_frame.insert(wdbi_frame.end(),value.begin(), value.end()); /* data to be written */

        /* Send the postive response frame */ 
        generate_frames.sendFrame(sender_id, wdbi_frame, socket, DATA_FRAME);
    }

    const std::vector<uint8_t> FileManager::getDidValue(const uint16_t did)
    {
        /* Should also contain validation */
        std::string file_path = std::string(PROJECT_PATH) + "/src/mcu/mcu_data.txt";
        auto data_map = FileManager::readMapFromFile(file_path);
        return data_map.at(did);
    }