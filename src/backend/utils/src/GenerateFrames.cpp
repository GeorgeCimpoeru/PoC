#include "../include/GenerateFrames.h"

GenerateFrames::GenerateFrames(Logger& logger)
    : logger(logger)
{}
GenerateFrames::GenerateFrames(int socket, Logger& logger)
    : logger(logger), socket(socket)
{
    this->addSocket(socket);
}

int GenerateFrames::getSocket()
{
    return this->socket;
}

struct can_frame GenerateFrames::createFrame(int &id,  std::vector<uint8_t> &data, FrameType frameType)
{
    struct can_frame frame;
    switch (frameType)
    {
        case ERROR_FRAME:
        /* Handle ERROR_FRAME */
        break;
        case OVERLOAD_FRAME:
        /* Handle OVERLOAD_FRAME */
        break;
        case DATA_FRAME:
            frame.can_id = (id & CAN_EFF_MASK) | CAN_EFF_FLAG;
            frame.can_dlc = data.size();
            for (uint8_t byte = 0; byte < frame.can_dlc; byte++)
            {
                frame.data[byte] = data[byte];
            }
            break;
        case REMOTE_FRAME:
            frame.can_id = (id & CAN_EFF_MASK) | CAN_EFF_FLAG;
            frame.can_id |= CAN_RTR_FLAG;
            frame.can_dlc = data.size();
            for (uint8_t byte = 0; byte < frame.can_dlc; byte++)
            {
                frame.data[byte] = data[byte];
            }
            break;
    }
    return frame;
}

bool GenerateFrames::sendFrame(int id, std::vector<uint8_t > data, FrameType frameType)
{
    struct can_frame frame = createFrame(id, data, frameType);
    int nbytes = write(this->socket, &frame, sizeof(frame));
    if (nbytes != sizeof(frame))
    {
        /* std::cout<<"Write error\n"; */
        LOG_WARN(logger.GET_LOGGER(), "Write error\n");
        return -1;
    }
    return 0;
}

int GenerateFrames::sendFrame(int can_id, std::vector<uint8_t> data, int s, FrameType frameType) 
{
    if (s < 0)
    {
        throw std::runtime_error("Socket not initialized");
        LOG_ERROR(logger.GET_LOGGER(), "Socket not initialized");
    }

    /* Create the CAN frame */
    struct can_frame frame = createFrame(can_id, data, frameType);

    /* Send the CAN frame */
    if (write(s, &frame, sizeof(frame)) != sizeof(frame)) 
    {
        perror("Write");
        return -1;
    }
    return 0;
}
void GenerateFrames::addSocket(int socket)
{
    if (socket >= 0)
    {
        this->socket = socket;
        return;
    }
    /* std::cout<<"Error: Pass a valid Socket\n"; */
    LOG_WARN(logger.GET_LOGGER(), "Error: Pass a valid Socket\n");
    exit(EXIT_FAILURE);
}

void GenerateFrames::sessionControl(int id, uint8_t sub_function, bool response)
{
    std::vector<uint8_t> data(3);
    if (!response)
    {
        data = {0x2,0x10,sub_function};
        this->sendFrame(id,data);
        return;
    }
    data = {0x2,0x50,sub_function};
    this->sendFrame(id, data);
    return;
}

void GenerateFrames::ecuReset(int id, uint8_t sub_function, bool response)
{
    std::vector<uint8_t> data(3);
    if (!response)
    {
        data = {0x2,0x11,sub_function};
        this->sendFrame(id, data);
        return;
    }
    data = {0x2,0x51,sub_function};
    this->sendFrame(id, data);
    return;
}

void GenerateFrames::ecuReset(int id, uint8_t sub_function, int socket, bool response)
{
    std::vector<uint8_t> data(3);
    if (!response)
    {
        data = {0x2,0x11,sub_function};
        this->sendFrame(id, data, socket, FrameType::DATA_FRAME);
        return;
    }
    data = {0x2,0x51,sub_function};
    this->sendFrame(id, data, socket, FrameType::DATA_FRAME);
    return;
}

