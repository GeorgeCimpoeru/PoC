#include "GenerateFrames.h"

GenerateFrames::GenerateFrames(int socket)
{
    this->AddSocket(socket);
}
struct can_frame GenerateFrames::CreateFrame(int &id,  std::vector<int> &data)
{
    struct can_frame frame;
    frame.can_id = id;
    frame.can_dlc = data.size();
    for (int i = 0; i < frame.can_dlc; i++)
    {
        frame.data[i] = data[i];
    }
    return frame;
}
bool GenerateFrames::SendFrame(int id,  std::vector<int> data)
{
    struct can_frame frame = CreateFrame(id, data);
    int nbytes = write(this->socket, &frame, sizeof(frame));
    if (nbytes != sizeof(frame))
    {
        std::cout<<"Write error\n";
        return -1;
    }
    return 0;
}
void GenerateFrames::AddSocket(int socket)
{
    if (socket >= 0)
    {
        this->socket = socket;
        return;
    }
    std::cout<<"Error: Pass a valid Socket\n";
    exit(EXIT_FAILURE);
}
void GenerateFrames::SessionControl(int id, int sub_function, bool response)
{
    std::vector<int> data(3);
    if (!response)
    {
        data = {0x2,0x10,sub_function};
        this->SendFrame(id,data);
        return;
    }
    data = {0x2,0x50,sub_function};
    this->SendFrame(id, data);
    return;
}
void GenerateFrames::EcuReset(int id, bool response)
{
    std::vector<int> data(3);
    if (!response)
    {
        data = {0x2,0x11,0x3};
        this->SendFrame(id, data);
        return;
    }
    data = {0x2,0x51,0x3};
    this->SendFrame(id, data);
    return;
}
void GenerateFrames::AuthenticationRequestSeed(int id, bool response, const std::vector<int> &seed)
{
    if (!response)
    {
        std::vector<int> data = {0x29,0x1};
        this->SendFrame(id, data);
        return;
    }
    std::vector<int> data(2 + seed.size());
    data[0] = 0x69;
    data[1] = 0x1;
    for (int i = 0; i<seed.size(); i++)
    {
        data[i+2] = seed[i];
    }
    this->SendFrame(id, data);
    return;
}
void GenerateFrames::AuthenticationSendKey(int id, std::vector<int> &key, bool response)
{
    if (!response)
    {
        std::vector<int> data(2 + key.size());
        data[0] = 0x29;
        data[1] = 0x02;
        for (int i = 0; i<key.size(); i++)
        {
            data[i+2] = key[i];
        }
        this->SendFrame(id, data);
        return;
    }
    std::vector<int> data = {0x69,0x02};
    this->SendFrame(id, data);
    return;
}
void GenerateFrames::RoutineControl(int id, int sub_function, int routin_identifier, bool response)
{
    if (!response)
    {
        //PCI???
        std::vector<int> data = {0x02,0x31, sub_function, routin_identifier / 0x100, routin_identifier % 0x100};
        this->SendFrame(id, data);
        return;
    }
    std::vector<int> data = {0x04,0x71, sub_function, routin_identifier / 0x100, routin_identifier % 0x100};
    this->SendFrame(id, data);
    return;
}
void GenerateFrames::TesterPresent(int id, bool response)
{
    if (!response)
    {
        std::vector<int> data = {0x02,0x3E,0x00};
        this->SendFrame(id, data);
        return;
    }
    std::vector<int> data = {0x02,0x7E,0x00};
    this->SendFrame(id, data);
    return;
    //According to documentation, maybe is necessary to implement
    //more subfunctions for this Service frame
    //  |
    //  V
}
void GenerateFrames::ReadDataByIdentifier(int id,int identifier, std::vector<int> response )
{
    if (response.size() == 0)
    {
        std::vector<int> data = {0x03, 0x22, identifier/0x100, identifier%0x100};
        this->SendFrame(id, data);
        return;
    }
    int length_response = response.size();
    if (length_response <= 4)
    {
        std::vector<int> data = {0x03, 0x62, identifier/0x100, identifier%0x100};
        for (int i = 0; i < length_response; i++)
        {
            data.push_back(response[i]);
        }
        this->SendFrame(id, data);
        return;
    }
    std::cout<<"ERROR: The frame is to long!, consider using method ReadDataByIdentifierLongResponse\n";
    return;
    //According to documentation, the frame can
    //request more than one DID. Future implementation
    //  |
    //  V
}
void GenerateFrames::GenerateFrameLongData(int id, int sid, int identifier, std::vector<int> response, bool first_frame)
{
    if (first_frame)
    {
        std::vector<int> data = {0x10, (int)response.size(), sid, identifier/0x100, identifier%0x100};
        //Send only 3 first bytes of data
        for (int i = 0; i < 3; i++)
        {
            data.push_back(response[i]);
        }
        this->SendFrame(id, data);
        return;
    }
    else
    {
        //Delete first 3 data that were sended in the first frame
        response.erase(response.begin(), response.begin() + 3);
        int size_of_response = response.size();
        std::vector<int> data;
        for (int i = 0; i <= size_of_response / 7; i++)
        {
            data = {0x20+i};
            for (int j = 0; j < 7 && ((i*7) + j) < size_of_response; j++)
            {
                data.push_back(response[(i*7)+j]);
            }
            this->SendFrame(id, data);
        }
        return;
    } 
}
void GenerateFrames::ReadDataByIdentifierLongResponse(int id,int identifier, std::vector<int> response, bool first_frame)
{
    GenerateFrameLongData(id,0x22,identifier,response,first_frame);
}
void GenerateFrames::FlowControlFrame(int id)
{
    this->SendFrame(id, {0x30,0x00,0x00,0x00});
}
//Need more information from standar do handle long responses
void GenerateFrames::ReadMemoryByAdress(int id, int memory_size, int memory_address, std::vector<int> response )
{
    //add lengths of of memory size/address to the frame
    int length_memory_size = numDigits(memory_size) / 2;
    int length_memory_address = numDigits(memory_address) / 2;
    int length_memory = length_memory_address * 0x10 + length_memory_size;
    if (response.size() == 0)
    {
        std::vector<int> data = {0x23, length_memory};
        //add memory address and size to the frame
        insertBytes(data, memory_address, length_memory_address);
        insertBytes(data, memory_size, length_memory_size);
        this->SendFrame(id, data);
        return;
    }
    std::vector<int> data = {0x63, length_memory};
    //add memory address and size to the frame
    insertBytes(data, memory_address, length_memory_address);
    insertBytes(data, memory_size, length_memory_size);
    if (data.size() + response.size() < 9)
    {
        for (int i = 0; i < response.size(); i++)
        {
            data.push_back(response[i]);
        }
        this->SendFrame(id, data);
        return;
    } else
    {
        std::cout<<"ERROR: Response to long, consider using ReadMemoryByAdressLongResponse method\n";
        return;
    }
}
void GenerateFrames::ReadMemoryByAdressLongResponse(int id, int memory_size, int memory_address, std::vector<int> response, bool first_frame)
{
    //add lengths of of memory size/address to the frame
    int length_memory_size = numDigits(memory_size) / 2;
    int length_memory_address = numDigits(memory_address) / 2;
    int length_memory = length_memory_address * 0x10 + length_memory_size;
    if (first_frame)
    {
        std::vector<int> data = {0x10, (int)response.size(), 0x23, length_memory};
        //add memory address and size to the frame
        insertBytes(data, memory_address, length_memory_address);
        insertBytes(data, memory_size, length_memory_size);
        //Send only 3 first bytes of data
        for (int i = 0; i < (data.size() - 8); i++)
        {
            data.push_back(response[i]);
        }
        this->SendFrame(id, data);
        return;
    }
    else
    {
        int memory_allready_send = 8 - (length_memory_address + length_memory_size + 4);
        //Delete data allready sended
        response.erase(response.begin(), response.begin() + memory_allready_send);
        int size_of_response = response.size();
        std::vector<int> data;
        for (int i = 0; i <= size_of_response / 7; i++)
        {
            data = {0x20+i};
            for (int j = 0; j < 7 && ((i*7) + j) < size_of_response; j++)
            {
                data.push_back(response[(i*7)+j]);
            }
            this->SendFrame(id, data);
        }
        return;
    } 
    return;
}
void GenerateFrames::WriteDataByIdentifier(int id, int identifier, std::vector<int> data_parameter )
{
    if (data_parameter.size() > 0)
    {
        if (data_parameter.size() <= 4)
        {
            for (int i = 0; i <= data_parameter.size() / 6; i++)
            {
                std::vector<int> data = {0x6E, identifier/100,identifier%100};
                for (int j = 0; j < 6 || ((i*6) + j) >= data_parameter.size(); j++)
                {
                    data.push_back(data_parameter[(i*6)+j]);
                }
                this->SendFrame(id, data);
            }
            return;
        }
        else
        {
            std::cout<<"The data_parameter is to long. Consider using WriteDataByIdentifierLongData method\n";
            return;
        }
    }
    std::vector<int> data = {0x6E, identifier/100,identifier%100};
    this->SendFrame(id, data);
    return;
}
void GenerateFrames::WriteDataByIdentifierLongData(int id, int identifier, std::vector<int> data_parameter, bool first_frame)
{
    GenerateFrameLongData(id,0x2E,identifier,data_parameter,first_frame);
}
//Read DTC methods
void GenerateFrames::ReadDtcInformation(int id, int sub_function, int dtc_status_mask)
{
    std::vector<int> data = {0x03, 0x19, sub_function, dtc_status_mask};
    this->SendFrame(id, data);
    return;
}
void GenerateFrames::ReadDtcInformationResponse01(int id, int sub_function, int status_availability_mask, int dtc_format_identifier, int dtc_count)
{
    std::vector<int> data = {0x03, 0x19, sub_function, status_availability_mask, dtc_format_identifier, dtc_count};
    this->SendFrame(id, data);
    return;
}
void GenerateFrames::ClearDiagnosticInformation(int id, std::vector<int> group_of_dtc, bool response)
{
    std::vector<int> data;
    if (!response)
    { 
        int number_of_dtc = group_of_dtc.size();
        for (int i = 0; i <= number_of_dtc / 7; i++)
        {
            int pci_length = number_of_dtc - (6 * i) > 6? 7 : (number_of_dtc % 6) + 1;
            data = {pci_length ,0x14};
            for (int j = 0; j < 6 && ((i*6) + j) < number_of_dtc; j++)
            {
                data.push_back(group_of_dtc[(i*6)+j]);
            }
            this->SendFrame(id, data);
        }
        return;
    }
    data = {0x1, 0x54};
    this->SendFrame(id, data);
    return;
}
void GenerateFrames::AccessTimingParameters(int id, int sub_function, bool response)
{
    if (!response)
    {
        std::vector<int> data = {0x02, 0x83, sub_function};
        this->SendFrame(id, data);
        return;
    }
    std::vector<int> data = {0x02, 0xC3, sub_function};
    this->SendFrame(id, data);
    return;
}
void GenerateFrames::NegativeResponse(int id, int nrc)
{
    std::vector<int> data = {0x03, 0x7F, nrc};
    this->SendFrame(id, data);
    return;
}
// https://piembsystech.com/request-download-0x34-service-uds-protocol/
void GenerateFrames::RequestDownload(int id, int data_format_identifier, int memory_address, int memory_size, int max_number_block)
{
    if (!max_number_block)
    {
        //Request Frame
        //add lengths of of memory size/address to the frame
        int length_memory_size = numDigits(memory_size) / 2;
        int length_memory_address = numDigits(memory_address) / 2;
        int length_memory = length_memory_size * 0x10 + length_memory_address;
        int pci_length = length_memory_size + length_memory_address + 3;
        std::vector<int> data = {pci_length, 0x34, data_format_identifier, length_memory};
        //add memory address and size to the frame
        insertBytes(data, memory_address, length_memory_address);
        insertBytes(data, memory_size, length_memory_size);
        this->SendFrame(id, data);
        return;
    }
    //Response frame
    int length_max_number_block = numDigits(max_number_block) / 2;
    std::vector<int> data = {length_max_number_block + 3, 0x74, (length_max_number_block * 0x10), max_number_block};
    this->SendFrame(id, data);
    return;
}
void GenerateFrames::TransferData(int id, int block_sequence_counter, std::vector<int> transfer_request)
{
    //If is not a response
    if (transfer_request.size() == 0)
    {
        std::vector<int> data = {(int)transfer_request.size() + 2, 0x36, block_sequence_counter};
        for (int i = 0; i < transfer_request.size(); i++)
        {
            data.push_back(transfer_request[i]);
        }
        this->SendFrame(id, data);
        return;
    }
    //Response frame
    std::vector<int> data = {0x02,0x76,block_sequence_counter};
    this->SendFrame(id, data);
    return;
}
void GenerateFrames::RequestTransferExit(int id, bool response)
{
    if (!response)
    {
        std::vector<int> data = {0x01, 0x37};
        this->SendFrame(id, data);
        return;
    }
    std::vector<int> data = {0x01,0x77};
    this->SendFrame(id, data);
    return;
}
bool GenerateFrames::RequestUpdateStatus(int id, bool response)
{
    //No impplementation, I don't find this service in the standart
    return false;
}
//Private
int GenerateFrames::numDigits(int number)
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