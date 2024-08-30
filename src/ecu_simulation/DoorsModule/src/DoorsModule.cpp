#include "../include/DoorsModule.h"

Logger* doorsModuleLogger = nullptr;
DoorsModule* doors = nullptr;

std::map<uint8_t, double> DoorsModule::timing_parameters;
std::map<uint8_t, std::future<void>> DoorsModule::active_timers;
std::map<uint8_t, std::atomic<bool>> DoorsModule::stop_flags;

/** Constructor - initializes the DoorsModule with default values,
 * sets up the CAN interface, and prepares the frame receiver. */
DoorsModule::DoorsModule() : moduleId(0x13),
                                 doorDriverOpen(false),
                                 doorPassengerOpen(false),
                                 doorDriverLocked(false),
                                 doorPassengerLocked(false),
                                 ajarWarning(false),
                                 canInterface(CreateInterface::getInstance(0x00, *doorsModuleLogger)),
                                 frameReceiver(nullptr)
{
    /* Insert the default DID values in the file */
    std::ofstream outfile("doors_data.txt");
    if (!outfile.is_open())
    {
        throw std::runtime_error("Failed to open file: doors_data.txt");
    }

    for (const auto& [data_identifier, data] : default_DID_doors)
    {
        outfile << std::hex << std::setw(4) << std::setfill('0') << data_identifier << " ";
        for (uint8_t byte : data) 
        {
            outfile << std::hex << std::setw(2) << std::setfill('0') << static_cast<int>(byte) << " ";
        }
        outfile << "\n";
    }
    outfile.close();

    doors_socket = canInterface->createSocket(0x00);
    /* Initialize the Frame Receiver */
    frameReceiver = new ReceiveFrames(doors_socket, moduleId, *doorsModuleLogger);

    LOG_INFO(doorsModuleLogger->GET_LOGGER(), "Doors object created successfully, ID : 0x{:X}", this->moduleId);

    /* Send Up-Notification to MCU */
    sendNotificationToMCU();
}

/* Parameterized Constructor - initializes the DoorsModule with provided interface number and module ID */
DoorsModule::DoorsModule(int _interfaceNumber, int _moduleId) : moduleId(_moduleId),
                                                                    doorDriverOpen(false),
                                                                    doorPassengerOpen(false),
                                                                    doorDriverLocked(false),
                                                                    doorPassengerLocked(false),
                                                                    ajarWarning(false),
                                                                    canInterface(CreateInterface::getInstance(_interfaceNumber, *doorsModuleLogger)),
                                                                    frameReceiver(nullptr)
{
    /* Insert the default DID values in the file */
    std::ofstream outfile("doors_data.txt");
    if (!outfile.is_open())
    {
        throw std::runtime_error("Failed to open file: doors_data.txt");
    }

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

    doors_socket = canInterface->createSocket(0x00);
    /* Initialize the Frame Receiver */
    frameReceiver = new ReceiveFrames(doors_socket, moduleId, *doorsModuleLogger);

    LOG_INFO(doorsModuleLogger->GET_LOGGER(), "Doors object created successfully using Parameterized Constructor, ID : 0x{:X}", this->moduleId);

    /* Send Up-Notification to MCU */ 
    sendNotificationToMCU();
}

/* Destructor */
DoorsModule::~DoorsModule()
{
    delete frameReceiver;
    LOG_INFO(doorsModuleLogger->GET_LOGGER(), "Doors object out of scope");
}

/* Function to notify MCU if the module is Up & Running */
void DoorsModule::sendNotificationToMCU()
{
    /* Create an instance of GenerateFrames with the CAN socket */
    GenerateFrames notifyFrame = GenerateFrames(doors_socket, *doorsModuleLogger);

    /* Create a vector of uint8_t (bytes) containing the data to be sent */
    std::vector<uint8_t> data = {0x0, 0xff, 0x13, 0x3};

    /* Send the CAN frame with ID 0x22130 and the data vector */
    notifyFrame.sendFrame(0x1310, data);

    LOG_INFO(doorsModuleLogger->GET_LOGGER(), "Doors module sent UP notification to MCU");
}

/* Helper function to execute shell commands and fetch output */
std::string DoorsModule::exec(char *cmd)
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

