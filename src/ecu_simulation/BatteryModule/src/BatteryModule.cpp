#include "../include/BatteryModule.h"

/** Constructor - initializes the BatteryModule with default values,
 * sets up the CAN interface, and prepares the frame receiver. */
BatteryModule::BatteryModule() : moduleId(0x101),
                                 energy(0.0),
                                 voltage(0.0),
                                 percentage(0.0),
                                 canInterface("vcan0"),
                                 frameReceiver(nullptr)
{
    /* Initialize the Frame Receiver */
    frameReceiver = new ReceiveFrames(canInterface.getSocketFd(), moduleId);
#ifdef BATTERY_MODULE_DEBUG
    std::cout << "BatteryModule()" << std::endl;
    std::cout << "(BatteryModule)moduleId = " << this->moduleId << std::endl;
#endif
}

/* Parameterized Constructor - initializes the BatteryModule with provided interface number and module ID */
BatteryModule::BatteryModule(int _interfaceNumber, int _moduleId) : moduleId(_moduleId),
                                                                    energy(0.0),
                                                                    voltage(0.0),
                                                                    percentage(0.0),
                                                                    canInterface("vcan" + std::to_string(_interfaceNumber)),
                                                                    frameReceiver(nullptr)
{
    /* Initialize the Frame Receiver */
    frameReceiver = new ReceiveFrames(canInterface.getSocketFd(), moduleId);
#ifdef BATTERY_MODULE_DEBUG
    std::cout << "BatteryModule(int interfaceNumber, int moduleId)" << std::endl;
    std::cout << "(BatteryModule)moduleId = " << this->moduleId << std::endl;
#endif
}

/* Destructor */
BatteryModule::~BatteryModule()
{
    delete frameReceiver;
}

/* Helper function to execute shell commands and fetch output */
std::string BatteryModule::exec(const char *cmd)
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

    while (std::getline(stream, line))
    {
        if (line.find("energy:") != std::string::npos)
        {
            energy = std::stof(line.substr(line.find(":") + 1));
        }
        else if (line.find("voltage:") != std::string::npos)
        {
            voltage = std::stof(line.substr(line.find(":") + 1));
        }
        else if (line.find("percentage:") != std::string::npos)
        {
            percentage = std::stof(line.substr(line.find(":") + 1));
        }
        else if (line.find("state:") != std::string::npos)
        {
            size_t pos = line.find(":");
            /* Extract substring starting from the first non-whitespace character after ':' */
            state = line.substr(pos + 1);
            /* Remove leading whitespace */
            state = state.substr(state.find_first_not_of(" \t"));
        }
    }
}

/* Function to fetch data from system about battery */
void BatteryModule::fetchBatteryData()
{
    try
    {
        /* Execute the shell command to read System Info about Battery */
        std::string data = exec("upower -i /org/freedesktop/UPower/devices/battery_BAT0");

        /* Call the function in order to parse the datas */
        parseBatteryInfo(data, energy, voltage, percentage, state);

        /* Update class member variables with fetched data */
        this->energy = energy;
        this->voltage = voltage;
        this->percentage = percentage;
#ifdef BATTERY_MODULE_DEBUG
        std::cout << "Fetched Data - Energy: " << energy << " Wh, Voltage: " << voltage << " V, Percentage: " << percentage << "%, State: " << state << std::endl;
#endif
    }
    catch (const std::exception &e)
    {
        std::cerr << "Error fetching battery data: " << e.what() << std::endl;
    }
}

/* Function to receive CAN frames */
void BatteryModule::receiveFrames()
{
    /* Create a HandleFrames object to process received frames */
    HandleFrames handleFrames;

    /* Receive a CAN frame using the frame receiver and process it with handleFrames */
    frameReceiver->Receive(handleFrames);

    /* Sleep for 100 milliseconds before receiving the next frame to prevent busy-waiting */
    std::this_thread::sleep_for(std::chrono::milliseconds(100));
}

/* Functon to Stop receiving frames */
void BatteryModule::stopFrames()
{
    frameReceiver->Stop();
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