#include "../../../mcu/include/MCUModule.h"
#include "../include/RequestDownload.h"

RequestDownloadService::RequestDownloadService(Logger& RDSlogger)
                        : generateFrames(socket, RDSlogger)
{
   
}
RequestDownloadService::RequestDownloadService(int socket, Logger& RDSlogger)
                        : socket(socket), generateFrames(socket, RDSlogger)
{
    
}
RequestDownloadService::~RequestDownloadService()
{

}

/**RequestDownload- request method 
 * Expected request:  pci_l + sid + data_format_identifier  +  address_memory_lenght + memory_address[] +  memory_size[]               +          download_type
 *  Index               [0]   [1]              [2]                  [3]             [3 + size(memory_adress)]  [3+size(memory_adress+memory_size)]  [3+size(memory_adress+memory_size)+1]
 */
void RequestDownloadService::requestDownloadRequest(int id, std::vector<uint8_t> stored_data, Logger &RDSlogger, DiagnosticSessionControl mcuDiagnosticSessionControl, ReadDataByIdentifier software_version, SecurityAccess logged_in)
{
    LOG_INFO(RDSlogger.GET_LOGGER(), "Service 0x34 RequestDownload");
    /* Extract and switch sender and receiver */
    uint8_t frame_dest_id = (id >> 8) & 0xFF;
    uint8_t frame_sender_id = id  & 0xFF;
    /* Extract and Log the DFI information */
    uint8_t data_format_identifier = stored_data[2];
    LOG_INFO(RDSlogger.GET_LOGGER(), "Data Format Identifier: 0x{0:x}", static_cast<int>(data_format_identifier));
    uint8_t compression_type = (data_format_identifier & 0xF0) >> 4;
    LOG_INFO(RDSlogger.GET_LOGGER(), "Compression Type: 0x{0:x}", static_cast<int>(compression_type));
    uint8_t encryption_type = data_format_identifier & 0x0F;
    LOG_INFO(RDSlogger.GET_LOGGER(), "Encryption Type: 0x{0:x}", static_cast<int>(encryption_type));

    /* Authenticate the request */
    if (!isRequestAuthenticated(logged_in, RDSlogger))
    {
        LOG_ERROR(RDSlogger.GET_LOGGER(), "Error: Authentication failed");
        id = (frame_dest_id << 8) | frame_sender_id;
        /* Not in programming session, authentication required*/
        /* sendNegativeResponse(id, 0x34, 0x34); */
        return;
    }

    /* Check if in programming session */
    if (!isInProgrammingSession(mcuDiagnosticSessionControl, RDSlogger))
    {
        LOG_ERROR(RDSlogger.GET_LOGGER(), "Error: Not in Programming Session");
        id = (frame_dest_id << 8) | frame_sender_id;
        /* Security access denied */
        /* sendNegativeResponse(id, 0x34, 0x33); */
        return;
    }

    /* Check if software is at the latest version */ 
    if (!isLatestSoftwareVersion(software_version, RDSlogger))
    {
        LOG_INFO(RDSlogger.GET_LOGGER(), "Software is not at the latest version");
        id = (frame_dest_id << 8) | frame_sender_id;
        return;
    }

    /* Check for compression */ 
    if (compression_type == 0x0) 
    {
        LOG_INFO(RDSlogger.GET_LOGGER(), "No compression method used");
    } 
    else 
    {
        /** Decompress using tar
         * Decompress stored_data from position 4 onwards (assuming position 2 is DFI)
         * Process decompressed_data as needed
         */ 
    }

    /* Check for encryption */
    if (encryption_type == 0x0)
    {
        LOG_INFO(RDSlogger.GET_LOGGER(), "No encryption method used");
    }
    else
    {
        /* check if encryption is needed */
    }

     /* Retrieve the address_memory_length */
    uint8_t address_memory_length = stored_data[3];
    LOG_INFO(RDSlogger.GET_LOGGER(), "address_memory_length: 0x{0:x}", static_cast<int>(address_memory_length));

    /* Determine the length of memory address and memory size */
    /* High nibble */
    uint8_t length_memory_address = (address_memory_length & 0xF0) >> 4;
    /* Low nibble */
    uint8_t length_memory_size = address_memory_length & 0x0F;
    LOG_INFO(RDSlogger.GET_LOGGER(), "length_memory_address: {}", static_cast<int>(length_memory_address));
    LOG_INFO(RDSlogger.GET_LOGGER(), "length_memory_size: {}", static_cast<int>(length_memory_size));
    /* Ensure there are enough bytes for memory_address and memory_size */ 
    if (stored_data.size() < static_cast<std::vector<unsigned char>::size_type>(1 + length_memory_address + length_memory_size))
    {
        LOG_ERROR(RDSlogger.GET_LOGGER(), "Payload does not contain enough data for memory address and size");
        /* Incorrect message length or invalid format */
        /* sendNegativeResponse(id, 0x34, 0x13); */
    }
    /* Retrieve the memory_address and memory_size based on their lengths */
    std::vector<int> memory_address(stored_data.begin() + 4 , stored_data.begin() + 4 + length_memory_address);
    std::vector<int> memory_size(stored_data.begin() + 4 + length_memory_address, stored_data.begin() + 4 + length_memory_address + length_memory_size);
    
    std::ostringstream dataStream;
    dataStream << "Memory_address: ";
    for (const auto &data : memory_address)
    {
        dataStream << std::hex << "0x" << static_cast<int>(data) << " ";
    }
    LOG_INFO(RDSlogger.GET_LOGGER(), "{}", dataStream.str());

    dataStream.str("");
    dataStream.clear();

    dataStream << "Memory_size: ";
    for (const auto &data : memory_size)
    {
        dataStream << std::hex << "0x" << static_cast<int>(data) << " ";
    }
    LOG_INFO(RDSlogger.GET_LOGGER(), "{}", dataStream.str());

    /* Calculate the position for receiver_id */ 
    uint8_t receiver_id = (id >> 16) & 0xFF;
    LOG_INFO(RDSlogger.GET_LOGGER(), ("receiver_id: 0x{0:x}"), receiver_id);


    /* Calculate the position for manual update 88 or automatic update 89 */ 
    size_t position_download_type = 4 + length_memory_address + length_memory_size;
    uint8_t download_type = 0;
    if (position_download_type < stored_data.size()) 
    {
        download_type = stored_data[position_download_type];
        LOG_INFO(RDSlogger.GET_LOGGER(), "download_type: 0x{:02X}", download_type);
        if (download_type != 0x88 && download_type != 0x89) {
            LOG_ERROR(RDSlogger.GET_LOGGER(), "Error: Invalid download type: {}", static_cast<uint8_t>(download_type));
        }
    }  

    /* Validate memory address and size */
    if (!isValidMemoryRange(memory_address, memory_size, RDSlogger))
    {
        LOG_ERROR(RDSlogger.GET_LOGGER(), "Error: Invalid memory range");
        /* Request out of range */
        /* sendNegativeResponse(id, 0x34, 0x31); */
        return;
    }

    /* Assuming 512 bytes as the max length */
    int block_size = 512;
    /* Initialize max_number_block with a value that ensures it will be updated */
    int max_number_block = 0;

    /* Calculate max_number_block as the maximum ceiling of memory_size divided by block_size */
    for (uint8_t byte = 0; byte < memory_size.size(); ++byte)
    {
        int blocks_needed = (memory_size[byte] + block_size - 1) / block_size;
        LOG_INFO(RDSlogger.GET_LOGGER(), "blocks_needed:{}", blocks_needed);
        if (blocks_needed > max_number_block)
        {
            max_number_block = blocks_needed;
        }
    }
    LOG_INFO(RDSlogger.GET_LOGGER(), "max_number_block:{}", max_number_block);
    if (download_type == 0x88)
    {
        requestDownloadResp(id, max_number_block, RDSlogger);
    }

    /*Download from drive- part 3*/

    if(download_type == 0x89)
    {
        if (receiver_id == 0x10)
        {
            LOG_INFO(RDSlogger.GET_LOGGER(), "Map memory in MCU and transfer data");
            /* Map memory in MCU -Set adress vector-> send to Install for mapping data */
            /* mapMemoryMCU(memory_address)*/
            id = (frame_dest_id << 8) | receiver_id;
            /* routine for transfer first or second partition */
        }
        else if (receiver_id == 0x11)
        {
            LOG_INFO(RDSlogger.GET_LOGGER(), "Map memory in ECU and transfer data");
            /* Map memory in ECU -Set adress vector-> send to Install for mapping data */
            /* mapMemoryECU(memory_address)*/
            id = (frame_dest_id << 8) | receiver_id;
            for (uint16_t block_number = 0; block_number < max_number_block; ++block_number)
            {
            /* Calculate block_sequence_counter for the current block */ 
            uint16_t block_sequence_counter = block_number;
            LOG_INFO(RDSlogger.GET_LOGGER(), "block_sequence_counter: {}", block_sequence_counter);
            /* Call transferData with the current block_sequence_counter */ 
            /* transferDataRequest(id, block_sequence_counter, transfer_request); */
            }
        }
    } 
}

