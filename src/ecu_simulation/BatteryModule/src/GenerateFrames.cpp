#include "../include/GenerateFrames.h"

GenerateFrames::GenerateFrames(int socket)
{
    this->addSocket(socket);
}

int GenerateFrames::getSocket()
{
    return this->socket;
}

struct can_frame GenerateFrames::createFrame(int &id,  std::vector<int> &data, FrameType frameType)
{
    struct can_frame frame;
    switch (frameType)
    {
        case DATA_FRAME:
            frame.can_id = id & CAN_EFF_MASK;
            frame.can_dlc = data.size();
            for (int i = 0; i < frame.can_dlc; i++)
            {
                frame.data[i] = data[i];
            }
            break;
        case REMOTE_FRAME:
            frame.can_id = id & CAN_EFF_MASK;
            frame.can_id |= CAN_RTR_FLAG;
            frame.can_dlc = data.size();
            for (int i = 0; i < frame.can_dlc; i++) 
            {
                frame.data[i] = data[i];
            }
            break;
        default:
            throw std::invalid_argument("Invalid frame type");
    }
    return frame;
}

bool GenerateFrames::sendFrame(int id, std::vector<int> data, FrameType frameType)
{
    struct can_frame frame = createFrame(id, data, frameType);
    int nbytes = write(this->socket, &frame, sizeof(frame));
    if (nbytes != sizeof(frame))
    {
        std::cout<<"Write error\n";
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
    std::cout<<"Error: Pass a valid Socket\n";
    exit(EXIT_FAILURE);
}

void GenerateFrames::sessionControl(int id, int sub_function, bool response)
{
    std::vector<int> data(3);
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

void GenerateFrames::ecuReset(int id, bool response)
{
    std::vector<int> data(3);
    if (!response)
    {
        data = {0x2,0x11,0x3};
        this->sendFrame(id, data);
        return;
    }
    data = {0x2,0x51,0x3};
    this->sendFrame(id, data);
    return;
}

void GenerateFrames::authenticationRequestSeed(int id, const std::vector<int> &seed)
{
    if (seed.size() == 0)
    {
        std::vector<int> data = {0x03, 0x29, 0x1};
        this->sendFrame(id, data);
        return;
    }
    std::vector<int> data = {(int)seed.size() + 2, 0x69, 0x1};
    for (std::size_t i = 0; i < seed.size(); i++)
    {
        data.push_back(seed[i]);
    }
    this->sendFrame(id, data);
    return;
}

void GenerateFrames::authenticationSendKey(int id, const std::vector<int> &key)
{
    if (key.size() > 0 )
    {
        std::vector<int> data = {(int)key.size() + 2, 0x29, 0x2};
        for (std::size_t i = 0; i<key.size(); i++)
        {
            data.push_back(key[i]);
        }
        this->sendFrame(id, data);
        return;
    }
    std::vector<int> data = {0x02,0x69,0x02};
    this->sendFrame(id, data);
    return;
}

void GenerateFrames::routineControl(int id, int sub_function, int routin_identifier, bool response)
{
    if (!response)
    {
        std::vector<int> data = {0x4, 0x31, sub_function, routin_identifier / 0x100, routin_identifier % 0x100};
        this->sendFrame(id, data);
        return;
    }
    std::vector<int> data = {0x04,0x71, sub_function, routin_identifier / 0x100, routin_identifier % 0x100};
    this->sendFrame(id, data);
    return;
}

void GenerateFrames::testerPresent(int id, bool response)
{
    if (!response)
    {
        std::vector<int> data = {0x02,0x3E,0x00};
        this->sendFrame(id, data);
        return;
    }
    std::vector<int> data = {0x02,0x7E,0x00};
    this->sendFrame(id, data);
    return;
}

void GenerateFrames::readDataByIdentifier(int id,int identifier, std::vector<int> response )
{
    if (response.size() == 0)
    {
        std::vector<int> data = {0x03, 0x22, identifier/0x100, identifier%0x100};
        this->sendFrame(id, data);
        return;
    }
    int length_response = response.size();
    if (length_response <= 5)
    {
        std::vector<int> data = {length_response + 3, 0x62, identifier/0x100, identifier%0x100};
        for (int i = 0; i < length_response; i++)
        {
            data.push_back(response[i]);
        }
        this->sendFrame(id, data);
        return;
    }
    std::cout<<"ERROR: The frame is to long!, consider using method ReadDataByIdentifierLongResponse\n";
    return;
    /*
    According to documentation, the frame can
    request more than one DID. Future implementation
      |
      V
    */
}

void GenerateFrames::generateFrameLongData(int id, int sid, int identifier, std::vector<int> response, bool first_frame)
{
    if (first_frame)
    {
        std::vector<int> data = {0x10, (int)response.size() + 3, sid, identifier/0x100, identifier%0x100};
        /*Send only 3 first bytes of data*/
        for (int i = 0; i < 3; i++)
        {
            data.push_back(response[i]);
        }
        this->sendFrame(id, data);
        return;
    }
    else
    {
        /*Delete first 3 data that were sended in the first frame */
        response.erase(response.begin(), response.begin() + 3);
        int size_of_response = response.size();
        std::vector<int> data;
        for (int i = 0; i <= size_of_response / 7; i++)
        {
            data = {0x21 + (i % 0xF)};
            for (int j = 0; j < 7 && ((i*7) + j) < size_of_response; j++)
            {
                data.push_back(response[(i*7)+j]);
            }
            this->sendFrame(id, data);
        }
        return;
    } 
}

void GenerateFrames::readDataByIdentifierLongResponse(int id,int identifier, std::vector<int> response, bool first_frame)
{
    generateFrameLongData(id,0x62,identifier,response,first_frame);
}

void GenerateFrames::flowControlFrame(int id)
{
    this->sendFrame(id, {0x30,0x00,0x00,0x00});
}

void GenerateFrames::readMemoryByAddress(int id, int memory_size, int memory_address, std::vector<int> response )
{
    /*add lengths of of memory size/address to the frame*/
    int length_memory_size = (countDigits(memory_size) +1) / 2;
    int length_memory_address = (countDigits(memory_address) + 1) / 2;
    int length_memory = length_memory_size * 0x10 + length_memory_address;
    if (response.size() == 0)
    {
        int pci_l = length_memory_size + length_memory_address + 2;
        std::vector<int> data = {pci_l, 0x23, length_memory};
        /*add memory address and size to the frame*/
        insertBytes(data, memory_address, length_memory_address);
        insertBytes(data, memory_size, length_memory_size);
        this->sendFrame(id, data);
        return;
    }
    int pci_l = length_memory_size + length_memory_address + 2 + response.size();
    std::vector<int> data = {pci_l, 0x63, length_memory};
    /*add memory address and size to the frame*/
    insertBytes(data, memory_address, length_memory_address);
    insertBytes(data, memory_size, length_memory_size);
    if (data.size() + response.size() < 9)
    {
        for (std::size_t i = 0; i < response.size(); i++)
        {
            data.push_back(response[i]);
        }
        this->sendFrame(id, data);
        return;
    } else
    {
        std::cout<<"ERROR: Response to long, consider using ReadMemoryByAdressLongResponse method\n";
        return;
    }
}

void GenerateFrames::readMemoryByAddressLongResponse(int id, int memory_size, int memory_address, std::vector<int> response, bool first_frame)
{
    /*add lengths of of memory size/address to the frame*/
    int length_memory_size = (countDigits(memory_size) +1) / 2;
    int length_memory_address = (countDigits(memory_address) + 1) / 2;
    int length_memory = length_memory_size * 0x10 + length_memory_address;
    if (first_frame)
    {
        int pci_l = (int)response.size() + 2 + length_memory_size + length_memory_address;
        std::vector<int> data = {0x10, pci_l, 0x63, length_memory};
        /*add memory address and size to the frame*/
        insertBytes(data, memory_address, length_memory_address);
        insertBytes(data, memory_size, length_memory_size);
        /*Send only 3 first bytes of data*/
        for (std::size_t i = 0; i < (data.size() - 8); i++)
        {
            data.push_back(response[i]);
        }
        this->sendFrame(id, data);
        return;
    }
    else
    {
        int memory_allready_send = 8 - (length_memory_address + length_memory_size + 4);
        /*Delete data allready sended*/
        response.erase(response.begin(), response.begin() + memory_allready_send);
        int size_of_response = response.size();
        std::vector<int> data;
        for (int i = 0; i <= size_of_response / 7; i++)
        {
            data = {0x21 + (i % 0xF)};
            for (int j = 0; j < 7 && ((i*7) + j) < size_of_response; j++)
            {
                data.push_back(response[(i*7)+j]);
            }
            this->sendFrame(id, data);
        }
        return;
    } 
    return;
}

void GenerateFrames::writeDataByIdentifier(int id, int identifier, std::vector<int> data_parameter )
{
    if (data_parameter.size() > 0)
    {
        if (data_parameter.size() <= 4)
        {
            std::vector<int> data = {(int)data_parameter.size() + 3,0x2E, identifier/0x100,identifier%0x100};
            for (int j = 0; j < data_parameter.size(); j++)
            {
                data.push_back(data_parameter[j]);
            }
            this->sendFrame(id, data);
            return;
        }
        else
        {
            std::cout<<"The data_parameter is to long. Consider using WriteDataByIdentifierLongData method\n";
            return;
        }
    }
    std::vector<int> data = {0x03, 0x6E, identifier/0x100,identifier%0x100};
    this->sendFrame(id, data);
    return;
}

void GenerateFrames::writeDataByIdentifierLongData(int id, int identifier, std::vector<int> data_parameter, bool first_frame)
{
    generateFrameLongData(id,0x2E,identifier,data_parameter,first_frame);
}

void GenerateFrames::readDtcInformation(int id, int sub_function, int dtc_status_mask)
{
    std::vector<int> data = {0x03, 0x19, sub_function, dtc_status_mask};
    this->sendFrame(id, data);
    return;
}

void GenerateFrames::readDtcInformationResponse01(int id, int status_availability_mask, int dtc_format_identifier, int dtc_count)
{
    std::vector<int> data = {0x03, 0x59, 0x01, status_availability_mask, dtc_format_identifier, dtc_count};
    this->sendFrame(id, data);
    return;
}

void GenerateFrames::clearDiagnosticInformation(int id, std::vector<int> group_of_dtc, bool response)
{
    std::vector<int> data;
    /*Request*/
    if (!response)
    { 
        if (group_of_dtc.size() < 8)
        {
            int number_of_dtc = group_of_dtc.size();
            data = {number_of_dtc + 1, 0x14};
            for (int i = 0; i < number_of_dtc; i++)
            {
                data.push_back(group_of_dtc[i]);
            }
            this->sendFrame(id, data);
            return;
        } else
        {
            std::cout<<"ERROR: Can't send more than 7 DTC/frame, please consider send 2 or more frames\n";
            return;
        }
        
    }
    /*Response*/
    data = {0x01, 0x54};
    this->sendFrame(id, data);
    return;
}

void GenerateFrames::accessTimingParameters(int id, int sub_function, bool response)
{
    if (!response)
    {
        std::vector<int> data = {0x02, 0x83, sub_function};
        this->sendFrame(id, data);
        return;
    }
    std::vector<int> data = {0x02, 0xC3, sub_function};
    this->sendFrame(id, data);
    return;
}

void GenerateFrames::negativeResponse(int id, int sid, int nrc)
{
    std::vector<int> data = {0x03, 0x7F, sid, nrc};
    this->sendFrame(id, data);
    return;
}

void GenerateFrames::requestDownload(int id, int data_format_identifier, int memory_address, int memory_size)
{
    /* Request Frame
    add lengths of of memory size/address to the frame*/
    int length_memory_size = countDigits(memory_size + 1) / 2;
    int length_memory_address = countDigits(memory_address + 1) / 2;
    int length_memory = length_memory_size * 0x10 + length_memory_address;
    int pci_length = length_memory_size + length_memory_address + 3; //pci_l
    std::vector<int> data = {pci_length, 0x34, data_format_identifier, length_memory}; //pci_l
    /*add memory address and size to the frame*/
    insertBytes(data, memory_address, length_memory_address);
    insertBytes(data, memory_size, length_memory_size);
    this->sendFrame(id, data);
    return;
}

void GenerateFrames::requestDownloadResponse(int id, int max_number_block)
{
    /*Response frame*/
    int length_max_number_block = (countDigits(max_number_block) + 1) / 2;
    std::vector<int> data = {length_max_number_block + 2, 0x74, (length_max_number_block * 0x10)}; //pci_l
    insertBytes(data, max_number_block, length_max_number_block);
    this->sendFrame(id, data);
    return;
}

void GenerateFrames::transferData(int id, int block_sequence_counter, std::vector<int> transfer_request)
{
    /*If is not a response*/
    if (transfer_request.size() != 0)
    {
        std::vector<int> data = {(int)transfer_request.size() + 2, 0x36, block_sequence_counter}; //pci_l
        for (std::size_t i = 0; i < transfer_request.size(); i++)
        {
            data.push_back(transfer_request[i]);
        }
        this->sendFrame(id, data);
        return;
    }
    /*Response frame*/
    std::vector<int> data = {0x02,0x76,block_sequence_counter}; //pci_l
    this->sendFrame(id, data);
    return;
}

void GenerateFrames::transferDataLong(int id, int block_sequence_counter, std::vector<int> transfer_request, bool first_frame)
{
    if (first_frame)
    {
        std::vector<int> data = {0x10, ((int)transfer_request.size() + 2), 0x36, block_sequence_counter};
        for (int i = 0; i < 4 ; i++)
        {
            data.push_back(transfer_request[i]);
        }
        this->sendFrame(id, data);
        return;
    }
    else
    {
        /*Delete first 3 data that were sended in the first frame*/
        transfer_request.erase(transfer_request.begin(), transfer_request.begin() + 4);
        int size_of_data = transfer_request.size();
        std::vector<int> data;
        for (int i = 0; i <= size_of_data / 7; i++)
        {
            data = {0x21 + (i % 0xF)};
            for (int j = 0; j < 7 && ((i*7) + j) < size_of_data; j++)
            {
                data.push_back(transfer_request[(i*7)+j]);
            }
            this->sendFrame(id, data);
        }
        return;
    } 
}

void GenerateFrames::requestTransferExit(int id, bool response)
{
    if (!response)
    {
        std::vector<int> data = {0x01, 0x37};
        this->sendFrame(id, data);
        return;
    }
    std::vector<int> data = {0x01,0x77};
    this->sendFrame(id, data);
    return;
}

bool GenerateFrames::requestUpdateStatus(int id, bool response)
{
    /*No impplementation, I don't find this service in the standart*/
    return false;
}

/*Private*/
int GenerateFrames::countDigits(int number)
{
    int digits = 0;
    if (number < 0) 
        digits = 1;
    while (number) {
        number /= 10;
        digits++;
    }
    return digits;
}

void GenerateFrames::insertBytes(std::vector<int>& byteVector, unsigned int num, int numBytes) {
    for (int i = numBytes - 1; i >= 0; --i) {
        byteVector.push_back((num >> (i * 8)) & 0xFF);
    }
}