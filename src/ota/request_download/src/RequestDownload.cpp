#include "../../../mcu/include/MCUModule.h"
#include "../include/RequestDownload.h"

RequestDownloadService::RequestDownloadService(Logger& RDSlogger)
                        : generateFrames(socket, RDSlogger)
{
   this->RDSlogger = RDSlogger;
}
RequestDownloadService::RequestDownloadService(int socket, Logger& RDSlogger)
                        : socket(socket), generateFrames(socket, RDSlogger)
{
    this->RDSlogger = RDSlogger;
}
RequestDownloadService::~RequestDownloadService()
{

}

/**RequestDownload- request method 
 * Expected request:  pci_l + sid + data_format_identifier  +  address_memory_lenght + memory_address[] +  memory_size[]               +          download_type
 *  Index               [0]   [1]              [2]                  [3]             [3 + size(memory_adress)]  [3+size(memory_adress+memory_size)]  [3+size(memory_adress+memory_size)+1]
 */
void RequestDownloadService::requestDownloadRequest(int id, std::vector<uint8_t> stored_data)
{
    LOG_INFO(RDSlogger.GET_LOGGER(), "Service 0x34 RequestDownload");
    /* Extract and switch sender and receiver */
    uint8_t frame_dest_id = (id >> 8) & 0xFF;
    uint8_t frame_sender_id = id  & 0xFF;
    int new_id = (frame_dest_id << 8) | frame_sender_id;
    /* Extract and Log the DFI information */
    uint8_t data_format_identifier = stored_data[2];
    LOG_DEBUG(RDSlogger.GET_LOGGER(), "Data Format Identifier: 0x{0:x}", static_cast<int>(data_format_identifier));
    uint8_t compression_type = (data_format_identifier & 0xF0) >> 4;
    LOG_DEBUG(RDSlogger.GET_LOGGER(), "Compression Type: 0x{0:x}", static_cast<int>(compression_type));
    uint8_t encryption_type = data_format_identifier & 0x0F;
    LOG_DEBUG(RDSlogger.GET_LOGGER(), "Encryption Type: 0x{0:x}", static_cast<int>(encryption_type));

    /* Authenticate the request */
    if (!isRequestAuthenticated())
    {
        LOG_ERROR(RDSlogger.GET_LOGGER(), "Error: Authentication failed");
        id = new_id;
        /* Not in programming session, authentication required*/
        /* sendNegativeResponse(id, 0x34, 0x34); */
        return;
    }

    /* Check if in programming session */
    if (!isInProgrammingSession())
    {
        LOG_ERROR(RDSlogger.GET_LOGGER(), "Error: Not in Programming Session");
        id = new_id;
        /* Security access denied */
        /* sendNegativeResponse(id, 0x34, 0x33); */
        return;
    }

    /* Check if software is at the latest version */ 
    if (!isLatestSoftwareVersion())
    {
        LOG_INFO(RDSlogger.GET_LOGGER(), "Software is not at the latest version");
        id = new_id;
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

    std::pair<int,int> address_and_size_length = extractSizeAndAddressLength(stored_data);
    int length_memory_address = address_and_size_length.first;
    int length_memory_size = address_and_size_length.second;

    
    std::pair<int,int> address_and_size =extractSizeAndAddress(stored_data,length_memory_address,length_memory_size );
    int memory_address = address_and_size.first;
    int memory_size = address_and_size.second;
    /* Calculate the position for manual update 88 or automatic update 89 */ 
    size_t position_download_type = 4 + length_memory_address + length_memory_size;
    uint8_t download_type = 0;
    if (position_download_type < stored_data.size()) 
    {
        download_type = stored_data[position_download_type];
        LOG_DEBUG(RDSlogger.GET_LOGGER(), "download_type: 0x{:02X}", download_type);
        if (download_type != 0x88 && download_type != 0x89) {
            LOG_ERROR(RDSlogger.GET_LOGGER(), "Error: Invalid download type: {}", static_cast<uint8_t>(download_type));
        }
    }  

    /* Validate memory address and size */
    if (!isValidMemoryRange(memory_address, memory_size))
    {
        LOG_ERROR(RDSlogger.GET_LOGGER(), "Error: Invalid memory range");
        /* Request out of range */
        /* sendNegativeResponse(id, 0x34, 0x31); */
        return;
    }

    int max_number_block = calculate_max_number_block(memory_size);

    download();

    if (download_type == 0x88)
    {
        requestDownloadResp(id, memory_address, max_number_block);
        return;
    }
    else if ( download_type == 0x89 )
    {
        requestDownloadResp89(id, memory_address, max_number_block);
        return;
    }
    /* Send frame error */
}

void RequestDownloadService::download()
{
    /* run here python script to download from drive */
}
int RequestDownloadService::calculate_max_number_block(int memory_size)
{
    /* Assuming 512 bytes as the max length */
    int block_size = 512;
    /* Initialize max_number_block with a value that ensures it will be updated */
    int max_number_block = 0;

    /* HOW IS CALCULATED 'max_number_block'??? */
    /* Calculate max_number_block as the maximum ceiling of memory_size divided by block_size */
    int blocks_needed = (memory_size + block_size - 1) / block_size;
    LOG_DEBUG(RDSlogger.GET_LOGGER(), "blocks_needed:{}", blocks_needed);
    
    // Update max_number_block with the calculated blocks_needed
    max_number_block = blocks_needed;

    LOG_DEBUG(RDSlogger.GET_LOGGER(), "max_number_block:{}", max_number_block);
    return 1;
}

void RequestDownloadService::requestDownloadResp89(int id, int memory_address, int max_number_block)
{
    /*Download from drive- part 3*/
    std::string path = "";

    int receiver_id = id & 0xFF;
    if ( receiver_id == 0x10)
    {
        LOG_INFO(RDSlogger.GET_LOGGER(), "Map memory in MCU and transfer data");
        /* Map memory in MCU -Set adress vector-> send to Install for mapping data */
        
        MemoryManager* managerInstance = MemoryManager::getInstance(memory_address, path, &MCULogger);
        managerInstance->getAddress();
        /* routine for transfer first or second partition */
    }
    else if (receiver_id == 0x11)
    {
        downloadInEcu(id,memory_address);
    }
}

void RequestDownloadService::downloadInEcu(int id, int memory_address)
{
    int receiver_id = id & 0xFF;
    int new_id = 0x1000 + receiver_id;
    LOG_INFO(RDSlogger.GET_LOGGER(), "Map memory in ECU and transfer data");
    /* Map memory in ECU -Set adress vector-> send to Install for mapping data */

    std::string path_download = "";

    std::vector<uint8_t> data = MemoryManager::readBinary(path_download);

    generateFrames.requestDownload(new_id,0x00,memory_address,data.size(),0x88);
    struct can_frame* frame = read_frame(new_id, 0x34);
    int max_number_block;
    if (frame != NULL)
    {
        max_number_block = frame->data[4]; // dummy value 4
    }
    else
    {
        /* generate error frame*/
        return;
    }
    
    for (std::vector<unsigned char>::size_type block_sequence_counter = 0; block_sequence_counter * max_number_block < data.size(); ++block_sequence_counter)
    {
        std::vector<uint8_t> data_to_send;
        data_to_send.insert(data_to_send.begin(), data.begin() + (max_number_block * block_sequence_counter), data.begin() + ((max_number_block * block_sequence_counter + 1)));
        /* Calculate block_sequence_counter for the current block */ 
        LOG_DEBUG(RDSlogger.GET_LOGGER(), "block_sequence_counter: {}", block_sequence_counter);
        /* Call transferData with the current block_sequence_counter */ 
        if ( data_to_send.size() >  5)
        {
            generateFrames.transferDataLong(new_id,block_sequence_counter,data_to_send);
            /* read flow controll frame */
            frame = read_frame(new_id, -0x10);
            if (frame == NULL)
            {
                /* generate error frame*/
                return;
            }
            generateFrames.transferDataLong(new_id,block_sequence_counter,data_to_send, false);
        }
        else
        {
            generateFrames.transferData(new_id,block_sequence_counter,data_to_send);
        }
    }
    frame = read_frame(new_id, 0x36);
    if (frame == NULL || frame->data[1] == 0x7F)
    {
        /* generate error frame*/
        return;
    }
    generateFrames.requestTransferExit(new_id);
    frame = read_frame(new_id, 0x37);
    if (frame == NULL || frame->data[1] == 0x7F)
    {
        /* generate error frame*/
        return;
    }
}

can_frame* RequestDownloadService::read_frame(int id, uint8_t sid)
{
    struct pollfd pfd;
    /* Set the socket file descriptor */ 
    pfd.fd = this->socket; 
    /* We are interested in read events -use POLLING */ 
    pfd.events = POLLIN;

    auto start = std::chrono::system_clock::now();
    auto end = std::chrono::system_clock::now();
    LOG_INFO(RDSlogger.GET_LOGGER(), "Waiting 3 sec until recive flow control frame...");
    while (true)
    {
        /* Use poll to wait for data to be available with a timeout of 1000ms (1 second) */ 
        int poll_result = poll(&pfd, 1, 1000);

        if (poll_result > 0 && pfd.revents & POLLIN) 
        {
            can_frame* frame = new can_frame;
            int nbytes = read(this->socket, frame, sizeof(*frame));

            if (nbytes > 0) 
            {
                if (((frame->can_id & 0xFF) == 0x10) && (frame->data[1] == sid+0x40))
                {
                    return frame;  
                }
                /* Clean up if frame is not used */ 
                delete frame;  
            }
        }
        end = std::chrono::system_clock::now();
        /* Time out of 3 sec */
        if((std::chrono::duration_cast<std::chrono::seconds>(end - start).count() > 3)) 
            break;
    }
    return nullptr;
}

/** RequestDownload -response handle --to be implemented 
 *  Expected response: pci_l + sid +  length_max_number_block*0x10  +  max_number_block
 *  Index              [0]     [1]         [2]                       [3]
 */
void RequestDownloadService::requestDownloadResp(int id, int memory_address, int max_number_block)
{
    std::string path = "";
    /* Rename destination in sender and viceversa */
    uint8_t frame_sender_id = (id >> 8) & 0xFF;
    uint8_t frame_dest_id = id & 0xFF;
    LOG_DEBUG(RDSlogger.GET_LOGGER(), "frame id dest: 0x{0:x}", static_cast<int>(frame_dest_id));
    /* Check if frame is intended for MCU */
    if(frame_dest_id == 0x10)
    {
        MemoryManager* managerInstance = MemoryManager::getInstance(memory_address, path, &MCULogger);
        managerInstance->getAddress();
        LOG_DEBUG(RDSlogger.GET_LOGGER(), "log in service");
        id = (frame_dest_id << 8) | (frame_sender_id);
        LOG_DEBUG(RDSlogger.GET_LOGGER(), "frame id 0x{0:x}", static_cast<int>(id));
        LOG_DEBUG(RDSlogger.GET_LOGGER(), "max no block {}", static_cast<int>(max_number_block));
        /* Call response method from GenerateFrames */
        generateFrames.requestDownloadResponse(id, max_number_block);
        return;
    }
    else
    {
        std::cerr << "Create interface is nullptr" << std::endl;
    }
}

std::pair<int,int> RequestDownloadService::extractSizeAndAddressLength( std::vector<uint8_t> stored_data)
{
     /* Retrieve the address_memory_length */
    uint8_t address_memory_length = stored_data[3];
    LOG_DEBUG(RDSlogger.GET_LOGGER(), "address_memory_length: 0x{0:x}", static_cast<int>(address_memory_length));

    /* Determine the length of memory address and memory size */
    /* High nibble */
    uint8_t length_memory_address = (address_memory_length & 0xF0) >> 4;
    /* Low nibble */
    uint8_t length_memory_size = address_memory_length & 0x0F;
    LOG_DEBUG(RDSlogger.GET_LOGGER(), "length_memory_address: {}", static_cast<int>(length_memory_address));
    LOG_DEBUG(RDSlogger.GET_LOGGER(), "length_memory_size: {}", static_cast<int>(length_memory_size));
    /* Ensure there are enough bytes for memory_address and memory_size */ 
    if (stored_data.size() < static_cast<std::vector<unsigned char>::size_type>(1 + length_memory_address + length_memory_size))
    {
        LOG_ERROR(RDSlogger.GET_LOGGER(), "Payload does not contain enough data for memory address and size");
        /* Incorrect message length or invalid format */
        /* sendNegativeResponse(id, 0x34, 0x13); */
    }
    return {length_memory_address, length_memory_size};
}

std::pair<int,int> RequestDownloadService::extractSizeAndAddress( std::vector<uint8_t> stored_data, uint8_t length_memory_address, uint8_t length_memory_size )
{
    /* Retrieve the memory_address and memory_size based on their lengths */
    std::vector<int> memory_address(stored_data.begin() + 4 , stored_data.begin() + 4 + length_memory_address);
    std::vector<int> memory_size(stored_data.begin() + 4 + length_memory_address, stored_data.begin() + 4 + length_memory_address + length_memory_size);
    
    std::ostringstream dataStream;
    dataStream << "Memory_address: ";
    for (const auto &data : memory_address)
    {
        dataStream << std::hex << "0x" << static_cast<int>(data) << " ";
    }
    LOG_DEBUG(RDSlogger.GET_LOGGER(), "{}", dataStream.str());

    dataStream.str("");
    dataStream.clear();

    dataStream << "Memory_size: ";
    for (const auto &data : memory_size)
    {
        dataStream << std::hex << "0x" << static_cast<int>(data) << " ";
    }
    LOG_DEBUG(RDSlogger.GET_LOGGER(), "{}", dataStream.str());

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

    return {full_memory_address, full_memory_size};
}

bool RequestDownloadService::isInProgrammingSession()
{
    /* Logic to check session state, return true until future implementation of DiagnosticSessionControl */
    return true;
    // DiagnosticSession current_session = DiagnosticSessionControl::getCurrentSession();
    // LOG_INFO(RDSlogger.GET_LOGGER(), "The current session is:{}", DiagnosticSessionControl::getCurrentSessionToString());
    // if (current_session == PROGRAMMING_SESSION)
    // {
    //    return true;
    // }
    // return false;
} 

bool RequestDownloadService::isValidMemoryRange(const int &memory_address, const int &memory_size)
{
    /* Validate the full memory address */
    if (memory_address < 0 || (static_cast<unsigned int>(memory_address) > 0xFFFFFFFFU))
    {
        LOG_ERROR(RDSlogger.GET_LOGGER(), "Error: Invalid memory address: 0x{0:x}", memory_address);
        /* Request out of range -Invalid memory address */
        /* sendNegativeResponse(id, 0x34, 0x31); */
        return false;
    }

    /* Validate the full memory size */
    if (memory_size < 1)
    {
        LOG_ERROR(RDSlogger.GET_LOGGER(), "Error: Invalid memory size: 0x{0:x}", memory_size);
        /* Request out of range -Invalid memory size*/
        /* sendNegativeResponse(id, 0x34, 0x31); */
        return false;
    }

    LOG_DEBUG(RDSlogger.GET_LOGGER(), "Validated Memory Address: 0x{0:x}", memory_address);
    LOG_DEBUG(RDSlogger.GET_LOGGER(), "Validated Memory Size: 0x{0:x}", memory_size);

    return true;
}

bool RequestDownloadService::isRequestAuthenticated()
{
    /* Logic to check security access */
    
    LOG_INFO(RDSlogger.GET_LOGGER(), "MCU authentication state");
    return true;
    bool is_authenticated = SecurityAccess::getMcuState();
    /* Check MCU state if unlocked */
    if (is_authenticated) 
    {
        LOG_DEBUG(RDSlogger.GET_LOGGER(), "MCU authentication state: {}", is_authenticated);
    }
    return is_authenticated;
}

bool RequestDownloadService::isLatestSoftwareVersion()
{
    /* Logic to check version */
    return true;
    ReadDataByIdentifier software_version(this->socket, this->RDSlogger);
    LOG_INFO(RDSlogger.GET_LOGGER(), "Check software version");
    uint16_t IDENTIFIER_SOFTWARE = 0x1234; //dummy
    std::vector<uint8_t> current_version = software_version.readDataByIdentifier(0x0,{0x0,0x22, uint8_t(IDENTIFIER_SOFTWARE / 0x100), uint8_t(IDENTIFIER_SOFTWARE % 0x100)},false );
    int my_version = 1; // dummy
    int current_version_int = 0;
    for ( uint8_t byte : current_version)
    {
        current_version_int = current_version_int * 0x100 + byte;
    }
    if (current_version_int == my_version) 
    {
       return true; 
    }
    return false;
}

/** Use libraries for tar
 * #include <archive.h>
 * #include <archive_entry.h>
 * Add flag -larchive to makefiles LDFLAGS
 */
/**Methods for decompress using TAR: To be included in .h
 * std::vector<uint8_t> decompressTar(const std::vector<uint8_t>& tarData);
 * std::vector<uint8_t> processPayload(const std::vector<uint8_t> &stored_data, uint8_t compression_type, uint8_t start_index, Logger& RDSlogger);
 */

/**Method implementation for decompress using TAR, To be included in .cpp
 * std::vector<uint8_t> RequestDownloadService::decompressTar(const std::vector<uint8_t> &tarData)
 * {
 *  struct archive *archive;
 *  struct archive_entry *entry;
 *  std::vector<uint8_t> decompressedData;
 *
 *  archive = archive_read_new();
 *  archive_read_support_format_tar(archive);
 *
 *  if (archive_read_open_memory(archive, tarData.data(), tarData.size()) != ARCHIVE_OK)
 *  {
 *      throw std::runtime_error("Failed to open tar archive");
 *   }
 *
 *  while (archive_read_next_header(archive, &entry) == ARCHIVE_OK)
 *  {
 *      size_t size = archive_entry_size(entry);
 *      std::vector<uint8_t> buffer(size);
 *      if (archive_read_data(archive, buffer.data(), size) < 0)
 *      {
 *          throw std::runtime_error("Failed to read data from tar archive");
 *      }
 *
 *      decompressedData.insert(decompressedData.end(), buffer.begin(), buffer.end());
 *  }
 *
 *  archive_read_free(archive);
 *  return decompressedData;
 * }
 *
 * std::vector<uint8_t> RequestDownloadService::processPayload(const std::vector<uint8_t> &stored_data, uint8_t compression_type, uint8_t start_index, Logger &RDSlogger)
 * {
 *  LOG_INFO(RDSlogger.GET_LOGGER(), "Compression Type: 0x{0:x}", static_cast<int>(compression_type));
 *  std::vector<uint8_t> payload(stored_data.begin() + start_index, stored_data.end());
 *  if (compression_type == 0x1)
 *  {
 *      return decompressTar(payload);
 *  }
 *  else if (compression_type == 0x0)
 *  {
 *  LOG_INFO(RDSlogger.GET_LOGGER(), "No compression method used");
 *  }
 *  else
 *  LOG_ERROR(RDSlogger.GET_LOGGER(), "Unknown compression method used");
 *   sendNegativeResponse(id, 0x34, 0x31); // Request out of range -the specified dataFormatIdentifier is not valid
 *  return payload;
 * }
 */
