#include "../include/EngineModule.h"

Logger* engineModuleLogger = nullptr;
EngineModule* engine = nullptr;

std::map<uint8_t, double> EngineModule::timing_parameters;
std::map<uint8_t, std::future<void>> EngineModule::active_timers;
std::map<uint8_t, std::atomic<bool>> EngineModule::stop_flags;
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
EngineModule::EngineModule() : moduleId(0x12),
                                 canInterface(CreateInterface::getInstance(0x00, *engineModuleLogger)),
                                 frameReceiver(nullptr)
{
    writeDataToFile();
    engine_socket = canInterface->createSocket(0x00);
    /* Initialize the Frame Receiver */
    frameReceiver = new ReceiveFrames(engine_socket, moduleId, *engineModuleLogger);

    LOG_INFO(engineModuleLogger->GET_LOGGER(), "Engine object created successfully, ID : 0x{:X}", this->moduleId);

    /* Send Up-Notification to MCU */
    sendNotificationToMCU();
}

/* Parameterized Constructor - initializes the EngineModule with provided interface number and module ID */
EngineModule::EngineModule(int _interfaceNumber, int _moduleId) : moduleId(_moduleId),
                                                                    canInterface(CreateInterface::getInstance(_interfaceNumber, *engineModuleLogger)),
                                                                    frameReceiver(nullptr)
{
    writeDataToFile();
    engine_socket = canInterface->createSocket(0x00);
    /* Initialize the Frame Receiver */
    frameReceiver = new ReceiveFrames(engine_socket, moduleId, *engineModuleLogger);

    LOG_INFO(engineModuleLogger->GET_LOGGER(), "Engine object created successfully using Parameterized Constructor, ID : 0x{:X}", this->moduleId);

    /* Send Up-Notification to MCU */ 
    sendNotificationToMCU();
}

/* Destructor */
EngineModule::~EngineModule()
{
    delete frameReceiver;
    LOG_INFO(engineModuleLogger->GET_LOGGER(), "Engine object out of scope");
}

/* Function to notify MCU if the module is Up & Running */
void EngineModule::sendNotificationToMCU()
{
    /* Create an instance of GenerateFrames with the CAN socket */
    GenerateFrames notifyFrame = GenerateFrames(engine_socket, *engineModuleLogger);

    /* Create a vector of uint8_t (bytes) containing the data to be sent */
    std::vector<uint8_t> data = {0x01, 0xD9};

    /* Send the CAN frame with ID 0x22110 and the data vector */
    notifyFrame.sendFrame(0x1210, data);

    LOG_INFO(engineModuleLogger->GET_LOGGER(), "Engine module sent UP notification to MCU");
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
            byte = dist(gen);  // Generate a random value between 0 and 255
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

/* Function to receive CAN frames */
void EngineModule::receiveFrames()
{
    LOG_INFO(engineModuleLogger->GET_LOGGER(), "Engine module starts the frame receiver");

    /* Create a HandleFrames object to process received frames */
    HandleFrames handleFrames(engine_socket, *engineModuleLogger);

    /* Receive a CAN frame using the frame receiver and process it with handleFrames */
    frameReceiver->receive(handleFrames);

    /* Sleep for 100 milliseconds before receiving the next frame to prevent busy-waiting */
    std::this_thread::sleep_for(std::chrono::milliseconds(100));
}

/* Functon to Stop receiving frames */
void EngineModule::stopFrames()
{
    frameReceiver->stop();
    LOG_INFO(engineModuleLogger->GET_LOGGER(), "Engine module stopped the frame receiver");
}

int EngineModule::getEngineSocket() const
{
    return engine_socket;
}

void EngineModule::setEngineSocket(uint8_t interface_number)
{
    this->engine_socket = this->canInterface->createSocket(interface_number);
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
    }
    outfile.close();
}