/* Helper function to parse doors info */
void DoorsModule::parseDoorsInfo(const std::string &data, bool &doorDriverOpen, bool &doorPassengerOpen, bool &doorDriverLocked, bool &doorPassengerLocked, bool &ajarWarning)
{
    std::istringstream stream(data);
    std::string line;
    std::unordered_map<uint16_t, std::string> updated_values;

    while (std::getline(stream, line))
    {
        if (line.find("doorDriverOpen:") != std::string::npos)
        {
            doorDriverOpen = line.substr(line.find(":") + 1) == "true";            
            updated_values[0x03A0] = std::to_string(static_cast<uint8_t>(doorDriverOpen));
        }
        else if (line.find("doorPassengerOpen:") != std::string::npos)
        {
            doorPassengerOpen = line.substr(line.find(":") + 1) == "true";            
            updated_values[0x03B0] = std::to_string(static_cast<uint8_t>(doorPassengerOpen));
        }
        else if (line.find("doorDriverLocked:") != std::string::npos)
        {
            doorDriverLocked = line.substr(line.find(":") + 1) == "true";            
            updated_values[0x03C0] = std::to_string(static_cast<uint8_t>(doorDriverLocked));
        }
        else if (line.find("doorPassengerLocked:") != std::string::npos)
        {
            doorPassengerLocked = line.substr(line.find(":") + 1) == "true";            
            updated_values[0x03D0] = std::to_string(static_cast<uint8_t>(doorPassengerLocked));
        }
        else if (line.find("ajarWarning:") != std::string::npos)
        {
            ajarWarning = line.substr(line.find(":") + 1) == "true";            
            updated_values[0x03E0] = std::to_string(static_cast<uint8_t>(ajarWarning));
        }
    }

    /* Path to doors data file */
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

/* Function to fetch data from system about doors */
void DoorsModule::fetchDoorsData()
{
    try
    {
        /* Execute the shell command to read System Info about Doors */
        char* path = strdup("upower -i /org/freedesktop/UPower/devices/doors_BAT0");
        std::string data = exec(path);
        /* Call the function in order to parse the datas */
        parseDoorsInfo(data, doorDriverOpen, doorPassengerOpen, doorDriverLocked, doorPassengerLocked, ajarWarning);

        /* Update class member variables with fetched data */
        this->doorDriverOpen = doorDriverOpen;
        this->doorPassengerOpen = doorPassengerOpen;
        this->doorDriverLocked = doorDriverLocked;
        this->doorPassengerLocked = doorPassengerLocked;
        this->ajarWarning = ajarWarning;

        LOG_INFO(doorsModuleLogger->GET_LOGGER(), "Doors Data : Door Driver Open: {} Wh, Door Passenger Open: {}, Door Driver Locked: {}, Door Passenger Locked: {},  Ajar Warning: {}", doorDriverOpen, doorPassengerOpen, doorPassengerLocked, ajarWarning);
    }
    catch (const std::exception &e)
    {
        /* std::cerr << "Error fetching doors data: " << e.what() << std::endl; */
        LOG_ERROR(doorsModuleLogger->GET_LOGGER(), "Error fetching doors data: {}", e.what());
    }
}

/* Function to receive CAN frames */
void DoorsModule::receiveFrames()
{
    LOG_INFO(doorsModuleLogger->GET_LOGGER(), "Doors module starts the frame receiver");

    /* Create a HandleFrames object to process received frames */
    HandleFrames handleFrames(this->doors_socket);

    /* Receive a CAN frame using the frame receiver and process it with handleFrames */
    frameReceiver->receive(handleFrames);

    /* Sleep for 100 milliseconds before receiving the next frame to prevent busy-waiting */
    std::this_thread::sleep_for(std::chrono::milliseconds(100));
}

/* Functon to Stop receiving frames */
void DoorsModule::stopFrames()
{
    frameReceiver->stop();
    LOG_INFO(doorsModuleLogger->GET_LOGGER(), "Doors module stopped the frame receiver");
}

/* Getter function to check if the driver's door is open */
bool DoorsModule::getDoorDriverOpen() const
{
    return doorDriverOpen;
}

/* Getter function to check if the passenger's door is open */
bool DoorsModule::getDoorPassengerOpen() const
{
    return doorPassengerOpen;
}

/* Getter function to check if the driver's door is locked */
bool DoorsModule::getDoorDriverLocked() const
{
    return doorDriverLocked;
}

/* Getter function to check if the passenger's door is locked */
bool DoorsModule::getDoorPassengerLocked() const
{
    return doorPassengerLocked;
}

/* Getter function to check if there is an ajar warning */
bool DoorsModule::getAjarWarning() const
{
    return ajarWarning;
}

int DoorsModule::getDoorsSocket() const
{
    return doors_socket;
}

void DoorsModule::setDoorsSocket(uint8_t interface_number)
{
    this->doors_socket = this->canInterface->createSocket(interface_number);
}
