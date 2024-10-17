#include "../include/MCUModule.h"

Logger* MCULogger = nullptr;
namespace MCU
{
    MCUModule* mcu = nullptr;
    std::map<uint8_t, double> MCUModule::timing_parameters;
    std::map<uint8_t, std::future<void>> MCUModule::active_timers;
    std::map<uint8_t, std::atomic<bool>> MCUModule::stop_flags;
    const std::vector<uint16_t> MCUModule::VALID_DID_MCU =
    {
        /* Vehicle Identification Number (VIN) */
        0xF190,
        /** ECU Serial Number */
        0xF17F,
        /* System Supplier ECU Hardware Number */
        0xF18C,
        /* System Supplier ECU Software Number */
        0xF1A0,
        /* System Name or Engine Type */
        0xF187,
        /* System Supplier ECU Software Version Number */
        0xF1A2,
        /* System Supplier ECU Hardware Version Number */
        0xF1A1,
        /* System Supplier ECU Manufacturing Date */
        0xF1A4,
        /* System Supplier ECU Coding/Configuration Part Number */
        0xF1A5,
        /* System Calibration Identification Number */
        0xF1A8,
        /* System Calibration Verification Number (CVN) */
        0xF1A9,
        /* System ECU Boot Software Identification Number */
        0xF1AA,
        /* System ECU Application Software Identification Number */
        0xF1AB,
        /* System ECU Data Set Identification Number */
        0xF1AC,
        /* System ECU Flash Software Version Number */
        0xF1AD,
        /* OTA Update Status */
        0x01E0
    };
    /* Constructor */
    MCUModule::MCUModule(uint8_t interfaces_number) : 
                    is_running(false),
                    create_interface(CreateInterface::getInstance(interfaces_number, *MCULogger)),
                    receive_frames(nullptr),
                    mcu_api_socket(create_interface->createSocket(interfaces_number)),
                    mcu_ecu_socket(create_interface->createSocket(interfaces_number >> 4))
                    {
        writeDataToFile();
        receive_frames = new ReceiveFrames(mcu_ecu_socket, mcu_api_socket);
    }

    /* Default constructor */
    MCUModule::MCUModule() : is_running(false),
                         create_interface(CreateInterface::getInstance(0x01, *MCULogger)),
                         receive_frames(nullptr)
    {
        writeDataToFile();
    }

    /* Destructor */
    MCUModule::~MCUModule() 
    {
        create_interface->stopInterface();
        delete receive_frames;
        if(system("pkill main_mcu") != 0)
        {

        }
        else
        {

        }
    }

    /* Start the module */
    void MCUModule::StartModule() 
    { 
    is_running = true;
    create_interface->setSocketBlocking(mcu_api_socket);
    create_interface->setSocketBlocking(mcu_ecu_socket);
    }

    int MCUModule::getMcuApiSocket() const 
    {
        return mcu_api_socket;
    }
    int MCUModule::getMcuEcuSocket() const 
    {
        return mcu_ecu_socket;
    }

    void MCUModule::setMcuApiSocket(uint8_t interface_number)
    {
        this->mcu_api_socket = this->create_interface->createSocket(interface_number);
    }
    
    void MCUModule::setMcuEcuSocket(uint8_t interface_number)
    {
        this->mcu_ecu_socket = this->create_interface->createSocket(interface_number >> 4);
    }

    /* Stop the module */
    void MCUModule::StopModule() 
    { 
    is_running = false;
    receive_frames->stopProcessingQueue();            
    receive_frames->stopListenAPI();
    receive_frames->stopListenCANBus(); 
    }

    /* Receive frames */
    void MCUModule::recvFrames() 
    {
        while (is_running)
        {
            receive_frames->startListenAPI();
            receive_frames->startListenCANBus();
            /* Start a thread to process the queue */
            std::thread queue_thread_process(&ReceiveFrames::processQueue, receive_frames);

            /* Start a thread to listen on API socket */
            std::thread queue_thread_listen(&ReceiveFrames::receiveFramesFromAPI, receive_frames);

            /* Receive frames from the CAN bus */
            receive_frames->receiveFramesFromCANBus();

            receive_frames->stopListenAPI();
            receive_frames->stopListenCANBus();

            /* Wait for the threads to finish */
            queue_thread_process.join();
            queue_thread_listen.join();
        }
    }
    void MCUModule::writeDataToFile()
    {
        std::string file_path = std::string(PROJECT_PATH) + "/src/mcu/mcu_data.txt";
        /* Insert the default DID values in the file */
        std::ofstream outfile(file_path);
        if (!outfile.is_open())
        {
            throw std::runtime_error("Failed to open file: mcu_data.txt");
        }

        /* Check if old_mcu_data.txt exists */
        std::string old_file_path = "old_mcu_data.txt";
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
            /* Write the default DID values to mcu_data.txt */
            for (const auto& [data_identifier, data] : default_DID_MCU)
            {
                outfile << std::hex << std::setw(4) << std::setfill('0') << std::uppercase << data_identifier << " ";
                for (uint8_t byte : data)
                {
                    outfile << std::hex << std::setw(2) << std::setfill('0') << static_cast<int>(byte) << " ";
                }
                outfile << "\n";
            }
        }
        outfile.close();
    }

    void MCUModule::setDidValue(const uint16_t did, const std::vector<uint8_t>& value)
    {
        std::string file_path = std::string(PROJECT_PATH) + "/src/mcu/mcu_data.txt";
        auto data_map = FileManager::readMapFromFile(file_path);
        auto did_it = data_map.find(did);

        if(did_it == default_DID_MCU.end())
        {
            LOG_WARN(MCULogger->GET_LOGGER(), "DID {} not found when trying to set value", did);
            return;
        }
        did_it->second = value;
        FileManager::writeMapToFile(file_path, data_map);
    }

    std::vector<uint8_t> MCUModule::getDidValue(const uint16_t did) const
    {
        /* Should also contain validation */
        std::string file_path = std::string(PROJECT_PATH) + "/src/mcu/mcu_data.txt";
        auto data_map = FileManager::readMapFromFile(file_path);
        return data_map.at(did);
    }
}