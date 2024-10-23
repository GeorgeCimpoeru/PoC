#include "../../../mcu/include/MCUModule.h"
#include "../include/RequestDownload.h"
#include "../../../ecu_simulation/BatteryModule/include/BatteryModule.h"

RDSData RequestDownloadService::rds_data = {0, 0, 0, 0};

RequestDownloadService::RequestDownloadService(Logger& RDSlogger)
                        : RDSlogger(RDSlogger), generate_frames(socket, RDSlogger)
{

}
RequestDownloadService::RequestDownloadService(int socket, Logger& RDSlogger)
                        : socket(socket), RDSlogger(RDSlogger), generate_frames(socket, RDSlogger)
{

}
RequestDownloadService::~RequestDownloadService()
{

}

/**RequestDownload- request method 
 * Expected request:  pci_l + sid + data_format_identifier  +  address_memory_length + memory_address[] +  memory_size[] + software_version
 *  Index               [0]   [1]              [2]                  [3]                     [4,5]             [6]               [7]                        
 */
void RequestDownloadService::requestDownloadRequest(canid_t id, std::vector<uint8_t> stored_data)
{
    NegativeResponse nrc(socket, RDSlogger);
    LOG_INFO(RDSlogger.GET_LOGGER(), "Service 0x34 RequestDownload");
    auto ota_status = MCU::mcu->getDidValue(OTA_UPDATE_STATUS_DID)[0];
    /* This will be replaced by OTA Session */
    /* Extract and switch sender and receiver */
    uint8_t receiver_id = id  & 0xFF;
    uint8_t sender_id = (id >> 8) & 0xFF;
    /* target id represents the ecu needed to be updated, if 0 then target will be mcu */
    /* it is 1 byte and can be found between bit 16 and 23 in canid_t */
    uint8_t target_id = (id >> 16) & 0xFF;
    /* Reverse IDs, target id will be in same position but receiver will be switched with sender */
    id = (target_id << 16) | (receiver_id << 8) | sender_id;

    if (receiver_id == 0x10 && !SecurityAccess::getMcuState(RDSlogger))
    {
        /* Authentication failed */
        nrc.sendNRC(id, RDS_SID, NegativeResponse::SAD);
        AccessTimingParameter::stopTimingFlag(receiver_id, 0x34);
        return;
    }

    if ((receiver_id == 0x11 || receiver_id == 0x12 ||
         receiver_id == 0x13 || receiver_id == 0x14) &&
         !ReceiveFrames::getEcuState())
    {
        /* Authentication failed */
        nrc.sendNRC(id, RDS_SID, NegativeResponse::SAD);
        AccessTimingParameter::stopTimingFlag(receiver_id, 0x34);
        return;
    }

    if (stored_data.size() < 7)
    {
        /* Incorrect message length or invalid format - prepare a negative response */
        nrc.sendNRC(id, RDS_SID, NegativeResponse::IMLOIF);
        AccessTimingParameter::stopTimingFlag(receiver_id, 0x34);
        return;
    }

    if(ota_status != IDLE && ota_status != INIT)
    {
        LOG_WARN(RDSlogger.GET_LOGGER(), "Service 0x34 RequestDownload can be used only from an INIT or IDLE state, current OTA state is {:x}", ota_status);
        nrc.sendNRC(id, RDS_SID, NegativeResponse::CNC);
        AccessTimingParameter::stopTimingFlag(receiver_id, 0x34);
        return;
    }
    MCU::mcu->setDidValue(OTA_UPDATE_STATUS_DID, {WAIT});

    /** data format identifier is 0x00 when no compression or encryption method is used 
     * 0x11 when both compression and encryption are used
     * 0x01 when only encryption is used
     * 0x10 when only compression is used
     * we can define more values later if needed
     */
    std::unordered_set<uint8_t> valid_data_format_indentifiers = {0x00, 0x01, 0x10, 0x11};
    if (valid_data_format_indentifiers.find(stored_data[2]) == valid_data_format_indentifiers.end())
    {
        /* Request out of range - prepare a negative response */
        nrc.sendNRC(id, RDS_SID, NegativeResponse::ROOR);
        AccessTimingParameter::stopTimingFlag(receiver_id, 0x34);
        return;
    }

    /* extract method returns IMLOIF if full length check fails */
    std::pair<int,int> address_and_size_length = extractSizeAndAddressLength(id, stored_data);
    int length_memory_address = address_and_size_length.first;
    int length_memory_size = address_and_size_length.second;

    std::pair<int,int> address_and_size = extractSizeAndAddress(stored_data, length_memory_address, length_memory_size);
    int memory_address = address_and_size.first;
    RequestDownloadService::rds_data.address = memory_address;
    int memory_size = address_and_size.second;
    RequestDownloadService::rds_data.size = memory_size;

    /* Validate memory address and size */
    if (!isValidMemoryRange(memory_address, memory_size))
    {
        LOG_ERROR(RDSlogger.GET_LOGGER(), "Error: Invalid memory range");
        /* Request out of range */
        nrc.sendNRC(id, RDS_SID, NegativeResponse::ROOR);
        MCU::mcu->setDidValue(OTA_UPDATE_STATUS_DID, {WAIT_DOWNLOAD_FAILED});
        AccessTimingParameter::stopTimingFlag(receiver_id, 0x34);
        return;
    }
    /* Extract and Log the data_format_identifier information */
    uint8_t data_format_identifier = stored_data[2];
    RequestDownloadService::rds_data.data_format = data_format_identifier;
    LOG_INFO(RDSlogger.GET_LOGGER(), "Data Format Identifier: 0x{0:x}", static_cast<int>(data_format_identifier));
    uint8_t compression_type = (data_format_identifier & 0xF0) >> 4;
    LOG_INFO(RDSlogger.GET_LOGGER(), "Compression Type: 0x{0:x}", static_cast<int>(compression_type));
    uint8_t encryption_type = data_format_identifier & 0x0F;
    LOG_INFO(RDSlogger.GET_LOGGER(), "Encryption Type: 0x{0:x}", static_cast<int>(encryption_type));

    /* Check if software is at the latest version */ 
    if (!isLatestSoftwareVersion())
    {
        LOG_INFO(RDSlogger.GET_LOGGER(), "Software is not at the latest version");
        MCU::mcu->setDidValue(OTA_UPDATE_STATUS_DID, {WAIT_DOWNLOAD_FAILED});
        AccessTimingParameter::stopTimingFlag(receiver_id, 0x34);
        return;
    }

    RequestDownloadService::rds_data.max_number_block = calculate_max_number_block(memory_size);
    requestDownloadResponse(id, memory_address, RequestDownloadService::rds_data.max_number_block);
    AccessTimingParameter::stopTimingFlag(receiver_id, 0x34);
    return;
}

