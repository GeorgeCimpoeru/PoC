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

void GenerateFrame::ReadDataByIdentifierLong(int can_id, int sid, int dataIdentifier, std::vector<int> response, bool firstFrame) 
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
