#include "../include/DoorsModule.h"

Logger* doorsModuleLogger = nullptr;
DoorsModule* doors = nullptr;

std::unordered_map<uint16_t, std::vector<uint8_t>> DoorsModule::default_DID_doors = {
        {0x03A0, {0}},  /* Driver Door Status  */
        {0x03B0, {0}},  /* Passenger Door Status*/
        {0x03C0, {0}},  /* Door Driver Locked Status*/
        {0x03D0, {0}},  /* Door Passenger Locked Status*/
        {0x03E0, {0}},   /* Ajar Warning Status */
#ifdef SOFTWARE_VERSION
        {0xF1A2, {static_cast<uint8_t>(SOFTWARE_VERSION)}}
#else
        {0xF1A2, {0x00}}
#endif
    };

/** Constructor - initializes the DoorsModule with default values,
 * sets up the CAN interface, and prepares the frame receiver. */
DoorsModule::DoorsModule()
{
    /* ECU object responsible for common functionalities for all ECUs (sockets, frames, parameters) */
    _ecu = new ECU(DOORS_ID, *doorsModuleLogger);
    writeDataToFile();
    LOG_INFO(doorsModuleLogger->GET_LOGGER(), "Doors object created successfully");
}

/* Destructor */
DoorsModule::~DoorsModule()
{
    LOG_INFO(doorsModuleLogger->GET_LOGGER(), "Doors object out of scope");
}

/* Function to fetch data with simulated door data */
void DoorsModule::fetchDoorsData()
{    
    /* Generate random values for each DID */
    std::unordered_map<uint16_t, std::string> updated_values;
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<> dist(0, 1);

    for (auto& [did, data] : default_DID_doors)
    {
        std::stringstream data_ss;
        for (auto& byte : data)
        {
            byte = dist(gen);  // Generate a random value between 0 and 1: doors status - 0:closed; 1:open; doors lock status - 0:unlocked; 1:locked; ajar warning - 0:no warning; 1: warning
            data_ss << std::hex << std::setw(2) << std::setfill('0') << std::uppercase << static_cast<int>(byte) << " ";
        }
        updated_values[did] = data_ss.str();
    }

    /* Path to engine data file */
    std::string file_path = "doors_data.txt";

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
        for (const auto& pair : updated_values)
        {
            std::stringstream did_ss;
            did_ss << std::hex << std::setw(4) << std::setfill('0') << std::uppercase << pair.first;
            if (file_line.find(did_ss.str()) != std::string::npos)
            {
                updated_file_contents += did_ss.str() + " " + pair.second + "\n";
                break;
            }
        }
    }

    /* Write the updated contents back to the file */
    std::ofstream outfile(file_path);
    outfile << updated_file_contents;
    outfile.close();

    LOG_INFO(doorsModuleLogger->GET_LOGGER(), "Doors data file updated with random values.");
}

int DoorsModule::getDoorsSocket() const
{
    return _ecu->_ecu_socket;
}

void DoorsModule::writeDataToFile()
{
    /* Insert the default DID values in the file */
    std::ofstream outfile("doors_data.txt");
    if (!outfile.is_open())
    {
        throw std::runtime_error("Failed to open file: doors_data.txt");
    }

    /* Check if old_doors_data.txt exists */
    std::string old_file_path = "old_doors_data.txt";
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
        for (const auto& [data_identifier, data] : default_DID_doors)
        {
            outfile << std::hex << std::setw(4) << std::setfill('0') << std::uppercase << data_identifier << " ";
            for (uint8_t byte : data)
            {
                outfile << std::hex << std::setw(1) << std::setfill('0') << static_cast<int>(byte) << " ";
            }
            outfile << "\n";
        }
        outfile.close();
        fetchDoorsData();
    }
}