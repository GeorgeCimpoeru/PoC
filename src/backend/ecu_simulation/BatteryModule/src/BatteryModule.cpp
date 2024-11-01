#include "../include/BatteryModule.h"


Logger* batteryModuleLogger = nullptr;
BatteryModule* battery = nullptr;

std::unordered_map<uint16_t, std::vector<uint8_t>> BatteryModule::default_DID_battery = {
        {0x01A0, {0}},  /* Energy Level */
        {0x01B0, {0}},  /* Voltage */
        {0x01C0, {0}},  /* Percentage */
        {0x01D0, {0}},   /* State of Charge */
        {0x01E0, {0}},   /* Temperature (C) */
        {0x01F0, {0}},   /* Life cycle */
#ifdef SOFTWARE_VERSION
        {0xF1A2, {static_cast<uint8_t>(SOFTWARE_VERSION)}}
#else
        {0xF1A2, {0x00}}
#endif
};

/** Constructor - initializes the BatteryModule with default values,
 * sets up the CAN interface, and prepares the frame receiver. */
BatteryModule::BatteryModule() : energy(0.0),
                                 voltage(0.0),
                                 percentage(0.0)
{

    /* ECU object responsible for common functionalities for all ECUs (sockets, frames, parameters) */
    _ecu = new ECU(BATTERY_ID, *batteryModuleLogger);
    writeDataToFile();
    checkDTC();
    LOG_INFO(batteryModuleLogger->GET_LOGGER(), "Battery object created successfully");
}

/* Destructor */
BatteryModule::~BatteryModule()
{
    LOG_INFO(batteryModuleLogger->GET_LOGGER(), "Battery object out of scope");
}

/* Helper function to execute shell commands and fetch output */
std::string BatteryModule::exec(const char *cmd, const char *mode)
{
    std::array<char, 128> buffer;
    std::string result;
    FILE* raw_pipe = popen(cmd, mode);

    if (!raw_pipe)
    {
        throw std::runtime_error("popen() failed!");
    }

    std::shared_ptr<FILE> pipe(raw_pipe, pclose);

    while (fgets(buffer.data(), buffer.size(), pipe.get()) != nullptr)
    {
        result += buffer.data();
    }

    return result;
}

/* Helper function to parse battery info */
void BatteryModule::parseBatteryInfo(const std::string &data, float &energy, float &voltage, float &percentage, std::string &state)
{
    std::istringstream stream(data);
    std::string line;
    std::unordered_map<uint16_t, std::string> updated_values;

    while (std::getline(stream, line))
    {
        if (line.find("energy:") != std::string::npos)
        {
            energy = std::stof(line.substr(line.find(":") + 1));
            uint8_t energy_value = static_cast<uint8_t>(energy);
            std::string energy_str = std::to_string(energy_value);
            /* Add a leading '0' if the string length is odd to ensure pairs of two characters */
            if (energy_str.size() % 2 != 0) {
                energy_str = "0" + energy_str;
            }
            /* Group characters in pairs of two with a space separator */
            std::string energy_grouped;
            for (size_t i = 0; i < energy_str.size(); i += 2) {
                energy_grouped += energy_str.substr(i, 2) + " ";
            }
            updated_values[0x01A0] = energy_grouped;
        }
        else if (line.find("voltage:") != std::string::npos)
        {
            voltage = std::stof(line.substr(line.find(":") + 1));
            uint8_t voltage_value = static_cast<uint8_t>(voltage);
            std::string voltage_str = std::to_string(voltage_value);
            if (voltage_str.size() % 2 != 0) {
                voltage_str = "0" + voltage_str;
            }
            std::string voltage_grouped;
            for (size_t i = 0; i < voltage_str.size(); i += 2) {
                voltage_grouped += voltage_str.substr(i, 2) + " ";
            }
            updated_values[0x01B0] = voltage_grouped;
        }
        else if (line.find("percentage:") != std::string::npos)
        {
            percentage = std::stof(line.substr(line.find(":") + 1));
            uint8_t percentage_value = static_cast<uint8_t>(percentage);
            std::string percentage_str = std::to_string(percentage_value);
            if (percentage_str.size() % 2 != 0) {
                percentage_str = "0" + percentage_str;
            }
            std::string percentage_grouped;
            for (size_t i = 0; i < percentage_str.size(); i += 2) {
                percentage_grouped += percentage_str.substr(i, 2) + " ";
            }
            updated_values[0x01C0] = percentage_grouped;
        }
        else if (line.find("state:") != std::string::npos)
        {
            size_t pos = line.find(":");
            state = line.substr(pos + 1);
            state = state.substr(state.find_first_not_of(" \t"));

            /* default: unknown */
            uint8_t state_value = 0x00;
            if (state == "charging")
            {
                state_value = 0x01;
            }
            else if (state == "discharging")
            {
                state_value = 0x02;
            }
            else if (state == "empty")
            {
                state_value = 0x03;
            }
            else if (state == "fully-charged")
            {
                state_value = 0x04;
            }
            else if (state == "pending-charge")
            {
                state_value = 0x05;
            }
            else if (state == "pending-discharge")
            {
                state_value = 0x06;
            }
            updated_values[0x01D0] = std::to_string(state_value);
        }
    }

    /* Random values for the 0x01E0 and 0x01F0 dids*/
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<> dist(0, 100);

    for (auto& [did, data] : default_DID_battery)
    {
        if(did == 0x01E0 || did == 0x01F0)
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
    }

    /* Path to battery data file */
    std::string file_path = "battery_data.txt";

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
        for (const auto &pair : updated_values)
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
}

