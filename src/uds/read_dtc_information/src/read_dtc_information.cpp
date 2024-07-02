#include "../include/read_dtc_information.h"

ReadDTC::ReadDTC(std::string path_folder )
{
    this->path_folder = path_folder;
    Logger logger;
    this->socket_ = creat_socket();
    this->generate = new GenerateFrames(socket_,logger);

    std::cout<<"Here\n";
}

ReadDTC::~ReadDTC()
{
    delete this->generate;
}

void ReadDTC::read_dtc(int id, int sub_function, int dtc_status_mask)
{
    switch (sub_function)
    {
        case 0x01:
            std::cout<<"Sub-function 1\n";
            number_of_dtc(id,dtc_status_mask);
            break;
        case 0x02:
            std::cout<<"Sub-function 2\n";
            report_dtcs(id,dtc_status_mask);
            break;
    }
}

void ReadDTC::number_of_dtc(int id, int dtc_status_mask)
{
    std::ifstream MyReadFile(this->path_folder);

    uint8_t status_availability_mask=0;
    int number_of_dtc = 0;
    std::string line;
    /* Read from the file all dtcs */
    while (std::getline (MyReadFile, line))
    {
        int status = to_int(line[9]) * 0x10 + to_int(line[10]);
        status_availability_mask |= status;
        /* Filter by status mask */
        if ((status & dtc_status_mask) || dtc_status_mask == 0)
        {
            number_of_dtc++;
        }
    }
    MyReadFile.close();
    /* dtc format_idtf 0x01 -> ISO_14229-1_DTCFormat */
    int dtc_format_identifier = 0x01;
    /* Send a frame with number of dtc founded */
    this->generate->readDtcInformationResponse01(id,status_availability_mask,dtc_format_identifier,number_of_dtc);
}

void ReadDTC::report_dtcs(int id, int dtc_status_mask)
{
    std::ifstream MyReadFile(this->path_folder);
    
    uint8_t status_availability_mask=0;
    std::vector<std::pair<int, int>> dtc_and_status_list;
    std::string line;
    /* Read from the file all dtcs */
    while (std::getline (MyReadFile, line))
    {
        int status = to_int(line[9]) * 0x10 + to_int(line[10]);
        status_availability_mask |= status;
        /* Filter by status mask */
        if ((status & dtc_status_mask) || dtc_status_mask == 0)
        {
            int dtc_hex = dtc_to_hex(line);
            dtc_and_status_list.push_back({dtc_hex,status});
        }
    }
    MyReadFile.close();
    /* Send a frame/frames with all dtcs founded */
    if (dtc_and_status_list.size() > 1)
    {
        this->generate->readDtcInformationResponse02Long(id,status_availability_mask,dtc_and_status_list,true);
        if (receive_flow_control())
        {
            this->generate->readDtcInformationResponse02Long(id,status_availability_mask,dtc_and_status_list,false);
        }
        else
        {
            std::cout<<"Timeout. FLow control frame not received!\n";
        }
    }
    else 
    {
        this->generate->readDtcInformationResponse02(id,status_availability_mask,dtc_and_status_list);
    }
    
}

int ReadDTC::dtc_to_hex(std::string dtc)
{
    std::map<char,uint8_t> first_byte =
    {
        {'P',00}, {'C',01}, {'B',2}, {'u',3}
    };
    int hex = (first_byte[dtc[0]]<<6) | (to_int(dtc [1])<<4) | to_int(dtc[2]);
    hex = hex * 0x100 + to_int(dtc[3]) * 0x10 + to_int(dtc[4]);
    hex = hex * 0x100 + to_int(dtc[6]) * 0x10 + to_int(dtc[7]);
    return hex;
}

bool ReadDTC::receive_flow_control()
{
    /* Define a pollfd structure to monitor the socket */ 
    struct pollfd pfd;
    /* Set the socket file descriptor */ 
    pfd.fd = this->socket_; 
    /* We are interested in read events -use POLLING */ 
    pfd.events = POLLIN;

    auto start = std::chrono::system_clock::now();
    auto end = std::chrono::system_clock::now();
    while (true)
    {
        /* Use poll to wait for data to be available with a timeout of 1000ms (1 second) */ 
        int poll_result = poll(&pfd, 1, 1000);

        if (poll_result > 0 && pfd.revents & POLLIN) 
        {
            struct can_frame frame ;
            int nbytes = read(this->socket_, &frame, sizeof(frame));

            if (nbytes > 0) 
            {
                if (frame.data[0] == 0x30)
                {
                        return true;
                }    
            }
        }
        end = std::chrono::system_clock::now();
        if((std::chrono::duration_cast<std::chrono::seconds>(end - start).count() > 6)) 
            break;
    }
    return false;
}

int ReadDTC::to_int(char c)
{
    return (c >= 'A') ? (c - 'A' + 10) : (c - '0');
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