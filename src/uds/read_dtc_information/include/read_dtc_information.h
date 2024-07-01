#ifndef READ_DTC_INFROMATION_H
#define READ_DTC_INFROMATION_H

#include "../../../utils/include/GenerateFrames.h"

#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <cstring>

#include <sys/ioctl.h>
#include <sys/socket.h>
#include <net/if.h>

#include<linux/can.h>
#include<string.h>
#include <fcntl.h>

class ReadDTC
{
    private:
        std::string path_folder;
        GenerateFrames* generate;

    public:
        ReadDTC(std::string path_folder);
        ~ReadDTC();
        void read(int id, int sub_function, int dtc_status_mask);

    private:
        void number_of_dtc(int id, int dtc_status_mask);
        void dtcs(int id, int dtc_status_mask);
        int creat_socket();
};

#endif