/* Function to fetch data from system about battery */
void BatteryModule::fetchBatteryData(const char *mode)
{
    try
    {
        /* Execute the shell command to read System Info about Battery */
        char* path = strdup("upower -i /org/freedesktop/UPower/devices/battery_BAT0");
        std::string data = exec(path,mode);
        /* Call the function in order to parse the datas */
        parseBatteryInfo(data, energy, voltage, percentage, state);

        /* Update class member variables with fetched data */
        this->energy = energy;
        this->voltage = voltage;
        this->percentage = percentage;

        LOG_INFO(batteryModuleLogger->GET_LOGGER(), "Battery Data : Energy: {} Wh, Voltage: {} V, Percentage: {} %, State: {}", energy, voltage, percentage, state);
    }
    catch (const std::exception &e)
    {
        /* std::cerr << "Error fetching battery data: " << e.what() << std::endl; */
        LOG_ERROR(batteryModuleLogger->GET_LOGGER(), "Error fetching battery data: {}", e.what());
    }
}

/* Getter function for current */
float BatteryModule::getEnergy() const
{
    return energy;
}

/* Getter function for voltage */
float BatteryModule::getVoltage() const
{
    return voltage;
}

/* Getter function for temperature */
float BatteryModule::getPercentage() const
{
    return percentage;
}

std::string BatteryModule::getLinuxBatteryState()
{
    return state;
}

int BatteryModule::getBatterySocket() const
{
    return _ecu->_ecu_socket;
}


void BatteryModule::writeDataToFile()
{
    /* Insert the default DID values in the file */
    std::ofstream outfile("battery_data.txt");
    if (!outfile.is_open())
    {
        throw std::runtime_error("Failed to open file: battery_data.txt");
    }

    /* Check if old_battery_data.txt exists */
    std::string old_file_path = "old_battery_data.txt";
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
        for (const auto& [data_identifier, data] : default_DID_battery)
        {
            outfile << std::hex << std::setw(4) << std::setfill('0') << std::uppercase << data_identifier << " ";
            for (uint8_t byte : data)
            {
                outfile << std::hex << std::setw(1) << std::setfill('0') << static_cast<int>(byte) << " ";
            }
            outfile << "\n";
        }
        outfile.close();
        fetchBatteryData("r");
    }
}

void BatteryModule::checkDTC()
{
    /* Check if dtcs.txt exists */
    std::string dtc_file_path = std::string(PROJECT_PATH) + "/src/ecu_simulation/BatteryModule/dtcs.txt";
    std::string battery_file_path = std::string(PROJECT_PATH) + "/src/ecu_simulation/BatteryModule/battery_data.txt";
    std::ifstream infile(dtc_file_path);

    if (!infile.is_open())
    {
        std::ofstream outfile(dtc_file_path);
        if (outfile.is_open())
        {
            LOG_INFO(batteryModuleLogger->GET_LOGGER(), "dtcs.txt file created successfully.");
            outfile.close();
        }
        else
        {
            LOG_ERROR(batteryModuleLogger->GET_LOGGER(), "Failed to create dtcs.txt file.");
            return;
        }
    }
    else
    {
        infile.close();
    }
    /* Read the map with DIDs from the file */
    std::unordered_map<uint16_t, std::vector<uint8_t>> current_DID_value = FileManager::readMapFromFile(battery_file_path);

    /* Voltage DTC */
    FileManager::writeDTC(current_DID_value, dtc_file_path, 0x01B0, 12, 13, "P01B0 24");
    /* Temperature DTC */
    FileManager::writeDTC(current_DID_value, dtc_file_path, 0x01E0, 21, 27, "P01E0 24");
}