void GenerateFrames::securityAccessRequestSeed(int id, const std::vector<uint8_t> &seed)
{
    if (seed.size() == 0)
    {
        std::vector<uint8_t> data = {0x02, 0x27, 0x1};
        this->sendFrame(id, data);
        return;
    }
    std::vector<uint8_t> data = {(uint8_t)(seed.size() + 2), 0x67, 0x1};
    for (uint8_t data_seed: seed)
    {
        data.push_back(data_seed);
    }
    this->sendFrame(id, data);
    return;
}

void GenerateFrames::securityAccessSendKey(int id, const std::vector<uint8_t> &key)
{
    if (key.size() > 0 )
    {
        std::vector<uint8_t> data = {(uint8_t)(key.size() + 2), 0x27, 0x2};
        for (uint8_t data_key: key)
        {
            data.push_back(data_key);
        }
        this->sendFrame(id, data);
        return;
    }
    std::vector<uint8_t> data = {0x02,0x67,0x02};
    this->sendFrame(id, data);
    return;
}

void GenerateFrames::routineControl(int id, uint8_t sub_function, uint16_t routine_identifier, std::vector<uint8_t>& routine_result, bool response)
{
    uint8_t default_pci = 0x04;
    if (!response)
    {
        std::vector<uint8_t> data = {default_pci, 0x31, sub_function, (uint8_t)(routine_identifier / 0x100), (uint8_t)(routine_identifier % 0x100)};
        this->sendFrame(id, data);
        return;
    }
    std::vector<uint8_t> data = {static_cast<uint8_t>(default_pci + routine_result.size()), 0x71, sub_function, (uint8_t)(routine_identifier / 0x100), (uint8_t)(routine_identifier % 0x100)};
    data.insert(data.end(), routine_result.begin(), routine_result.end());
    this->sendFrame(id, data);
    return;
}

void GenerateFrames::testerPresent(int id, bool response)
{
    if (!response)
    {
        std::vector<uint8_t> data = {0x02,0x3E,0x00};
        this->sendFrame(id, data);
        return;
    }
    std::vector<uint8_t> data = {0x02,0x7E,0x00};
    this->sendFrame(id, data);
    return;
}

void GenerateFrames::readDataByIdentifier(int id,int identifier, std::vector<uint8_t> response )
{
    if (response.size() == 0)
    {
        std::vector<uint8_t> data = {0x03, 0x22, (uint8_t)(identifier/0x100), (uint8_t)(identifier%0x100)};
        this->sendFrame(id, data);
        return;
    }
    int length_response = response.size();
    if (length_response <= 4)
    {
        std::vector<uint8_t> data = {(uint8_t)(length_response + 3), 0x62, (uint8_t)(identifier/0x100), (uint8_t)(identifier%0x100)};
        for (uint8_t data_response: response)
        {
            data.push_back(data_response);
        }
        this->sendFrame(id, data);
        return;
    }
    /* std::cout<<"ERROR: The frame is to long!, consider using method ReadDataByIdentifierLongResponse\n"; */
    LOG_WARN(logger.GET_LOGGER(), "ERROR: The frame is to long!, consider using method ReadDataByIdentifierLongResponse\n");
    return;
    /*
    *According to documentation, the frame can
    *request more than one DID. Future implementation
    *  |
    *  V
    */
}

void GenerateFrames::readDataByIdentifierLongResponse(int id,uint16_t identifier, std::vector<uint8_t> response, bool first_frame)
{
    int length_response = response.size();
    std::vector<uint8_t> data = {(uint8_t)(length_response + 3), 0x62, (uint8_t)(identifier/0x100), (uint8_t)(identifier%0x100)};
    for (uint8_t data_response: response)
    {
        data.push_back(data_response);
    }
    
    if (data.size() > 8)
    {
        GenerateConsecutiveFrames(id,data,first_frame);
    }
     else
    {
        LOG_WARN(logger.GET_LOGGER(), "The data_parameter is to long. Consider using writeDataByIdentifier method\n");
    }
}

