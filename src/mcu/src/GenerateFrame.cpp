/**
 * @file GenerateFrame.cpp
 * @brief Implementation file for the GenerateFrame class.
 * 
 * This file contains the implementation of the GenerateFrame class, which is responsible for creating and sending CAN frames.
 */
#include "../include/GenerateFrame.h"

/* Constructor which create a CAN frame */
GenerateFrame::GenerateFrame(int socket) 
{
    if (socket < 0) 
    {
        throw std::invalid_argument("Invalid socket");
    }
    this->socket = socket;
}

/* Default constructor */
GenerateFrame::GenerateFrame() 
{
    
}

/* Function to get the socket */
int GenerateFrame::getSocket() 
{
    return this->socket;
}

/* Function to create a CAN frame */
can_frame GenerateFrame::CreateFrame(uint32_t can_id, std::vector<uint8_t> data, FrameType frameType) 
{
    struct can_frame frame;
    frame.can_id = can_id;

    switch (frameType) 
    {
        case DATA_FRAME:
            frame.can_id &= CAN_EFF_MASK;
            frame.can_dlc = data.size();
            for (uint8_t bit = 0; bit < data.size(); bit++) 
            {
                frame.data[bit] = data[bit];
            }
            break;
        case REMOTE_FRAME:
            frame.can_id &= CAN_EFF_MASK;
            frame.can_id |= CAN_RTR_FLAG;
            frame.can_dlc = data.size();
            for (uint8_t bit = 0; bit < data.size(); bit++) 
            {
                frame.data[bit] = data[bit];
            }
            break;
        case ERROR_FRAME:
            frame.can_id = CAN_ERR_FLAG;
            frame.can_dlc = 0;
            break;
        case OVERLOAD_FRAME:
            frame.can_id = CAN_ERR_FLAG;
            frame.can_dlc = 0;
            break;
        default:
            throw std::invalid_argument("Invalid frame type");
    }
    return frame;
}

/* Function to send a CAN frame */
int GenerateFrame::SendFrame(uint32_t can_id, std::vector<uint8_t> data, FrameType frameType) 
{
    if (this->socket < 0)
    {
        throw std::runtime_error("Socket not initialized");
    }

    /* Create the CAN frame */
    this->frame = CreateFrame(can_id, data, frameType);

    /* Send the CAN frame */
    if (write(this->socket, &frame, sizeof(frame)) != sizeof(frame)) 
    {
        perror("Write");
        return -1;
    }

    /* Close the socket */
    // close(socket);
    return 0;
}

/* Function to get the CAN frame */
can_frame GenerateFrame::getFrame() 
{
    return this->frame;
}

void GenerateFrame::SessionControl(uint32_t can_id, uint8_t subfunction, bool response) 
{
    if (response) 
    {
        this->SendFrame(can_id, {0x2, 0x50, subfunction});
    }
    else
    {   
        this->SendFrame(can_id, {0x2, 0x10, subfunction});
    }
}

void GenerateFrame::EcuReset(uint32_t can_id, bool response) 
{
    if (response) 
    {
        this->SendFrame(can_id, {0x2, 0x51, 0x3});
    }
    else 
    {
        this->SendFrame(can_id, {0x2, 0x11, 0x3});
    }
}

void GenerateFrame::ReadDataByIdentifier(uint32_t can_id, uint8_t dataIdentifier, std::vector<uint8_t> response) 
{
    if (response.empty())
    {
        std::vector<uint8_t> data = {0x3, 0x22, static_cast<uint8_t>(dataIdentifier/0x100), static_cast<uint8_t>(dataIdentifier%0x100)};
        this->SendFrame(can_id, data);
    }
    else if (response.size() <= 5)
    {
        std::vector<uint8_t> data = {static_cast<uint8_t>(response.size() + 3), 0x62, static_cast<uint8_t>(dataIdentifier/0x100), static_cast<uint8_t>(dataIdentifier%0x100)};
        data.insert(data.end(), response.begin(), response.end());
        this->SendFrame(can_id, data);
    }
    else
    {
        std::cout << "Response size is too large" << std::endl;
    }
}