/** RequestDownload -response handle --to be implemented 
 *  Expected response: pci_l + sid +  length_max_number_block*0x10  +  max_number_block
 *  Index              [0]     [1]         [2]                       [3]
 */
void RequestDownloadService::requestDownloadResp(int id, int max_number_block, Logger &RDSlogger)
{
    static std::mutex create_interface_mutex;
    /* Rename destination in sender and viceversa */
    uint8_t frame_sender_id = (id >> 8) & 0xFF;
    uint8_t frame_dest_id = id & 0xFF;
    LOG_INFO(RDSlogger.GET_LOGGER(), "frame id dest: 0x{0:x}", static_cast<int>(frame_dest_id));
    /* Check if frame is intended for MCU */
    if(frame_dest_id == 0x10)
    {
        std::lock_guard<std::mutex> lock(create_interface_mutex);
        LOG_INFO(RDSlogger.GET_LOGGER(), "log in service");
        id = (frame_dest_id << 8) | (frame_sender_id);
        LOG_INFO(RDSlogger.GET_LOGGER(), "frame id 0x{0:x}", static_cast<int>(id));
        LOG_INFO(RDSlogger.GET_LOGGER(), "max no block {}", static_cast<int>(max_number_block));
        /* Call response method from GenerateFrames */
        generateFrames.requestDownloadResponse(id, max_number_block);
        return;
    }
    else
    {
        std::cerr << "Create interface is nullptr" << std::endl;
    }
    
}