void GenerateFrames::flowControlFrame(int id)
{
    this->sendFrame(id, {0x30,0x00,0x00,0x00});
}

void GenerateFrames::readMemoryByAddress(int id, int memory_address, int memory_size, std::vector<uint8_t> response )
{
    /* add lengths of of memory size/address to the frame */
    uint8_t length_memory_size = (countDigits(memory_size) +1) / 2;
    uint8_t length_memory_address = (countDigits(memory_address) + 1) / 2;
    uint8_t length_memory = length_memory_size * 0x10 + length_memory_address;
    if (response.size() == 0)
    {
        uint8_t pci_l = length_memory_size + length_memory_address + 2;
        std::vector<uint8_t> data = {pci_l, 0x23, length_memory};
        /* add memory address and size to the frame */
        insertBytes(data, memory_address, length_memory_address);
        insertBytes(data, memory_size, length_memory_size);
        this->sendFrame(id, data);
        return;
    }
    uint8_t pci_l = length_memory_size + length_memory_address + 2 + response.size();
    std::vector<uint8_t> data = {pci_l, 0x63, length_memory};
    /* add memory address and size to the frame */
    insertBytes(data, memory_address, length_memory_address);
    insertBytes(data, memory_size, length_memory_size);
    if (data.size() + response.size() < 9)
    {
        for (uint8_t data_response: response)
        {
            data.push_back(data_response);
        }
        this->sendFrame(id, data);
        return;
    } else
    {
        /* std::cout<<"ERROR: Response to long, consider using ReadMemoryByAdressLongResponse method\n"; */
        LOG_WARN(logger.GET_LOGGER(), "ERROR: Response to long, consider using ReadMemoryByAdressLongResponse method\n");
        return;
    }
}

void GenerateFrames::readMemoryByAddressLongResponse(int id, int memory_address, int memory_size, std::vector<uint8_t> response, bool first_frame)
{
    uint8_t length_memory_size = (countDigits(memory_size) +1) / 2;
    uint8_t length_memory_address = (countDigits(memory_address) + 1) / 2;
    uint8_t length_memory = length_memory_size * 0x10 + length_memory_address;
    uint8_t pci_l = length_memory_size + length_memory_address + 2 + response.size();
    std::vector<uint8_t> data = {pci_l, 0x63, length_memory};
    /* add memory address and size to the frame */
    insertBytes(data, memory_address, length_memory_address);
    insertBytes(data, memory_size, length_memory_size);
    for (uint8_t data_response: response)
    {
        data.push_back(data_response);
    }

    if (data.size() > 8)
    {
        GenerateConsecutiveFrames(id,data,first_frame);
    }
     else
    {
        LOG_WARN(logger.GET_LOGGER(), "The data_parameter is to long. Consider using writeDataByIdentifier method\n");
    }
}

void GenerateFrames::writeDataByIdentifier(int id, uint16_t identifier, std::vector<uint8_t> data_parameter )
{
    if (data_parameter.size() > 0)
    {
        if (data_parameter.size() <= 4)
        {
            std::vector<uint8_t> data = {(uint8_t)(data_parameter.size() + 3),0x2E, (uint8_t)(identifier/0x100),(uint8_t)(identifier%0x100)};
            for (uint8_t data_byte: data_parameter)
            {
                data.push_back(data_byte);
            }
            this->sendFrame(id, data);
            return;
        }
        else
        {
            /* std::cout<<"The data_parameter is to long. Consider using WriteDataByIdentifierLongData method\n"; */
            LOG_WARN(logger.GET_LOGGER(), "The data_parameter is to long. Consider using WriteDataByIdentifierLongData method\n");
            return;
        }
    }
    std::vector<uint8_t> data = {0x03, 0x6E, (uint8_t)(identifier/0x100),(uint8_t)(identifier%0x100)};
    this->sendFrame(id, data);
    return;
}

