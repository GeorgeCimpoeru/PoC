//#include <../../../ecu_simulation/BatteryModule/include/GenerateFrames.h>

#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <cstring>

class ReadDTC
{
    private:
        std::string path_folder;
    public:
        ReadDTC(std::string path_folder);
        void run(int sub_function, int dtc_status_mask);
};
ReadDTC::ReadDTC(std::string path_folder)
{
    this->path_folder = path_folder;
}
void ReadDTC::run(int sub_function, int dtc_status_mask)
{
    switch (sub_function)
    {
        case 0x01:
            std::cout<<"Sub-function 1\n";
            break;
        case 0x02:
            std::cout<<"Sub-function 1\n";
            break;
    }
}

int main(int argc, char** argv)
{
    if (argc > 2)
    {
        ReadDTC obj("/bin/dtc.txt");
        int sub_function;
        sscanf(argv[1], "%d", &sub_function);
        int dtc_status_mask;
        sscanf(argv[2], "%d", &dtc_status_mask); 
        obj.run(sub_function, dtc_status_mask);
        std::cout<<sub_function<<"    "<<dtc_status_mask<<"\n";
    }
    else
    {
        std::cout<<"Please provide the sub-function and the DTC status mask \n";
    }
}