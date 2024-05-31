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
GenerateFrame::GenerateFrame() {}

/* Function to get the socket */
int GenerateFrame::getSocket() 
{
    return this->socket;
}

/* Function to create a CAN frame */
can_frame GenerateFrame::CreateFrame(int can_id, std::vector<int> data, FrameType frameType) 
{
    struct can_frame frame;
    frame.can_id = can_id;

    switch (frameType) 
    {
        case DATA_FRAME:
            frame.can_id &= CAN_EFF_MASK;
            frame.can_dlc = data.size();
            std::memcpy(frame.data, data.data(), data.size());
            break;
        case REMOTE_FRAME:
            frame.can_id &= CAN_EFF_MASK;
            frame.can_id |= CAN_RTR_FLAG;
            frame.can_dlc = data.size();
            std::memcpy(frame.data, data.data(), data.size());
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
int GenerateFrame::SendFrame(int can_id, std::vector<int> data, FrameType frameType) 
{
    if (this->socket < 0)
    {
        throw std::runtime_error("Socket not initialized");
    }

    /* Create the CAN frame */
    frame = CreateFrame(can_id, data, frameType);

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
    return frame;
}

void GenerateFrame::SessionControl(int can_id, int subfunction, bool response) 
{
    if (response) this->SendFrame(can_id, {0x2, 0x50, subfunction});
    else this->SendFrame(can_id, {0x2, 0x10, subfunction});
}

void GenerateFrame::EcuReset(int can_id, bool response) 
{
    if (response) this->SendFrame(can_id, {0x2, 0x51, 0x3});
    else this->SendFrame(can_id, {0x2, 0x11, 0x3});
}

void GenerateFrame::ReadDataByIdentifier(int can_id, int dataIdentifier, std::vector<int> response) 
{
    if (response.empty())
    {
        std::vector<int> data = {0x3, 0x22, dataIdentifier/0x100, dataIdentifier%0x100};
        this->SendFrame(can_id, data);
    }
    else if (response.size() <= 5)
    {
        std::vector<int> data = {(int)response.size() + 3, 0x62, dataIdentifier/0x100, dataIdentifier%0x100};
        data.insert(data.end(), response.begin(), response.end());
        this->SendFrame(can_id, data);
    }
    else
    {
        std::cout << "Response size is too large" << std::endl;
    }
}

void GenerateFrame::CreateFrameLong(int can_id, int sid, int dataIdentifier, std::vector<int> response, bool firstFrame) 
{
    if (firstFrame)
    {
        std::vector<int> data = {0x10, (int)response.size() + 3, sid, dataIdentifier/0x100, dataIdentifier%0x100};
        for (int i = 0; i < 3; i++) data.push_back(response[i]);
        this->SendFrame(can_id, data);
    }
    else
    {
        response.erase(response.begin(), response.begin() + 3);
        std::vector<int> data;
        for (int i = 0; i < response.size() / 7; i++)
        {
            data = {0x21 + (i % 0xF)};
            for (int j = 0; j < 7 && ((i * 7) + j) < response.size(); j++) data.push_back(response[i * 7 + j]);
            this->SendFrame(can_id, data);
        }
    }
}

void GenerateFrame::ReadDataByIdentifierLong(int can_id, int dataIdentifier, std::vector<int> response, bool firstFrame) 
{
    CreateFrameLong(can_id, 0x62, dataIdentifier, response, firstFrame);
}

void GenerateFrame::FlowControlFrame(int can_id) 
{
    this->SendFrame(can_id, {0x30, 0x00, 0x00, 0x00});
}

void GenerateFrame::AuthenticationRequestSeed(int can_id, const std::vector<int>& seed) 
{
    if (seed.empty()) this->SendFrame(can_id, {0x3, 0x29, 0x1});
    else
    {
        std::vector<int> data = {(int)seed.size() + 2, 0x69, 0x1};
        for (int i = 0; i < seed.size(); i++) data.push_back(seed[i]);
        this->SendFrame(can_id, data);
    }
}

void GenerateFrame::AuthenticationSendKey(int can_id, const std::vector<int>& key) 
{
    if (key.empty()) 
    {
        std::vector<int> data = {(int)key.size() + 2, 0x29, 0x2};
        for (int i = 0; i < key.size(); i++) data.push_back(key[i]);
        this->SendFrame(can_id, data);
    }
    else this->SendFrame(can_id, {0x02, 0x69, 0x02});
}

void GenerateFrame::RoutineControl(int can_id, int subfunction, int routine_identifier, bool response) 
{
    if (response) this->SendFrame(can_id, {0x4, 0x71, subfunction, routine_identifier / 0x100, routine_identifier % 0x100});
    else this->SendFrame(can_id, {0x4, 0x31, subfunction, routine_identifier / 0x100, routine_identifier % 0x100});
}

void GenerateFrame::TesterPresent(int can_id, bool response) 
{
    if (response) this->SendFrame(can_id, {0x02, 0x7E, 0x00});
    else this->SendFrame(can_id, {0x2, 0x3E, 0x00});
}

void GenerateFrame::ReadMemoryByAddress(int can_id, int memory_size, int memory_address, std::vector<int> response) 
{
    int len_mem_size = CountDigits(memory_size + 1) / 2;
    int len_mem_addr = CountDigits(memory_address + 1) / 2;
    int len_mem = len_mem_size * 0x10 + len_mem_addr;
    if (response.empty())
    {
        int pci_len = len_mem_size + len_mem_addr + 2;
        std::vector<int> data = {pci_len, 0x23, len_mem};
        InsertBytes(data, memory_address, len_mem_addr);
        InsertBytes(data, memory_size, len_mem_size);
        this->SendFrame(can_id, data);
    }
    else
    {
        int pci_len = len_mem_size + len_mem_addr + 2 + response.size();
        std::vector<int> data = {pci_len, 0x63, len_mem};
        InsertBytes(data, memory_address, len_mem_addr);
        InsertBytes(data, memory_size, len_mem_size);
        if (data.size() + response.size() < 9)
        {
            for (int i = 0; i < response.size(); i++) data.push_back(response[i]);
            this->SendFrame(can_id, data);
        }
        else
        {
            std::cout << "Response size is too large" << std::endl;
        }
    }
}

void GenerateFrame::ReadMemoryByAddressLong(int can_id, int memory_size, int memory_address, std::vector<int> response, bool first_frame) 
{
    int len_mem_size = CountDigits(memory_size + 1) / 2;
    int len_mem_addr = CountDigits(memory_address + 1) / 2;
    int len_mem = len_mem_size * 0x10 + len_mem_addr;
    if (first_frame)
    {
        int pci_len = response.size() + len_mem_size + len_mem_addr + 2;
        std::vector<int> data = {0x10, pci_len, 0x63, len_mem};
        InsertBytes(data, memory_address, len_mem_addr);
        InsertBytes(data, memory_size, len_mem_size);
        for (int i = 0; i < 3; i++) data.push_back(response[i]);
        this->SendFrame(can_id, data);
    }
    else
    {
        int mem_sent = 8 - len_mem_size - len_mem_addr - 4;
        response.erase(response.begin(), response.begin() + mem_sent);
        std::vector<int> data;
        for (int i = 0; i < response.size() / 7; i++)
        {
            data = {0x21 + (i % 0xF)};
            for (int j = 0; j < 7 && ((i * 7) + j) < response.size(); j++) data.push_back(response[i * 7 + j]);
            this->SendFrame(can_id, data);
        }
    }
}

void GenerateFrame::WriteDataByIdentifier(int can_id, int identifier, std::vector<int> data_parameter) 
{
    if (data_parameter.empty()) this->SendFrame(can_id, {0x03, 0x6E, identifier / 0x100, identifier % 0x100});
    else
    {
        if (data_parameter.size() <= 4)
        {
            std::vector<int> data = {(int)data_parameter.size() + 3, 0x2E, identifier / 0x100, identifier % 0x100};
            data.insert(data.end(), data_parameter.begin(), data_parameter.end());
            this->SendFrame(can_id, data);
        }
        else
        {
            std::cout << "Data parameter size is too large" << std::endl;
        }
    }
}

void GenerateFrame::WriteDataByIdentifierLong(int can_id, int identifier, std::vector<int> data_parameter, bool first_frame) 
{
    CreateFrameLong(can_id,0x2E,identifier,data_parameter,first_frame);
}

void GenerateFrame::ReadDtcInformation(int can_id, int subfunction, int dtc_status_mask) 
{
    this->SendFrame(can_id, {0x03, 0x19, subfunction, dtc_status_mask});
}

void GenerateFrame::ReadDtcInformationResponse01(int can_id, int status_availability_mask, int dtc_format_identifier, int dtc_count) 
{
    this->SendFrame(can_id, {0x03, 0x59, 0x01, status_availability_mask, dtc_format_identifier, dtc_count});
}

void GenerateFrame::ClearDiagnosticInformation(int can_id, std::vector<int> group_of_dtc, bool response) 
{
    std::vector<int> data;
    if (response) this->SendFrame(can_id, {0x01, 0x54});
    else 
    {
        if (group_of_dtc.size() < 8)
        {
            data = {(int)group_of_dtc.size() + 1, 0x14};
            for (int i = 0; i < group_of_dtc.size(); i++) data.push_back(group_of_dtc[i]);
            this->SendFrame(can_id, data);
        }
        else std::cout << "Group of DTC size is too large" << std::endl;
    }
}

void GenerateFrame::AccessTimingParameters(int can_id, int subfunction, bool response) 
{
    if (response) this->SendFrame(can_id, {0x02, 0xC3, subfunction});
    else this->SendFrame(can_id, {0x02, 0x83, subfunction});
}

void GenerateFrame::NegativeResponse(int can_id, int sid, int nrc) 
{
    this->SendFrame(can_id, {0x03, 0x7F, sid, nrc});
}

void GenerateFrame::RequestDownload(int can_id, int data_format_identifier, int memory_address, int memory_size) 
{
    int len_mem_size = CountDigits(memory_size + 1) / 2;
    int len_mem_addr = CountDigits(memory_address + 1) / 2;
    int len_mem = len_mem_size * 0x10 + len_mem_addr;
    int pci_len = len_mem_size + len_mem_addr + 3;
    std::vector<int> data = {pci_len, 0x34, data_format_identifier, len_mem};
    InsertBytes(data, memory_address, len_mem_addr);
    InsertBytes(data, memory_size, len_mem_size);
    this->SendFrame(can_id, data);
}

void GenerateFrame::RequestDownloadResponse(int can_id, int max_number_block) 
{
    int len_max_num_block = CountDigits(max_number_block + 1) / 2;
    std::vector<int> data = {len_max_num_block + 2, 0x74, (len_max_num_block * 0x10)};
    InsertBytes(data, max_number_block, len_max_num_block);
    this->SendFrame(can_id, data);
}

/* TO DO */
void GenerateFrame::TransferData(int can_id, int block_sequence_counter, std::vector<int> transfer_request) 
{
    if (transfer_request.empty()) this->SendFrame(can_id, {0x02, 0x76, block_sequence_counter});
    else
    {
        std::vector<int> data = {(int)transfer_request.size() + 2, 0x36, block_sequence_counter};
        for (int i = 0; i < transfer_request.size(); i++) data.push_back(transfer_request[i]);
        this->SendFrame(can_id, data);
    }    
}

/* TO DO */
void GenerateFrame::TransferDataLong(int can_id, int block_sequence_counter, std::vector<int> transfer_request, bool first_frame) 
{
    if (first_frame)
    {
        std::vector<int> data = {0x10, (int)transfer_request.size() + 2, 0x36, block_sequence_counter};
        for (int i = 0; i < 4; i++) data.push_back(transfer_request[i]);
        this->SendFrame(can_id, data);
    }
    else
    {
        transfer_request.erase(transfer_request.begin(), transfer_request.begin() + 4);
        std::vector<int> data;
        for (int i = 0; i < transfer_request.size() / 7; i++)
        {
            data = {0x21 + (i % 0xF)};
            for (int j = 0; j < 7 && ((i * 7) + j) < transfer_request.size(); j++) data.push_back(transfer_request[i * 7 + j]);
            this->SendFrame(can_id, data);
        }
    }
}

void GenerateFrame::RequestTransferExit(int id, bool response) 
{
    if (response) this->SendFrame(id, {0x01, 0x77});
    else this->SendFrame(id, {0x01, 0x37});
}

void GenerateFrame::InsertBytes(std::vector<int>& data, unsigned int index, int num_bytes) 
{
    for (int i = 0; i < num_bytes; i++) data.insert(data.begin() + index, 0x00);
}

int GenerateFrame::CountDigits(int number) 
{
    int count = 0;
    if (number <= 0) return 1;
    while (number != 0) 
    {
        number /= 10;
        count++;
    }
    return count;
}