void GenerateFrames::writeDataByIdentifierLongData(int id, uint16_t identifier, std::vector<uint8_t> data_parameter, bool first_frame)
{
    std::vector<uint8_t> data = {(uint8_t)(data_parameter.size() + 3),0x2E, (uint8_t)(identifier/0x100),(uint8_t)(identifier%0x100)};
    for (uint8_t data_byte: data_parameter)
    {
        data.push_back(data_byte);
    }
    if (data.size() > 8)
    {
        GenerateConsecutiveFrames(id,data,first_frame);
    }
     else
    {
        LOG_WARN(logger.GET_LOGGER(), "The data_parameter is to long. Consider using writeDataByIdentifier method\n");
    }
}

void GenerateFrames::readDtcInformation(int id, uint8_t sub_function, uint8_t dtc_status_mask)
{
    std::vector<uint8_t> data = {0x03, 0x19, sub_function, dtc_status_mask};
    this->sendFrame(id, data);
    return;
}

void GenerateFrames::readDtcInformationResponse01(int id, uint8_t status_availability_mask, uint8_t dtc_format_identifier, uint16_t dtc_count)
{
    std::vector<uint8_t> data = {0x06, 0x59, 0x01, status_availability_mask, dtc_format_identifier, uint8_t(dtc_count / 0x100), uint8_t(dtc_count % 0x100)};
    this->sendFrame(id, data);
    return;
}

void GenerateFrames::readDtcInformationResponse02(int id, uint8_t status_availability_mask, std::vector<std::pair<int,int>> dtc_and_status_list)
{
    std::vector<uint8_t> data = {0x03, 0x59, 0x02, status_availability_mask};
    for (std::pair<int, int> dtc_and_status : dtc_and_status_list)
    {
        uint8_t length_dtc_and_status = (countDigits(dtc_and_status.first) +1) / 2;
        insertBytes(data,dtc_and_status.first,length_dtc_and_status);
        data.push_back(dtc_and_status.second);
        data[0] = 3 + length_dtc_and_status ;
    }
    if (data.size() <= 8)
    {
        this->sendFrame(id, data);
    }
    else
    {
        //std::cout<<"The data_parameter is to long. Consider using readDtcInformationResponse02Long method\n";
        LOG_WARN(logger.GET_LOGGER(), "The data_parameter is to long. Consider using readDtcInformationResponse02Long method\n");
    }
    return;
}

void GenerateFrames::readDtcInformationResponse02Long(int id, uint8_t status_availability_mask, std::vector<std::pair<int,int>> dtc_and_status_list, bool first_frame)
{
    std::vector<uint8_t> data = {0x00, 0x59, 0x02, status_availability_mask};
    uint8_t pci_l=3;
    for (std::pair<int, int> dtc_and_status : dtc_and_status_list)
    {
        uint8_t length_dtc_and_status = (countDigits(dtc_and_status.first) +1) / 2;
        if(length_dtc_and_status > 2)
        {
            length_dtc_and_status = 2;
        }
        insertBytes(data,dtc_and_status.first,length_dtc_and_status);
        data.push_back(dtc_and_status.second);
        pci_l += length_dtc_and_status + 1;
    }
    data[0] = pci_l ;
    if (data.size() > 8)
    {
        GenerateConsecutiveFrames(id,data,first_frame);
    }
     else
    {
        //std::cout<<"The data_parameter is to short. Consider using readDtcInformationResponse02 method\n";
        LOG_WARN(logger.GET_LOGGER(), "The data_parameter is to long. Consider using readDtcInformationResponse02 method\n");
    }
}