bool RequestDownloadService::isInProgrammingSession(DiagnosticSessionControl mcuDiagnosticSessionControl, Logger &RDSlogger)
{
    /* Logic to check session state, return true until future implementation */
    return true; 
    DiagnosticSession current_session = mcuDiagnosticSessionControl.getCurrentSession();
    /* Compare the current session with PROGRAMMING_SESSION */ 
    if (current_session == PROGRAMMING_SESSION) 
    {
        LOG_INFO(RDSlogger.GET_LOGGER(), "The current session is:{}", mcuDiagnosticSessionControl.getCurrentSessionToString());
        return true; 
    }
    
} 

bool RequestDownloadService::isValidMemoryRange(const std::vector<int> &memory_address, const std::vector<int> &memory_size, Logger &RDSlogger)
{
    /* Extract full memory address and size based on their combined byte values */
    int full_memory_address = 0;
    int full_memory_size = 0;

    /* Combine memory address bytes into a single integer */
    for (size_t i = 0; i < memory_address.size(); ++i)
    {
        full_memory_address = (full_memory_address << 8) | memory_address[i];
    }

    /* Combine memory size bytes into a single integer */
    for (size_t i = 0; i < memory_size.size(); ++i)
    {
        full_memory_size = (full_memory_size << 8) | memory_size[i];
    }

    /* Validate the full memory address */
    if (full_memory_address < 0 || (static_cast<unsigned int>(full_memory_address) > 0xFFFFFFFFU))
    {
        LOG_ERROR(RDSlogger.GET_LOGGER(), "Error: Invalid memory address: 0x{0:x}", full_memory_address);
        /* Request out of range -Invalid memory address */
        /* sendNegativeResponse(id, 0x34, 0x31); */
        return false;
    }

    /* Validate the full memory size */
    if (full_memory_size < 1)
    {
        LOG_ERROR(RDSlogger.GET_LOGGER(), "Error: Invalid memory size: 0x{0:x}", full_memory_size);
        /* Request out of range -Invalid memory size*/
        /* sendNegativeResponse(id, 0x34, 0x31); */
        return false;
    }

    LOG_INFO(RDSlogger.GET_LOGGER(), "Validated Memory Address: 0x{0:x}", full_memory_address);
    LOG_INFO(RDSlogger.GET_LOGGER(), "Validated Memory Size: 0x{0:x}", full_memory_size);

    return true;
}

bool RequestDownloadService::isRequestAuthenticated(SecurityAccess logged_in, Logger &RDSlogger)
{
    /* Logic to check security access */
    
    LOG_INFO(RDSlogger.GET_LOGGER(), "MCU authentication state");
    return true;
    /** bool mcu_state = logged_in.getMCUState();
    * Check MCU state if unlocked 
    * if (mcu_state == 0x00) 
    * {
    *   LOG_INFO(RDSlogger.GET_LOGGER(), "MCU authentication state: {}", mcu_state);
    *   return true; 
    * }
    */
}

bool RequestDownloadService::isLatestSoftwareVersion(ReadDataByIdentifier software_version, Logger &RDSlogger)
{
    /* Logic to check version */
    
    LOG_INFO(RDSlogger.GET_LOGGER(), "Check software version");
    return true;
    /**std::vector<uint8_t> current_version = software_version.readDataByIdentifier();
    * if (current_version == my_version) 
    * {
    *    return true; 
    * }
    */
}