void GenerateFrame::CreateFrameLong(uint32_t can_id, uint8_t sid, uint8_t dataIdentifier, std::vector<uint8_t> response, bool firstFrame) 
{
    if (firstFrame)
    {
        std::vector<uint8_t> data = {0x10, static_cast<uint8_t>(response.size() + 3), sid, static_cast<uint8_t>(dataIdentifier/0x100), static_cast<uint8_t>(dataIdentifier%0x100)};
        for (uint8_t bit = 0; bit < 3; bit++) 
        {
            data.push_back(response[bit]);
        }
        this->SendFrame(can_id, data);
    }
    else
    {
        response.erase(response.begin(), response.begin() + 3);
        std::vector<uint8_t> data;
        for (uint8_t r_bit = 0; r_bit < response.size() / 7; r_bit++)
        {
            data = {static_cast<uint8_t>(0x21 + (r_bit % 0xF))};
            for (uint8_t d_bit = 0; d_bit < 7 && ((r_bit * 7) + d_bit) < response.size(); d_bit++)
            {
                data.push_back(response[r_bit * 7 + d_bit]);
            }
            this->SendFrame(can_id, data);
        }
    }
}

void GenerateFrame::ReadDataByIdentifierLong(uint32_t can_id, uint8_t dataIdentifier, std::vector<uint8_t> response, bool firstFrame) 
{
    CreateFrameLong(can_id, 0x62, dataIdentifier, response, firstFrame);
}

void GenerateFrame::FlowControlFrame(uint32_t can_id) 
{
    this->SendFrame(can_id, {0x30, 0x00, 0x00, 0x00});
}

void GenerateFrame::AuthenticationRequestSeed(uint32_t can_id, const std::vector<uint8_t>& seed) 
{
    if (seed.empty()) 
    {
        this->SendFrame(can_id, {0x3, 0x29, 0x1});
    }
    else
    {
        std::vector<uint8_t> data = {static_cast<uint8_t>(seed.size() + 2), 0x69, 0x1};
        for (uint8_t bit = 0; bit < seed.size(); bit++) 
        {
            data.push_back(seed[bit]);
        }
        this->SendFrame(can_id, data);
    }
}

void GenerateFrame::AuthenticationSendKey(uint32_t can_id, const std::vector<uint8_t>& key) 
{
    if (key.empty()) 
    {
        this->SendFrame(can_id, {0x02, 0x69, 0x02});
    }
    else 
    {
        std::vector<uint8_t> data = {static_cast<uint8_t>(key.size() + 2), 0x29, 0x2};
        for (uint8_t bit = 0; bit < key.size(); bit++) 
        {
            data.push_back(key[bit]);
        }
        this->SendFrame(can_id, data);
    }
}

void GenerateFrame::RoutineControl(uint32_t can_id, uint8_t subfunction, uint8_t routine_identifier, bool response) 
{
    if (response) 
    {
        this->SendFrame(can_id, {0x4, 0x71, subfunction, static_cast<uint8_t>(routine_identifier / 0x100), static_cast<uint8_t>(routine_identifier % 0x100)});
    }
    else 
    {
        this->SendFrame(can_id, {0x4, 0x31, subfunction, static_cast<uint8_t>(routine_identifier / 0x100), static_cast<uint8_t>(routine_identifier % 0x100)});
    }
}

void GenerateFrame::TesterPresent(uint32_t can_id, bool response) 
{
    if (response) 
    {
        this->SendFrame(can_id, {0x02, 0x7E, 0x00});
    }
    else 
    {
        this->SendFrame(can_id, {0x2, 0x3E, 0x00});
    }
}

void GenerateFrame::ReadMemoryByAddress(uint32_t can_id, uint8_t memory_size, uint8_t memory_address, std::vector<uint8_t> response) 
{
    uint8_t len_mem_size = CountDigits(memory_size + 1) / 2;
    uint8_t len_mem_addr = CountDigits(memory_address + 1) / 2;
    uint8_t len_mem = len_mem_size * 0x10 + len_mem_addr;
    if (response.empty())
    {
        uint8_t pci_len = len_mem_size + len_mem_addr + 2;
        std::vector<uint8_t> data = {pci_len, 0x23, len_mem};
        InsertBytes(data, memory_address, len_mem_addr);
        InsertBytes(data, memory_size, len_mem_size);
        this->SendFrame(can_id, data);
    }
    else
    {
        uint8_t pci_len = len_mem_size + len_mem_addr + 2 + response.size();
        std::vector<uint8_t> data = {pci_len, 0x63, len_mem};
        InsertBytes(data, memory_address, len_mem_addr);
        InsertBytes(data, memory_size, len_mem_size);
        if (data.size() + response.size() < 9)
        {
            for (uint8_t bit = 0; bit < response.size(); bit++) 
            {
                data.push_back(response[bit]);
            }
            this->SendFrame(can_id, data);
        }
        else
        {
            std::cout << "Response size is too large" << std::endl;
        }
    }
}

