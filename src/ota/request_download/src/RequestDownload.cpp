#include <mutex>
#include "../include/RequestDownload.h"
#include "../../../mcu/include/MCUModule.h"

RequestDownloadService::RequestDownloadService(Logger& RDSlogger)
                        : generateFrames(RDSlogger)
{
   
}
RequestDownloadService::RequestDownloadService(int socket, Logger& RDSlogger)
                        : socket(socket), generateFrames(RDSlogger)
{
    
}
RequestDownloadService::~RequestDownloadService()
{

}

void RequestDownloadService::requestDownloadRequest(int id, std::vector<uint8_t>stored_data, Logger& RDSlogger) {
    LOG_INFO(RDSlogger.GET_LOGGER(), "Service 0x34 RequestDownload");

    
    uint8_t data_format_identifier = stored_data[2];
    uint8_t compression_type = (data_format_identifier & 0xF0) >> 4;
    uint8_t encryption_type = data_format_identifier & 0x0F;

    /* Log the DFI information */ 
    LOG_INFO(RDSlogger.GET_LOGGER(), "Data Format Identifier: 0x{0:x}", static_cast<int>(data_format_identifier));
    
    LOG_INFO(RDSlogger.GET_LOGGER(), "Compression Type: 0x{0:x}", static_cast<int>(compression_type));
    LOG_INFO(RDSlogger.GET_LOGGER(), "Encryption Type: 0x{0:x}", static_cast<int>(encryption_type));

    /* Check for compression */ 
    if (compression_type == 0x0) {
        LOG_INFO(RDSlogger.GET_LOGGER(), "No compression method used");
    } else {
        /** Decompress using tar
         * Decompress stored_data from position 4 onwards (assuming position 2 is DFI)
         * std::vector<uint8_t> decompressed_data = decompresstar(stored_data, 2);
         * Process decompressed_data as needed
         */ 
    }

    /* Check for encryption */ 
    if (encryption_type == 0x0) {
        LOG_INFO(RDSlogger.GET_LOGGER(), "No encryption method used");
    } else {
        /* check if encryption is needed */ 
    }

    /* Retrieve the address_memory_length */ 
    uint8_t address_memory_length = stored_data[3];
    /* Determine the length of memory address and memory size */ 
    /* High nibble */ 
    uint8_t length_memory_address = (address_memory_length & 0xF0) >> 4; 
    /* Low nibble */ 
    uint8_t length_memory_size = address_memory_length & 0x0F; 
    /* Retrieve the memory_address and memory_size based on their lengths */
    std::vector<int> memory_address(stored_data.begin() + 4, stored_data.begin() + 4 + length_memory_address);
    std::vector<int> memory_size(stored_data.begin() + 4 + length_memory_address, stored_data.begin() + 4 + length_memory_address + length_memory_size);

    LOG_INFO(RDSlogger.GET_LOGGER(), "address_memory_length: 0x{0:x}", static_cast<int>(address_memory_length));
    LOG_INFO(RDSlogger.GET_LOGGER(), "length_memory_address: {}", static_cast<int>(length_memory_address));
    LOG_INFO(RDSlogger.GET_LOGGER(), "length_memory_size: {}", static_cast<int>(length_memory_size));

    std::ostringstream dataStream;
    dataStream << "Memory_address: ";
    for (const auto& data : memory_address) 
    {
        dataStream << std::hex << "0x" << static_cast<int>(data) << " ";
    }
    LOG_INFO(RDSlogger.GET_LOGGER(), "{}", dataStream.str());

    dataStream.str("");
    dataStream.clear();

    dataStream << "Memory_size: ";
    for (const auto& data : memory_size) 
    {
        dataStream << std::hex << "0x" << static_cast<int>(data) << " ";
    }
    LOG_INFO(RDSlogger.GET_LOGGER(), "{}", dataStream.str());

    /* Validate memory address and size */ 
    if (!isValidMemoryRange(memory_address, memory_size, RDSlogger)) {
        LOG_INFO(RDSlogger.GET_LOGGER(), "Error: Invalid memory range");
        // Request out of range
        /* sendNegativeResponse(id, 0x31); */
        return;
    }

    /* Check if in programming session */ 
    if (!isInProgrammingSession()) {
        LOG_INFO(RDSlogger.GET_LOGGER(), "Error: Not in Programming Session");
        /* Conditions not correct */ 
        /* sendNegativeResponse(id, 0x22); */
        return;
    }

    /* Authenticate the request */ 
    if (!isRequestAuthenticated(id, data_format_identifier)) {
        LOG_INFO(RDSlogger.GET_LOGGER(), "Error: Authentication failed");
        /* sendNegativeResponse(id, 0x33); // Security access denied */
        return;
    }
    /* This length field indicates how many bytes are used to represent maxNumberofBlockLength */
    uint8_t length_max_number_block = 2;
    /* Assuming 512 bytes as the max length, adjust this based on your server capabilities */ 
    int max_number_block = 512;
    requestDownloadResponse(id, length_max_number_block, max_number_block, RDSlogger);
}

