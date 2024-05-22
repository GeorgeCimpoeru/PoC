#include"McuModule.h"

MCU_module::MCU_module(std::string interface_name)
{
    this->interface_name = interface_name;  
    create_interface();
}

int MCU_module::create_interface()
{
    // Create interface
    std::string cmd = "sudo ip link add type vcan name " + this->interface_name;

    if (system(cmd.c_str())) {
        std::cout << "Error when trying to create the interface\n";
        return 1;
    }

    // Start interface
    cmd = "sudo ip link set " + this->interface_name + " up";

    if (system(cmd.c_str())) {
        std::cout << "Error when trying to start the interface\n";
        return 1;
    }

    // Create socket
    int sock = socket(PF_CAN, SOCK_RAW, CAN_RAW);
    if (sock < 0) {
        std::cout<<"Error when trying to create the socket\n";
        return 1;
    }

    // Binding socket
    strcpy(ifr.ifr_name, this->interface_name.c_str() );
    ioctl(sock, SIOCGIFINDEX, &ifr);

    addr.can_family = AF_CAN;
    addr.can_ifindex = ifr.ifr_ifindex;

    int bnd = bind(sock, (struct sockaddr*)&addr, sizeof(addr));
    if(bnd < 0)
    {
        std::cout<<"Error when trying to bind\n";
        return 1;
    }

    return 0;
}

int MCU_module::delete_interface() 
{
    // Turn down interface
    std::string cmd = "sudo ip link set " + this->interface_name + " down";

    if (system(cmd.c_str())) {
        std::cout << "Error when trying to turn down the interface\n";
        return 1;
    }

    // Delete interface
    cmd = "sudo ip link delete " + this->interface_name + " type can";

    if (system(cmd.c_str())) {
        std::cout << "Error when trying to delete the interface\n";
        return 1;
    }

    return 0;
}