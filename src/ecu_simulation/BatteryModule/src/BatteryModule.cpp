#include "../include/BatteryModule.h"


Logger* batteryModuleLogger = nullptr;
BatteryModule* battery = nullptr;

std::map<uint8_t, double> BatteryModule::timing_parameters;
std::map<uint8_t, std::future<void>> BatteryModule::active_timers;
std::map<uint8_t, std::atomic<bool>> BatteryModule::stop_flags;

/** Constructor - initializes the BatteryModule with default values,
 * sets up the CAN interface, and prepares the frame receiver. */
BatteryModule::BatteryModule() : moduleId(0x11),
                                 energy(0.0),
                                 voltage(0.0),
                                 percentage(0.0),
                                 canInterface(CreateInterface::getInstance(0x00, *batteryModuleLogger)),
                                 frameReceiver(nullptr)
{
    /* Insert the default DID values in the file */
    std::ofstream outfile("battery_data.txt");
    if (!outfile.is_open())
    {
        throw std::runtime_error("Failed to open file: battery_data.txt");
    }

    for (const auto& [data_identifier, data] : default_DID_battery)
    {
        outfile << std::hex << std::setw(4) << std::setfill('0') << data_identifier << " ";
        for (uint8_t byte : data) 
        {
            outfile << std::hex << std::setw(2) << std::setfill('0') << static_cast<int>(byte) << " ";
        }
        outfile << "\n";
    }
    outfile.close();

    battery_socket = canInterface->createSocket(0x00);
    /* Initialize the Frame Receiver */
    frameReceiver = new ReceiveFrames(battery_socket, moduleId, *batteryModuleLogger);

    LOG_INFO(batteryModuleLogger->GET_LOGGER(), "Battery object created successfully, ID : 0x{:X}", this->moduleId);

    /* Send Up-Notification to MCU */
    sendNotificationToMCU();
}

/* Parameterized Constructor - initializes the BatteryModule with provided interface number and module ID */
BatteryModule::BatteryModule(int _interfaceNumber, int _moduleId) : moduleId(_moduleId),
                                                                    energy(0.0),
                                                                    voltage(0.0),
                                                                    percentage(0.0),
                                                                    canInterface(CreateInterface::getInstance(_interfaceNumber, *batteryModuleLogger)),
                                                                    frameReceiver(nullptr)
{
    /* Insert the default DID values in the file */
    std::ofstream outfile("battery_data.txt");
    if (!outfile.is_open())
    {
        throw std::runtime_error("Failed to open file: battery_data.txt");
    }

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

    battery_socket = canInterface->createSocket(0x00);
    /* Initialize the Frame Receiver */
    frameReceiver = new ReceiveFrames(battery_socket, moduleId, *batteryModuleLogger);

    LOG_INFO(batteryModuleLogger->GET_LOGGER(), "Battery object created successfully using Parameterized Constructor, ID : 0x{:X}", this->moduleId);

    /* Send Up-Notification to MCU */ 
    sendNotificationToMCU();
}

/* Destructor */
BatteryModule::~BatteryModule()
{
    delete frameReceiver;
    LOG_INFO(batteryModuleLogger->GET_LOGGER(), "Battery object out of scope");
}

/* Function to notify MCU if the module is Up & Running */
void BatteryModule::sendNotificationToMCU()
{
    /* Create an instance of GenerateFrames with the CAN socket */
    GenerateFrames notifyFrame = GenerateFrames(battery_socket, *batteryModuleLogger);

    /* Create a vector of uint8_t (bytes) containing the data to be sent */
    std::vector<uint8_t> data = {0x01, 0xD9};

    /* Send the CAN frame with ID 0x22110 and the data vector */
    notifyFrame.sendFrame(0x1110, data);

    LOG_INFO(batteryModuleLogger->GET_LOGGER(), "Battery module sent UP notification to MCU");
}

/* Helper function to execute shell commands and fetch output */
std::string BatteryModule::exec(char *cmd)
{
    std::array<char, 128> buffer;
    std::string result;
    std::shared_ptr<FILE> pipe(popen(cmd, "r"), pclose);

    if (!pipe)
    {
        throw std::runtime_error("popen() failed!");
    }

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
            updated_values[0x01A0] = std::to_string(static_cast<uint8_t>(energy));
        }
        else if (line.find("voltage:") != std::string::npos)
        {
            voltage = std::stof(line.substr(line.find(":") + 1));
            updated_values[0x01B0] = std::to_string(static_cast<uint8_t>(voltage));
        }
        else if (line.find("percentage:") != std::string::npos)
        {
            percentage = std::stof(line.substr(line.find(":") + 1));
            updated_values[0x01C0] = std::to_string(static_cast<uint8_t>(percentage));
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
void BatteryModule::fetchBatteryData()
{
    try
    {
        /* Execute the shell command to read System Info about Battery */
        char* path = strdup("upower -i /org/freedesktop/UPower/devices/battery_BAT0");
        std::string data = exec(path);
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

/* Function to receive CAN frames */
void BatteryModule::receiveFrames()
{
    LOG_INFO(batteryModuleLogger->GET_LOGGER(), "Battery module starts the frame receiver");

    /* Create a HandleFrames object to process received frames */
    HandleFrames handleFrames(this->battery_socket, *batteryModuleLogger);

    /* Receive a CAN frame using the frame receiver and process it with handleFrames */
    frameReceiver->receive(handleFrames);

    /* Sleep for 100 milliseconds before receiving the next frame to prevent busy-waiting */
    std::this_thread::sleep_for(std::chrono::milliseconds(100));
}

/* Functon to Stop receiving frames */
void BatteryModule::stopFrames()
{
    frameReceiver->stop();
    LOG_INFO(batteryModuleLogger->GET_LOGGER(), "Battery module stopped the frame receiver");
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
    return battery_socket;
}

void BatteryModule::setBatterySocket(uint8_t interface_number)
{
    this->battery_socket = this->canInterface->createSocket(interface_number);
}