void GenerateFrames::GenerateConsecutiveFrames(int id, std::vector<uint8_t> data, bool first_frame)
{
    
    if (first_frame)
    {
        std::vector<uint8_t> data_in_frame = {0x10};
        data_in_frame.insert(data_in_frame.end(), data.begin(), data.begin() + 7);
        this->sendFrame(id, data_in_frame);
    }
    else
    {
        data.erase(data.begin(), data.begin() + 7);
        uint8_t index = 0;
        while (data.size())
        {
            uint8_t head = 0x20 + (++index)  % 0xFF;
            std::vector<uint8_t> data_in_frame = { head };
            if (data.size() >=8)
            {
                data_in_frame.insert(data_in_frame.end(), data.begin(), data.begin() + 7);
                data.erase(data.begin(), data.begin() + 7);
            }
            else
            {
                data_in_frame.insert(data_in_frame.end(), data.begin(), data.begin() + data.size());
                data.erase(data.begin(), data.begin() + data.size());
            }
            this->sendFrame(id, data_in_frame);
        }
    }
}

void GenerateFrames::clearDiagnosticInformation(int id, std::vector<uint8_t> group_of_dtc, bool response)
{
    std::vector<uint8_t> data;
    /* Request */
    if (!response)
    { 
        if (group_of_dtc.size() < 7)
        {
            uint8_t number_of_dtc = group_of_dtc.size();
            data = {(uint8_t)(number_of_dtc + 1), 0x14};
            for (uint8_t dtc: group_of_dtc)
            {
                data.push_back(dtc);
            }
            this->sendFrame(id, data);
            return;
        } else
        {
            /* std::cout<<"ERROR: Can't send more than 6 DTC/frame, please consider send 2 or more frames\n"; */
            LOG_WARN(logger.GET_LOGGER(), "ERROR: Can't send more than 6 DTC/frame, please consider send 2 or more frames\n");
            return;
        }
        
    }
    /* Response */
    data = {0x01, 0x54};
    this->sendFrame(id, data);
    return;
}

void GenerateFrames::accessTimingParameters(int id, uint8_t sub_function, std::vector<uint8_t> data_parameter, bool response)
{
    if (!response)
    {
        std::vector<uint8_t> data = {0x02, 0x83, sub_function};
        this->sendFrame(id, data);
        return;
    }
    else if(!data_parameter.size())
    {
        std::vector<uint8_t> data = {0x02, 0xc3, sub_function};
        this->sendFrame(id, data);
        return;
    }
    else
    {
        this->sendFrame(id, data_parameter);
        return;
    }
}

void GenerateFrames::negativeResponse(int id, uint8_t sid, uint8_t nrc)
{
    std::vector<uint8_t> data = {0x03, 0x7F, sid, nrc};
    this->sendFrame(id, data);
    return;
}

void GenerateFrames::requestDownload(int id, uint8_t data_format_identifier, int memory_address, int memory_size, uint8_t download_type)
{
    /* Request Frame add lengths of of memory size/address to the frame */
    uint8_t length_memory_size = (countDigits(memory_size ) + 1) / 2;
    uint8_t length_memory_address = (countDigits(memory_address) + 1 )/ 2;
    uint8_t length_memory = length_memory_size * 0x10 + length_memory_address;
    uint8_t pci_length = length_memory_size + length_memory_address + 3;
    std::vector<uint8_t> data = {pci_length, 0x34, data_format_identifier, length_memory};
    /* add memory address and size to the frame */
    insertBytes(data, memory_address, length_memory_address);
    insertBytes(data, memory_size, length_memory_size);
    data.push_back(download_type);
    this->sendFrame(id, data);
    return;
}

void GenerateFrames::requestDownloadLong(int id, uint8_t data_format_identifier, int memory_address, int memory_size, uint8_t download_type, bool first_frame) 
{
    /* Request Frame add lengths of of memory size/address to the frame */
    uint8_t length_memory_size = (countDigits(memory_size ) + 1) / 2;
    uint8_t length_memory_address = (countDigits(memory_address) + 1 )/ 2;
    uint8_t length_memory = length_memory_size * 0x10 + length_memory_address;
    uint8_t pci_length = length_memory_size + length_memory_address + 3 + 2;
    std::vector<uint8_t> data = {pci_length, 0x34, data_format_identifier, length_memory};
    /* add memory address and size to the frame */
    insertBytes(data, memory_address, length_memory_address);
    insertBytes(data, memory_size, length_memory_size);
    data.push_back(download_type);
    /* Send only 3 first bytes of data */
    this->GenerateConsecutiveFrames(id, data, first_frame);
    return;
    
}

