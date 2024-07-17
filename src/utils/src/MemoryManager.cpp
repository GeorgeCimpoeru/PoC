#include "../include/MemoryManager.h"

MemoryManager* MemoryManager::instance = nullptr;

MemoryManager* MemoryManager::getInstance()
{
    if ( instance == nullptr)
    {
        instance = new MemoryManager(-1, "", nullptr);
    }
    if (instance->address == -1)
    {
        LOG_WARN(instance->logger->GET_LOGGER(), "Warning: The addres/path/logger is not initialized");
    }
    return instance;
}

MemoryManager* MemoryManager::getInstance(off_t address, std::string path, Logger* logger)
{
    if (instance == nullptr)
    {
        instance = new MemoryManager(address, path, logger);
    }
    return instance;
}

MemoryManager::MemoryManager(off_t address, std::string path, Logger* logger)
{
    this->address = address;
    this->address_continue_to_write = address;
    this->path = path;
    this->logger = logger;
}

int MemoryManager::to_int(std::string number)
{
    return std::stoi(number);
}

void MemoryManager::setAddress(off_t address)
{
    this->address = address;
    this->address_continue_to_write = address;
}

void MemoryManager::setPath(std::string path)
{
    this->path = path;
}

off_t MemoryManager::getAddress()
{
    return this->address;
}

std::string MemoryManager::getPath()
{
    return this->path;
}

std::string MemoryManager::runCommand(char command[])
{
    char buffer[128];
    std::string result = "";
    FILE* pipe = popen(command, "r");
    if (!pipe) throw std::runtime_error("popen() failed!");
    try
    {
        while (fgets(buffer, sizeof buffer, pipe) != NULL)
        {
            result += buffer;
        }
    } catch (...)
    {
        pclose(pipe);
        throw;
    }
    pclose(pipe);
    return result;
}

bool MemoryManager::availableAddress(off_t address)
{
    char verify_address_command[] = "sudo fdisk -l /dev/loop21 | grep '^/dev/' | grep '*' | awk '{print $3,$4}'";

    if (address == -1)
    {
        LOG_ERROR(instance->logger->GET_LOGGER(), "Error: the address was not initialize correctly.\n");
        return false;
    }
    std::string result = runCommand(verify_address_command);
    if (result.length() < 3)
    {
        LOG_WARN(instance->logger->GET_LOGGER(), "No boot partition found");
        return true;
    }
    std::string::size_type pos = result.find(' ');
    int start_position = to_int(result.substr(0,pos));
    int end_position = to_int(result.substr(pos+ 1));

    constexpr size_t SECTOR_SIZE = 512;
    size_t boot_start_byte = start_position * SECTOR_SIZE;
    size_t boot_end_byte = (end_position + 1) * SECTOR_SIZE - 1;
    if ( address <= boot_end_byte)
    {
        LOG_ERROR(instance->logger->GET_LOGGER(), "Error:Address in boot partition.");
        return false;
    }
    return true;
}

bool MemoryManager::availableMemory(off_t size_of_data)
{
    constexpr size_t SECTOR_SIZE = 512;

    char verify_memory_command[] = "sudo fdisk -l /dev/loop21 | grep '^/dev/' | grep -v '*' | awk '{print $3}'";
    std::string result = runCommand(verify_memory_command);
    if (result.length() < 3)
    {
        LOG_WARN(instance->logger->GET_LOGGER(), "No boot partition found");
        return true;
    }
    size_t last_memory_address = to_int(result) * SECTOR_SIZE;
    if ( address + size_of_data >= last_memory_address)
    {
        LOG_ERROR(instance->logger->GET_LOGGER(), "Error: Not enough memory.");
        return false;
    }
    return true;
}

