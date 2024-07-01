#include "../include/read_dtc_information.h"

ReadDTC::ReadDTC(std::string path_folder)
{
    this->path_folder = path_folder;
    Logger logger;
    this->generate = new GenerateFrames(this->creat_socket(),logger);

    std::cout<<"Here\n";
}

ReadDTC::~ReadDTC()
{
    delete this->generate;
}

void ReadDTC::read(int id, int sub_function, int dtc_status_mask)
{
    switch (sub_function)
    {
        case 0x01:
            std::cout<<"Sub-function 1\n";
            number_of_dtc(id,dtc_status_mask);
            break;
        case 0x02:
            dtcs(id,dtc_status_mask);
            std::cout<<"Sub-function 2\n";
            break;
    }
}

void ReadDTC::number_of_dtc(int id, int dtc_status_mask)
{
    std::ifstream MyReadFile(this->path_folder);
    
    int number_of_dtc = 0;
    std::string line;
    while (std::getline (MyReadFile, line))
    {
        number_of_dtc++;
    }
    MyReadFile.close();
    this->generate->readDtcInformationResponse01(id,dtc_status_mask,0x11,number_of_dtc);
}

void ReadDTC::dtcs(int id, int dtc_status_mask)
{
    std::ifstream MyReadFile(this->path_folder);

    std::string line;
    while (std::getline (MyReadFile, line))
    {
        std::cout << line;
    }
    MyReadFile.close();
}

int ReadDTC::creat_socket()
{
    int s;
    struct sockaddr_can addr;
    struct ifreq ifr;

    s = socket(PF_CAN, SOCK_RAW, CAN_RAW);

    strcpy(ifr.ifr_name, "vcan0" );
    ioctl(s, SIOCGIFINDEX, &ifr);

    addr.can_family = AF_CAN;
    addr.can_ifindex = ifr.ifr_ifindex;

    bind(s, (struct sockaddr *)&addr, sizeof(addr));
    return s;
}