void RequestDownloadService::requestDownloadResponse(int id, uint8_t length_max_number_block, int max_number_block, Logger &RDSlogger)
{
    static std::mutex create_interface_mutex;
    /* uint8_t frame_sender_id= (id >> 8) & 0xFF; */
    uint8_t frame_dest_id = id & 0xFF;
    LOG_INFO(RDSlogger.GET_LOGGER(), "frame id dest: 0x{0:x}", static_cast<int>(frame_dest_id));
    if(frame_dest_id == 0x10)
    {
        std::lock_guard<std::mutex> lock(create_interface_mutex);
        LOG_INFO(RDSlogger.GET_LOGGER(), "log in service");
        /* Construct the response message */ 
        int sid = 0x74;
        /* Assuming 0x54 is the positive response SID for 0x34 service */ 
        std::vector<uint8_t> response = {0x54}; 
        response.push_back(sid);
        response.push_back(length_max_number_block);
        /* High byte of max_number_block */ 
        response.push_back((max_number_block >> 8) & 0xFF);
        /* Low byte of max_number_block */ 
        response.push_back(max_number_block & 0xFF); 
        uint8_t frame_dest_id = (id >> 8) & 0xFF;
        uint8_t frame_sender_id = id & 0xFF;
        id = (frame_sender_id << 8) | frame_dest_id;
        LOG_INFO(RDSlogger.GET_LOGGER(), "frame id 0x{0:x}", static_cast<int>(id));
        generateFrames.sendFrame(id, response, socket, DATA_FRAME);
        return;
    }
}
bool RequestDownloadService::isInProgrammingSession()
{
    /* Logic to check session state */ 
    return true;
}

bool RequestDownloadService::isValidMemoryRange(const std::vector<int>& memory_address, const std::vector<int>& memory_size, Logger &RDSlogger) {
    /* Extract full memory address and size based on their combined byte values */ 
    int full_memory_address = 0;
    int full_memory_size = 0;

    /* Combine memory address bytes into a single integer */ 
    for (size_t i = 0; i < memory_address.size(); ++i) {
        full_memory_address = (full_memory_address << 8) | memory_address[i];
    }

    /* Combine memory size bytes into a single integer */ 
    for (size_t i = 0; i < memory_size.size(); ++i) {
        full_memory_size = (full_memory_size << 8) | memory_size[i];
    }

    /* Validate the full memory address */ 
    if (full_memory_address < 0 || static_cast<unsigned int>(full_memory_address) > 0xFFFFFFFFU) {
        LOG_ERROR(RDSlogger.GET_LOGGER(), "Error: Invalid memory address: 0x{0:x}", full_memory_address);
        return false;
    }

    /* Validate the full memory size */ 
    if (full_memory_size < 1) {
        LOG_ERROR(RDSlogger.GET_LOGGER(), "Error: Invalid memory size: 0x{0:x}", full_memory_size);
        return false;
    }

    LOG_INFO(RDSlogger.GET_LOGGER(), "Validated Memory Address: 0x{0:x}", full_memory_address);
    LOG_INFO(RDSlogger.GET_LOGGER(), "Validated Memory Size: 0x{0:x}", full_memory_size);

    /* Mapping memory inside ECU/MCU logic... */ 
    return true;
}

bool RequestDownloadService::isRequestAuthenticated(uint8_t id, uint8_t dataFormatIdentifier) {
    /* Logic to check request authentication */ 
    return true;
}