int RequestDownloadService::calculate_max_number_block(int memory_size)
{
    /* max_number_block = maximum number of bytes for 1 transfer data 
        This is set to 5 because in a transfer data we can send only 5 bytes of data (8 - pci - sid - blc_indx)
    */
    int max_number_block = MAX_TRANSER_DATA_BYTES;
    return max_number_block;    
}

void RequestDownloadService::requestDownloadAutomatic(canid_t id, int memory_address, int max_number_block)
{
    /* Download from drive- part 3 */
    std::string path = "";

    int receiver_id = id & 0xFF;
    if ( receiver_id == 0x10)
    {
        LOG_INFO(RDSlogger.GET_LOGGER(), "Map memory in MCU and transfer data");
        /* Map memory in MCU -Set adress vector-> send to Install for mapping data */
        
        MemoryManager* managerInstance = MemoryManager::getInstance(memory_address, path, RDSlogger);
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

    std::vector<uint8_t> data = MemoryManager::readBinary(path_download, RDSlogger);

    generate_frames.requestDownload(new_id,0x00,memory_address,data.size(),0x88);
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
        LOG_INFO(RDSlogger.GET_LOGGER(), "block_sequence_counter: {}", block_sequence_counter);
        /* Call transferData with the current block_sequence_counter */ 
        if ( data_to_send.size() >  5)
        {
            generate_frames.transferDataLong(new_id,block_sequence_counter,data_to_send);
            /* read flow control frame */
            frame = read_frame(new_id, -0x10);
            if (frame == NULL)
            {
                /* generate error frame*/
                return;
            }
            generate_frames.transferDataLong(new_id,block_sequence_counter,data_to_send, false);
        }
        else
        {
            generate_frames.transferData(new_id,block_sequence_counter,data_to_send);
        }
    }
    frame = read_frame(new_id, 0x36);
    if (frame == NULL || frame->data[1] == 0x7F)
    {
        /* generate error frame*/
        return;
    }
    generate_frames.requestTransferExit(new_id);
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
void RequestDownloadService::requestDownloadResponse(canid_t id, int memory_address, int max_number_block)
{
    /* this path is temporary and differs on each VM */
    std::string path = DEV_LOOP;
    LOG_INFO(RDSlogger.GET_LOGGER(), "memory adress: 0x{0:x}", static_cast<int>(memory_address));

    MemoryManager* managerInstance = MemoryManager::getInstance(memory_address, path, RDSlogger);
    managerInstance->getAddress();
    /* Call response method from generate_frames */
    generate_frames.requestDownloadResponse(id, max_number_block);
    MCU::mcu->setDidValue(OTA_UPDATE_STATUS_DID, {WAIT_DOWNLOAD_COMPLETED});
    return;
}

std::pair<int,int> RequestDownloadService::extractSizeAndAddressLength(canid_t id, std::vector<uint8_t> stored_data)
{
     /* Retrieve the address_memory_length */
    uint8_t address_memory_length = stored_data[3];
    LOG_INFO(RDSlogger.GET_LOGGER(), "address_memory_length: 0x{0:x}", address_memory_length);

    /* Determine the length of memory address and memory size */
    /* High nibble */
    uint8_t length_memory_address = (address_memory_length & 0xF0) >> 4;
    /* Low nibble */
    uint8_t length_memory_size = address_memory_length & 0x0F;
    LOG_INFO(RDSlogger.GET_LOGGER(), "length_memory_address: {}", length_memory_address);
    LOG_INFO(RDSlogger.GET_LOGGER(), "length_memory_size: {}", length_memory_size);
    /* Ensure there are enough bytes for memory_address and memory_size */ 
    if (stored_data.size() < static_cast<std::vector<unsigned char>::size_type>(1 + length_memory_address + length_memory_size))
    {
        LOG_ERROR(RDSlogger.GET_LOGGER(), "Payload does not contain enough data for memory address and size");
        /* Incorrect message length or invalid format */
        NegativeResponse nrc(socket, RDSlogger);
        nrc.sendNRC(id, RDS_SID, NegativeResponse::IMLOIF);
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
    LOG_INFO(RDSlogger.GET_LOGGER(), "{}", dataStream.str());

    dataStream.str("");
    dataStream.clear();

    dataStream << "Memory_size: ";
    for (const auto &data : memory_size)
    {
        dataStream << std::hex << "0x" << static_cast<int>(data) << " ";
    }
    LOG_INFO(RDSlogger.GET_LOGGER(), "{}", dataStream.str());

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

bool RequestDownloadService::isValidMemoryRange(const int &memory_address, const int &memory_size)
{
    /* Validate the full memory address */
    if (memory_address < DEV_LOOP_PARTITION_1_ADDRESS_START || (static_cast<unsigned int>(memory_address) > DEV_LOOP_PARTITION_2_ADDRESS_END))
    {
        LOG_ERROR(RDSlogger.GET_LOGGER(), "Error: Invalid memory address: 0x{0:x}", memory_address);
        /* Request out of range -Invalid memory address */
        return false;
    }

    /* Validate the full memory size */
    if (memory_size < 1)
    {
        LOG_ERROR(RDSlogger.GET_LOGGER(), "Error: Invalid memory size: 0x{0:x}", memory_size);
        /* Request out of range -Invalid memory size*/
        return false;
    }

    LOG_INFO(RDSlogger.GET_LOGGER(), "Validated Memory Address: 0x{0:x}", memory_address);
    LOG_INFO(RDSlogger.GET_LOGGER(), "Validated Memory Size: 0x{0:x}", memory_size);

    return true;
}

bool RequestDownloadService::isLatestSoftwareVersion()
{
    /* Logic to check version */
    return true;
    ReadDataByIdentifier software_version(this->socket, RDSlogger);
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

void RequestDownloadService::downloadSoftwareVersion(uint8_t ecu_id, uint8_t sw_version)
{
    namespace py = pybind11;
    py::scoped_interpreter guard{}; // start the interpreter and keep it alive
    /* PROJECT_PATH defined in makefile to be the root folder path (POC)*/
    std::string project_path = PROJECT_PATH;
    std::string path_to_drive_api = project_path + "/src/ota/google_drive_api";
    try
    {

        auto sys = py::module::import("sys");
        sys.attr("path").attr("append")(path_to_drive_api);

        /* Get the created Python module */
        py::module python_module = py::module::import("GoogleDriveApi");
        /* From the module, get the needed functionality (gDrive object) */
        py::object gGdrive_object = python_module.attr("gDrive");

        /* Call the downloadFile method from GoogleDriveApi.py */
        gGdrive_object.attr("downloadFile")(ecu_id, sw_version);
    }
    catch(const py::error_already_set& e)
    {
        LOG_ERROR(RDSlogger.GET_LOGGER(), "Python error: {}", e.what());
        MCU::mcu->setDidValue(OTA_UPDATE_STATUS_DID, {ERROR});
    }
}

RDSData RequestDownloadService::getRdsData()
{
    return RequestDownloadService::rds_data;
}