bool MemoryManager::writeToAddress(std::vector<uint8_t>& data) 
{
    if(!availableAddress(this->address_continue_to_write) && !availableMemory(data.size()))
    {
        LOG_ERROR(instance->logger->GET_LOGGER(), "Error: Aborting.");
        return false;
    }
    int sd_fd = open(path.c_str(), O_RDWR );
    if (sd_fd < 0)
    {
        LOG_ERROR(instance->logger->GET_LOGGER(), "Error opening SD card device: " + path);
        return false;
    }

    if (lseek(sd_fd, address_continue_to_write, SEEK_SET) < 0)
    {
       LOG_ERROR(instance->logger->GET_LOGGER(), "Error seeking to address: " + std::to_string(address_continue_to_write));
        close(sd_fd);
        return false;
    }

    ssize_t bytes_written = write(sd_fd, data.data(), data.size());
    if (bytes_written != static_cast<ssize_t>(data.size()))
    {
        LOG_ERROR(instance->logger->GET_LOGGER(), "Error writing data to address: " + std::to_string(address));
        close(sd_fd);
        return false;
    }

    close(sd_fd);
    LOG_INFO(instance->logger->GET_LOGGER(), "Data successfully written to address " + std::to_string(address) + " on "+path);
    address_continue_to_write += data.size();
    return true;
}

bool MemoryManager::writeToFile(std::vector<uint8_t> &data, std::string path_file)
{
    std::ofstream sd_card(path_file, std::ios::out | std::ios::binary | std::ios::app);
    if (!sd_card.is_open())
    {
        LOG_ERROR(instance->logger->GET_LOGGER(), "Error opening file: " + path_file);
        return false;
    }

    sd_card.write(reinterpret_cast<const char*>(data.data()), data.size());

    if (sd_card.fail())
    {
        LOG_ERROR(instance->logger->GET_LOGGER(), "Failed to write to file: " + path_file);
        sd_card.close();
        return false;
    }

    sd_card.close();
    LOG_INFO(instance->logger->GET_LOGGER(), "Successfully written to file: " + path_file);
    return true;
}

std::vector<uint8_t> MemoryManager::readBinary(std::string path_to_binary)
{
    std::fstream sd_card;
    sd_card.open(path_to_binary, std::fstream::in | std::fstream::out | std::fstream::binary);
    if (!sd_card.is_open())
    {
        LOG_ERROR(instance->logger->GET_LOGGER(), "Error opening SD card device: " + path_to_binary);
        return {};
    }
    sd_card.seekg(0, std::ios::end);
    if (sd_card.fail())
    {
        LOG_ERROR(instance->logger->GET_LOGGER(), "Error: Could not seek the address");
        sd_card.close();
        return {};
    }
    std::streamsize size = sd_card.tellg();
    sd_card.seekg(0, std::ios::beg);

    std::vector<char> buffer(size);
    if (!sd_card.read(buffer.data(),size))
    {
        LOG_ERROR(instance->logger->GET_LOGGER(), "Failed to read from file " + path_to_binary);
        return {};
    }
    sd_card.close();
    std::vector<uint8_t> uint8Buffer(buffer.begin(), buffer.end());
    LOG_INFO(instance->logger->GET_LOGGER(), "Successfully readed from file: " + path_to_binary);
    return uint8Buffer;
}

std::vector<uint8_t> MemoryManager::readFromAddress(std::string path, off_t address_start, off_t size)
{
    if (! instance->availableAddress(address_start))
    {
        LOG_ERROR(instance->logger->GET_LOGGER(), "Error trying to read from address: " + std::to_string(address_start));
        return {};
    }
    int sd_fd = open(path.c_str(), O_RDWR );
    if (sd_fd < 0)
    {
        LOG_ERROR(instance->logger->GET_LOGGER(), "Error opening SD card device: " + path);
        return {};
    }

    if (lseek(sd_fd, address_start, SEEK_SET) < 0)
    {
        LOG_ERROR(instance->logger->GET_LOGGER(), "Error seeking to address: " + std::to_string(address_start));
        close(sd_fd);
        return {};
    }
    std::vector<char> buffer(size);
    auto bytes_readed = read(sd_fd, buffer.data(),size);
    if (bytes_readed != size)
    {
         LOG_ERROR(instance->logger->GET_LOGGER(), "Failed to read the file " + path);
        return {};
    }

    close(sd_fd);
    LOG_INFO(instance->logger->GET_LOGGER(), "Data successfully readed from address " + std::to_string(address_start) + " on "+path );
    std::vector<uint8_t> uint8Buffer(buffer.begin(), buffer.end());
    return uint8Buffer;
}

void print_bin_data(std::vector<uint8_t> &data)
{
    int i=0;
    for(auto element : data)
    {
        std::cout<<std::hex<< std::setw(2) << std::setfill('0') <<(unsigned int)element<< " ";
        if ((i + 1) % 16 == 0)
        {
            std::cout << std::endl;
        }
        i++;
    }
    std::cout<<"\n";
}