void GenerateFrame::ReadMemoryByAddressLong(uint32_t can_id, uint8_t memory_size, uint8_t memory_address, std::vector<uint8_t> response, bool first_frame) 
{
    uint8_t len_mem_size = CountDigits(memory_size + 1) / 2;
    uint8_t len_mem_addr = CountDigits(memory_address + 1) / 2;
    uint8_t len_mem = len_mem_size * 0x10 + len_mem_addr;
    if (first_frame)
    {
        uint8_t pci_len = response.size() + len_mem_size + len_mem_addr + 2;
        std::vector<uint8_t> data = {0x10, pci_len, 0x63, len_mem};
        InsertBytes(data, memory_address, len_mem_addr);
        InsertBytes(data, memory_size, len_mem_size);
        for (uint8_t bit = 0; bit < 3; bit++) 
        {
            data.push_back(response[bit]);
        }
        this->SendFrame(can_id, data);
    }
    else
    {
        uint8_t mem_sent = 8 - len_mem_size - len_mem_addr - 4;
        response.erase(response.begin(), response.begin() + mem_sent);
        std::vector<uint8_t> data;
        for (uint8_t bit = 0; bit < response.size() / 7; bit++)
        {
            data = {static_cast<uint8_t>(0x21 + (bit % 0xF))};
            for (uint8_t d_bit = 0; d_bit < 7 && ((bit * 7) + d_bit) < response.size(); d_bit++) 
            {
                data.push_back(response[bit * 7 + d_bit]);
            }
            this->SendFrame(can_id, data);
        }
    }
}

void GenerateFrame::WriteDataByIdentifier(uint32_t can_id, uint8_t identifier, std::vector<uint8_t> data_parameter) 
{
    if (data_parameter.empty()) 
    {
        this->SendFrame(can_id, {0x03, 0x6E, static_cast<uint8_t>(identifier / 0x100), static_cast<uint8_t>(identifier % 0x100)});
    }
    else
    {
        if (data_parameter.size() <= 4)
        {
            std::vector<uint8_t> data = {static_cast<uint8_t>(data_parameter.size() + 3), 0x2E, static_cast<uint8_t>(identifier / 0x100), static_cast<uint8_t>(identifier % 0x100)};
            data.insert(data.end(), data_parameter.begin(), data_parameter.end());
            this->SendFrame(can_id, data);
        }
        else
        {
            std::cout << "Data parameter size is too large" << std::endl;
        }
    }
}

void GenerateFrame::WriteDataByIdentifierLong(uint32_t can_id, uint8_t identifier, std::vector<uint8_t> data_parameter, bool first_frame) 
{
    CreateFrameLong(can_id,0x2E,identifier,data_parameter,first_frame);
}

void GenerateFrame::ReadDtcInformation(uint32_t can_id, uint8_t subfunction, uint8_t dtc_status_mask) 
{
    this->SendFrame(can_id, {0x03, 0x19, subfunction, dtc_status_mask});
}

void GenerateFrame::ReadDtcInformationResponse01(uint32_t can_id, uint8_t status_availability_mask, uint8_t dtc_format_identifier, uint8_t dtc_count) 
{
    this->SendFrame(can_id, {0x03, 0x59, 0x01, status_availability_mask, dtc_format_identifier, dtc_count});
}

void GenerateFrame::ClearDiagnosticInformation(uint32_t can_id, std::vector<uint8_t> group_of_dtc, bool response) 
{
    std::vector<uint8_t> data;
    if (response) 
    {
        this->SendFrame(can_id, {0x01, 0x54});
    }
    else 
    {
        if (group_of_dtc.size() < 8)
        {
            data = {static_cast<uint8_t>(group_of_dtc.size() + 1), 0x14};
            for (uint8_t bit = 0; bit < group_of_dtc.size(); bit++) 
            {
                data.push_back(group_of_dtc[bit]);
            }
            this->SendFrame(can_id, data);
        }
        else std::cout << "Group of DTC size is too large" << std::endl;
    }
}

