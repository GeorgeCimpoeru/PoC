#include "../include/EngineModule.h"

Logger* engineModuleLogger = nullptr;
EngineModule* engine = nullptr;

std::unordered_map<uint16_t, std::vector<uint8_t>> EngineModule::default_DID_engine = {
        /* Engine RPM */
        {0x0100, {0}},
        /* Engine Coolant Temperature */
        {0x010C, {0}},
        /* Throttle Position */
        {0x0110, {0}},
        /* Vehicle Speed */
        {0x0114, {0}},
        /* Engine Load */
        {0x011C, {0}},
        /* Fuel Level */
        {0x0120, {0}},
        /* Oil Temperature */
        {0x0124, {0}},
        /* Fuel Pressure */
        {0x012C, {0}},
        /* Intake Air Temperature */
        {0x0130, {0}}
    };

/** Constructor - initializes the EngineModule with default values,
 * sets up the CAN interface, and prepares the frame receiver. */
EngineModule::EngineModule()
{
    /* ECU object responsible for common functionalities for all ECUs (sockets, frames, parameters) */
    _ecu = new ECU(ENGINE_ID, *engineModuleLogger);
    writeDataToFile();
    checkDTC();
    LOG_INFO(engineModuleLogger->GET_LOGGER(), "Engine object created successfully");
}

/* Destructor */
EngineModule::~EngineModule()
{
    LOG_INFO(engineModuleLogger->GET_LOGGER(), "Engine object out of scope");
}

void EngineModule::fetchEngineData()
{
    /* Path to engine data file */
    std::string file_path = "engine_data.txt";

    /* Generate random values for each DID */
    std::unordered_map<uint16_t, std::string> updated_values;
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<> dist(0, 255);

    for (auto& [did, data] : default_DID_engine)
    {
        std::stringstream data_ss;
        for (auto& byte : data)
        {
            byte = dist(gen);  
            /* Generate a random value between 0 and 255 */
            data_ss << std::hex << std::setw(2) << std::setfill('0') << std::uppercase << static_cast<int>(byte) << " ";
        }
        updated_values[did] = data_ss.str();
    }

    /* Read the current file contents into memory */
    std::ifstream infile(file_path);
    std::stringstream buffer;
    buffer << infile.rdbuf();
    infile.close();

    std::string file_contents = buffer.str();
    std::istringstream file_stream(file_contents);
    std::string updated_file_contents;
    std::string file_line;

    /* Update the relevant DID values in the file contents */
    while (std::getline(file_stream, file_line))
    {
        bool updated = false;
        for (const auto& pair : updated_values)
        {
            std::stringstream did_ss;
            did_ss << std::hex << std::setw(4) << std::setfill('0') << std::uppercase << pair.first;
            if (file_line.find(did_ss.str()) != std::string::npos)
            {
                updated_file_contents += did_ss.str() + " " + pair.second + "\n";
                updated = true;
                break;
            }
        }
        if (!updated)
        {
            updated_file_contents += file_line + "\n";
        }
    }

    /* Write the updated contents back to the file */
    std::ofstream outfile(file_path);
    outfile << updated_file_contents;
    outfile.close();

    LOG_INFO(engineModuleLogger->GET_LOGGER(), "Engine data file updated with random values.");
}

int EngineModule::getEngineSocket() const
{
    return _ecu->_ecu_socket;
}

void EngineModule::writeDataToFile()
{
    /* Insert the default DID values in the file */
    std::ofstream outfile("engine_data.txt");
    if (!outfile.is_open())
    {
        throw std::runtime_error("Failed to open file: engine_data.txt");
    }

    /* Check if old_engine_data.txt exists */
    std::string old_file_path = "old_engine_data.txt";
    std::ifstream infile(old_file_path);

    if (infile.is_open())
    {
        /* Read the current file contents into memory */
        std::stringstream buffer;
        /* Read the entire file into the buffer */
        buffer << infile.rdbuf();
        infile.close();

        /* Store the original content */
        std::string original_file_contents = buffer.str();

        /* Write the content of old_mcu_data.txt into mcu_data.txt */
        outfile << original_file_contents;

        /* Delete the old file after reading its contents */
        std::remove(old_file_path.c_str());
        outfile.close();
    }
    else
    {
        for (const auto& [data_identifier, data] : default_DID_engine)
        {
            outfile << std::hex << std::setw(4) << std::setfill('0') << std::uppercase << data_identifier << " ";
            for (uint8_t byte : data)
            {
                outfile << std::hex << std::setw(1) << std::setfill('0') << static_cast<int>(byte) << " ";
            }
            outfile << "\n";
        }
        outfile.close();
        fetchEngineData();
    }
}

void EngineModule::checkDTC()
{      
    /* Check if dtcs.txt exists */
    std::string dtc_file_path = std::string(PROJECT_PATH) + "/src/ecu_simulation/EngineModule/dtcs.txt";
    std::string engine_file_path = std::string(PROJECT_PATH) + "/src/ecu_simulation/EngineModule/engine_data.txt";
    std::ifstream infile(dtc_file_path);

    if (!infile.is_open())
    {
        std::ofstream outfile(dtc_file_path);
        if (outfile.is_open())
        {
            LOG_INFO(engineModuleLogger->GET_LOGGER(), "dtcs.txt file created successfully.");
            outfile.close();
        }
        else
        {
            LOG_ERROR(engineModuleLogger->GET_LOGGER(), "Failed to create dtcs.txt file.");
            return;
        }
    }
    else
    {
        infile.close();
    }
    /* Read the map with DIDs from the file */
    std::unordered_map<uint16_t, std::vector<uint8_t>> current_DID_value = FileManager::readMapFromFile(engine_file_path);

    /* Fuel Pressure DTC*/
    FileManager::writeDTC(current_DID_value, dtc_file_path, 0x012C, 30, 50, "P0190 24");
    /* Oil Temperature DTC */
    FileManager::writeDTC(current_DID_value, dtc_file_path, 0x0124, 230, 260, "P0196 24");
    /* Engine Load DTC*/
    FileManager::writeDTC(current_DID_value, dtc_file_path, 0x011C, 0, 85, "P0069 24");
    /* Engine Coolant Temperature DTC */
    FileManager::writeDTC(current_DID_value, dtc_file_path, 0x010C, 90, 105, "P0115 24");
}

