#include "GenerateFrames.h"

GenerateFrames::GenerateFrames(int socket)
{
    this->AddSocket(socket);
}

struct can_frame GenerateFrames::CreateFrame(int &id, int data_length, int *data)
{
    struct can_frame frame;
    frame.can_id = id;
    frame.can_dlc = data_length;
    for (int i = 0; i < data_length; i++)
    {
        frame.data[i] = data[i];
    }
    return frame;
}

bool GenerateFrames::SendFrame(int id, int data_length, int *data)
{
    struct can_frame frame = CreateFrame(id, data_length, data);
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