void GenerateFrame::AccessTimingParameters(uint32_t can_id, uint8_t subfunction, bool response) 
{
    if (response) 
    {
        this->SendFrame(can_id, {0x02, 0xC3, subfunction});
    }
    else 
    {
        this->SendFrame(can_id, {0x02, 0x83, subfunction});
    }
}

void GenerateFrame::NegativeResponse(uint32_t can_id, uint8_t sid, uint8_t nrc) 
{
    this->SendFrame(can_id, {0x03, 0x7F, sid, nrc});
}

void GenerateFrame::RequestDownload(uint32_t can_id, uint8_t data_format_identifier, uint8_t memory_address, uint8_t memory_size) 
{
    uint8_t len_mem_size = CountDigits(memory_size + 1) / 2;
    uint8_t len_mem_addr = CountDigits(memory_address + 1) / 2;
    uint8_t len_mem = len_mem_size * 0x10 + len_mem_addr;
    uint8_t pci_len = len_mem_size + len_mem_addr + 3;
    std::vector<uint8_t> data = {pci_len, 0x34, data_format_identifier, len_mem};
    InsertBytes(data, memory_address, len_mem_addr);
    InsertBytes(data, memory_size, len_mem_size);
    this->SendFrame(can_id, data);
}

void GenerateFrame::RequestDownloadResponse(uint32_t can_id, uint8_t max_number_block) 
{
    uint8_t len_max_num_block = CountDigits(max_number_block + 1) / 2;
    std::vector<uint8_t> data = {static_cast<uint8_t>(len_max_num_block + 2), 0x74, static_cast<uint8_t>((len_max_num_block * 0x10))};
    InsertBytes(data, max_number_block, len_max_num_block);
    this->SendFrame(can_id, data);
}

void GenerateFrame::TransferData(uint32_t can_id, uint8_t block_sequence_counter, std::vector<uint8_t> transfer_request) 
{
    if (transfer_request.empty()) 
    {
        this->SendFrame(can_id, {0x02, 0x76, block_sequence_counter});
    }
    else
    {
        std::vector<uint8_t> data = {static_cast<uint8_t>(transfer_request.size() + 2), 0x36, block_sequence_counter};
        for (uint8_t bit = 0; bit < transfer_request.size(); bit++) 
        {
            data.push_back(transfer_request[bit]);
        }
        this->SendFrame(can_id, data);
    }    
}

void GenerateFrame::TransferDataLong(uint32_t can_id, uint8_t block_sequence_counter, std::vector<uint8_t> transfer_request, bool first_frame) 
{
    if (first_frame)
    {
        std::vector<uint8_t> data = {0x10, static_cast<uint8_t>(transfer_request.size() + 2), 0x36, block_sequence_counter};
        for (uint8_t bit = 0; bit < 4; bit++) 
        {
            data.push_back(transfer_request[bit]);
        }
        this->SendFrame(can_id, data);
    }
    else
    {
        transfer_request.erase(transfer_request.begin(), transfer_request.begin() + 4);
        std::vector<uint8_t> data;
        for (uint8_t bit = 0; bit < transfer_request.size() / 7; bit++)
        {
            data = {static_cast<uint8_t>(0x21 + (bit % 0xF))};
            for (uint8_t d_bit = 0; d_bit < 7 && ((bit * 7) + d_bit) < transfer_request.size(); d_bit++) 
            {
                data.push_back(transfer_request[bit * 7 + d_bit]);
            }
            this->SendFrame(can_id, data);
        }
    }
}

void GenerateFrame::RequestTransferExit(uint32_t can_id, bool response) 
{
    if (response) 
    {
        this->SendFrame(can_id, {0x01, 0x77});
    }
    else 
    {
        this->SendFrame(can_id, {0x01, 0x37});
    }
}

void GenerateFrame::InsertBytes(std::vector<uint8_t>& data, uint8_t index, uint8_t num_bytes) 
{
    for (uint8_t bit = 0; bit < num_bytes; bit++) 
    {
        data.insert(data.begin() + index, 0x00);
    }
}

int GenerateFrame::CountDigits(int number) 
{
    int count = 0;
    if (number <= 0) 
    {
        return 1;
    }
    while (number != 0) 
    {
        number /= 10;
        count++;
    }
    return count;
}