void GenerateFrames::requestDownloadResponse(int id, int max_number_block)
{
    /* Response frame */
    std::vector<uint8_t> max_number_block_bytes;
    bool first_byte_found = false;
    for(char i = 3, byte; i >=0; --i)
    {
        byte = (max_number_block >> (i * 8)) & 0xFF;
        if(byte != 0 || first_byte_found == true)
        {
            max_number_block_bytes.push_back(byte);
            first_byte_found = true;
        }
    }
    uint8_t length_max_number_block = max_number_block_bytes.size();
    std::vector<uint8_t> data = {(uint8_t)(length_max_number_block + 2), 0x74, static_cast<uint8_t>(length_max_number_block * 0x10)};
    data.insert(data.end(), max_number_block_bytes.begin(), max_number_block_bytes.end());
    LOG_INFO(logger.GET_LOGGER(), "max number block length 0x{:x}", length_max_number_block);
    LOG_INFO(logger.GET_LOGGER(), "max number block 0x{:x}", max_number_block);

    this->sendFrame(id, data);
    return;
}

void GenerateFrames::transferData(int id, uint8_t block_sequence_counter, std::vector<uint8_t> transfer_request)
{
    /* If is not a response */
    if (transfer_request.size() != 0)
    {
        if (transfer_request.size() <= 5)
        {
             std::vector<uint8_t> data = {(uint8_t)(transfer_request.size() + 2), 0x36, block_sequence_counter};
            for (uint8_t data_transfer: transfer_request)
            {
                data.push_back(data_transfer);
            }
            this->sendFrame(id, data);
            return;
        } else
        {
            /* std::cout<<"The transfer_request is to long. Consider using transferDataLong method\n"; */
            LOG_WARN(logger.GET_LOGGER(), "The transfer_request is to long. Consider using transferDataLong method\n");
            return;
        }
    }
    /* Response frame */
    std::vector<uint8_t> data = {0x02,0x76,block_sequence_counter};
    this->sendFrame(id, data);
    return;
}

void GenerateFrames::transferDataLong(int id, uint8_t block_sequence_counter, std::vector<uint8_t> transfer_request, bool first_frame)
{
    std::vector<uint8_t> data = {(uint8_t)(transfer_request.size() + 2), 0x36, block_sequence_counter};
    for (uint8_t data_transfer: transfer_request)
    {
        data.push_back(data_transfer);
    }
    if (data.size() > 8)
    {
        GenerateConsecutiveFrames(id,data,first_frame);
    }
     else
    {
        LOG_WARN(logger.GET_LOGGER(), "The data_parameter is to long. Consider using transferData method\n");
    }
}

void GenerateFrames::requestTransferExit(int id, bool response)
{
    if (!response)
    {
        std::vector<uint8_t> data = {0x01, 0x37};
        this->sendFrame(id, data);
        return;
    }
    std::vector<uint8_t> data = {0x01,0x77};
    this->sendFrame(id, data);
    return;
}

bool GenerateFrames::requestUpdateStatusResponse(int id, std::vector<uint8_t> response)
{
    this->sendFrame(id, response);
    if(response[1] == 0x7F)
    {
        return false;
    }
    return true;
}

/* Private */
int GenerateFrames::countDigits(int number)
{
    int digits = 0;
    if (number < 0) 
        return 0;
    while (number) {
        number /= 10;
        digits++;
    }
    return digits;
}

void GenerateFrames::insertBytes(std::vector<uint8_t>& byteVector, unsigned int num, int numBytes) {
    for (int i = numBytes - 1; i >= 0; --i) {
        byteVector.push_back((num >> (i * 8)) & 0xFF);
    }
}
