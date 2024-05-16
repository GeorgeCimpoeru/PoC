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
void GenerateFrames::SessionControl(int id, int sub_function, bool response=false)
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
void GenerateFrames::EcuReset(int id, bool response=false)
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
void GenerateFrames::AuthenticationRequestSeed(int id, bool response=false, const std::vector<int> &seed = {})
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
void GenerateFrames::AuthenticationSendKey(int id, std::vector<int> &key, bool response=false)
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
//Incomplete
void GenerateFrames::RoutineControl(int id, bool response=false)
{
    
}
void GenerateFrames::TesterPresent(int id, bool response=false)
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
    //Incomplete: According to documentation, maybe is necessary to implement
    //more subfunctions for this Service frame
    //  |
    //  V
}
void GenerateFrames::ReadDataByIdentifier(int id,int identifier, std::vector<int> response = {})
{
    if (response.size() == 0)
    {
        std::vector<int> data = {0x02,0x22,identifier/0x100,identifier%0x100};
        this->SendFrame(id, data);
        return;
    }
    for (int i = 0; i < response.size() / 5; i++)
    {
        std::vector<int> data = {0x02,0x62,identifier/0x100,identifier%0x100};
        for (int j = 0; j < 5 || ((i*5) + j) >= response.size(); j++)
        {
            data.push_back(response[(i*5)+j]);
        }
        this->SendFrame(id, data);
    }
    return;
    //Incomplete: According to documentation, the frame can
    //request more than one DID. Future implementation
    //  |
    //  V
}
void GenerateFrames::ReadMemoryByAdress(int id, int address_and_length_identifier, int memory_size, int memory_address, std::vector<int> response = {})
{
    if (response.size() == 0)
    {
        std::vector<int> data = {0x23, address_and_length_identifier,memory_size/100,memory_size%100,memory_address/100,memory_address%100};
        this->SendFrame(id, data);
        return;
    }
    for (int i = 0; i < response.size() / 8; i++)
    {
        std::vector<int> data = {0x063};
        for (int j = 0; j < 8 || ((i*8) + j) >= response.size(); j++)
        {
            data.push_back(response[(i*8)+j]);
        }
        this->SendFrame(id, data);
    }
    return;
}
void GenerateFrames::WriteDataByIdentifier(int id, int identifier, std::vector<int> data_parameter = {})
{
    if (data_parameter.size() > 0)
    {
        for (int i = 0; i < data_parameter.size() / 6; i++)
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
    std::vector<int> data = {0x6E, identifier/100,identifier%100};
    this->SendFrame(id, data);
    return;
}
//Incomplete: This next three method (ReadDtcInformation, ClearDiagnosticInfirmation,
//AccessTimingParameters) need to be improved
void GenerateFrames::ReadDtcInformation(int id, bool response=false)
{
    if (!response)
    {
        std::vector<int> data = {0x19};
        this->SendFrame(id, data);
        return;
    }
    std::vector<int> data = {0x59};
    this->SendFrame(id, data);
    return;
}
void GenerateFrames::ClearDiagnosticInfirmation(int id, bool response=false)
{
    if (!response)
    {
        std::vector<int> data = {0x14};
        this->SendFrame(id, data);
        return;
    }
    std::vector<int> data = {0x54};
    this->SendFrame(id, data);
    return;
}
void GenerateFrames::AccessTimingParameters(int id, bool response=false)
{
    if (!response)
    {
        std::vector<int> data = {0x83};
        this->SendFrame(id, data);
        return;
    }
    std::vector<int> data = {0xC3};
    this->SendFrame(id, data);
    return;
}
void GenerateFrames::NegativeResponse(int id, int nrc, bool response=false)
{

}
//Incomplete:Need to be improved!!!
void GenerateFrames::RequestDownload(int id, int data_format_identifier, int address_length_identifier,
                                     int memory_address, int memory_size, bool response=false)
{
    if (!response)
    {
        std::vector<int> data = {/*Length*/0x7,0x34, data_format_identifier,address_length_identifier,memory_address,memory_size};
        this->SendFrame(id, data);
        return;
    }

    //Incomplete: Response need to be improved
    std::vector<int> data = {0x74};
    this->SendFrame(id, data);
    return;
}

//Incomplete: Need to be improved!!!
void GenerateFrames::TransferData(int id, int block_sequence_counter, int transfer_request, bool response=false)
{
    if (!response)
    {
        std::vector<int> data = {/*Length*/0x04, 0x36, block_sequence_counter, transfer_request};
        this->SendFrame(id, data);
        return;
    }

    //Response need to be improved
    std::vector<int> data = {0x02,0x76,block_sequence_counter};
    this->SendFrame(id, data);
    return;
}
void GenerateFrames::RequestTransferExit(int id, int transfer_request, bool response=false)
{
    if (!response)
    {
        std::vector<int> data = {/*Length*/0x3, 0x37, transfer_request};
        this->SendFrame(id, data);
        return;
    }
    //Response need to be improved
    std::vector<int> data = {0x02,0x77,/*block_sequence_counter or Tranfer record??*/0x04};
    this->SendFrame(id, data);
    return;
}
bool GenerateFrames::RequestUpdateStatus(int id, bool response=false)
{
    //No impplementation, I don't find this service in the standart
}