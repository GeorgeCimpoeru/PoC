#include"../include/InterfaceModule.h"

INTERFACE_module::INTERFACE_module(std::string interface_name)
{
    this->interface_name = interface_name;  
    create_interface();
    start_interface();
}

int INTERFACE_module::create_interface()
{
    // Create interface
    std::string cmd = "sudo ip link add type vcan name " + this->interface_name;

    if (system(cmd.c_str())) {
        std::cout << "Error when trying to create the interface\n";
        return 1;
    }

    return 0;
}

int INTERFACE_module::start_interface()
{
    // Start interface
    std::string cmd = "sudo ip link set " + this->interface_name + " up";

    if (system(cmd.c_str())) {
        std::cout << "Error when trying to start the interface\n";
        return 1;
    }

    // Create socket
    _socket = socket(PF_CAN, SOCK_RAW, CAN_RAW);
    if (_socket < 0) {
        std::cout<<"Error when trying to create the socket\n";
        return 1;
    }

    // Binding socket
    strcpy(ifr.ifr_name, this->interface_name.c_str() );
    ioctl(_socket, SIOCGIFINDEX, &ifr);

    addr.can_family = AF_CAN;
    addr.can_ifindex = ifr.ifr_ifindex;

    int bnd = bind(_socket, (struct sockaddr*)&addr, sizeof(addr));
    if(bnd < 0)
    {
        std::cout<<"Error when trying to bind\n";
        return 1;
    }

    return 0;
}

int INTERFACE_module::stop_interface() 
{
    // Turn down interface
    std::string cmd = "sudo ip link set " + this->interface_name + " down";

    if (system(cmd.c_str())) {
        std::cout << "Error when trying to turn down the interface\n";
        return 1;
    }

    return 0;
}

int INTERFACE_module::delete_interface() 
{
    // Delete interface
    std::string cmd = "sudo ip link delete " + this->interface_name + " type can";

    if (system(cmd.c_str())) {
        std::cout << "Error when trying to delete the interface\n";
        return 1;
    }

    return 0;
}

int INTERFACE_module::get_socket()
{
    return _socket;
}

std::string INTERFACE_module::get_interface_name()
{
    return interface_name;
}