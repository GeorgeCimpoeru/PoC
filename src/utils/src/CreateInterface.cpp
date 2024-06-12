#include"../include/CreateInterface.h"

/* Constructor which initializes the interface indicator number */
CreateInterface::CreateInterface(uint8_t interface_name)
{
    this->interface_name = interface_name;  
    create_interface();
    start_interface();
}
/* Method that sets the socket to not block the reading operation */
int CreateInterface::setSocketBlocking()
{
    int flags = fcntl(_socketECU, F_GETFL, 0);
    if (flags == -1) {
        std::cerr << "Error for obtaining flags on socket: " << strerror(errno) << std::endl;
        return 1;
    }
    // Set the O_NONBLOCK flag to make the socket non-blocking
    flags |= O_NONBLOCK;
    if (fcntl(_socketECU, F_SETFL, flags) == -1) {
        std::cerr << "Error setting flags: " << strerror(errno) << std::endl;
        return -1;
    }
}
/* Method to create a vcan interface */
bool CreateInterface::create_interface()
{
    /** define the mask for the first 4 bits for the first interface in hexadecimal 
     * get the first 4 bits applying the mask using the bitwise AND operator and shift right
    */   
    unsigned char first_four_bits = ( this->interface_name & 0xF0) >> 4;
    /** define the mask for the last 4 bits for the second interface in hexadecimal 
     * get the last 4 bits applying the mask using the bitwise AND operator
    */ 
    unsigned char last_four_bits =  this->interface_name & 0X0F;
   
    /* Create interface command for the first interface (for ECU communication) */
    std::string cmd_ecu = "sudo ip link add vcan" + std::to_string(first_four_bits) + " type vcan";   
    /* Create interface command for the second interface (for API communication) */
    std::string cmd_api = "sudo ip link add vcan" + std::to_string(last_four_bits) + " type vcan";
    
    if (system(cmd_ecu.c_str())) {
        std::cout << "Error when trying to create the first interface\n";
        return false;
    }

    if (system(cmd_api.c_str())) {
        std::cout << "Error when trying to create the second interface\n";
        return false;
    }

    return true;
}

/* Method to start a vcan interface */
bool CreateInterface::start_interface()
{
    /** define the mask for the first 4 bits for the first interface in hexadecimal 
     * get the first 4 bits applying the mask using the bitwise AND operator and shift right
    */   
    unsigned char first_four_bits = (this->interface_name & 0xF0) >> 4;
    /** define the mask for the last 4 bits for the second interface in hexadecimal 
     * get the last 4 bits applying the mask using the bitwise AND operator
    */ 
    unsigned char last_four_bits = this->interface_name & 0X0F;
    
    /* start interface command for the first interface (for ECU communication)*/
    std::string cmd_ecu = "sudo ip link set vcan" + std::to_string(first_four_bits) + " up";
    /* start interface command for the second interface (for API communication)*/
    std::string cmd_api = "sudo ip link set vcan" + std::to_string(last_four_bits) + " up";   
    
    if (system(cmd_ecu.c_str())) {
        std::cout << "Error when trying to start the first interface\n";
        return false;
    }

     if (system(cmd_api.c_str())) {
        std::cout << "Error when trying to start the second interface\n";
        return false;
    }

    /* Create socket for the first interface */
    _socketECU = socket(PF_CAN, SOCK_RAW, CAN_RAW);
    if (_socketECU < 0) {
        std::cout<<"Error when trying to create the first socket\n";
        return 1;
    }    

    /* Binding socket */  
    std::string vcan_interface_ecu =  "vcan" + std::to_string(first_four_bits);    
    strcpy(ifr.ifr_name, vcan_interface_ecu.c_str() );    
    ioctl(_socketECU, SIOCGIFINDEX, &ifr);   

    addr.can_family = AF_CAN;
    addr.can_ifindex = ifr.ifr_ifindex;

    int bndECU = bind(_socketECU, (struct sockaddr*)&addr, sizeof(addr));

    if(bndECU < 0)
    {
        std::cout<<"Error when trying to bindECU\n";
        return 1;
    }    

    /* Create socket for the second interface (API) */
    _socketAPI = socket(PF_CAN, SOCK_RAW, CAN_RAW);
    if (_socketAPI < 0) {
        std::cout<<"Error when trying to create the second socket\n";
        return 1;
    }
    /* Binding socket */      
    std::string vcan_interface_api =  "vcan" + std::to_string(last_four_bits);
    strcpy(ifr.ifr_name, vcan_interface_api.c_str() );
    ioctl(_socketAPI, SIOCGIFINDEX, &ifr);

    addr.can_family = AF_CAN;
    addr.can_ifindex = ifr.ifr_ifindex;

    
    int bndAPI = bind(_socketAPI, (struct sockaddr*)&addr, sizeof(addr));

    if(bndAPI < 0)
    {
        std::cout<<"Error when trying to bindAPI\n";
        return 1;
    }    
    return true;
}

/* Method to stop a vcan interface */
bool CreateInterface::stop_interface() 
{
    /** define the mask for the first 4 bits for the first interface in hexadecimal 
     * get the first 4 bits applying the mask using the bitwise AND operator and shift right
    */   
    unsigned char first_four_bits = (this->interface_name & 0xF0) >> 4;
    /** define the mask for the last 4 bits for the second interface in hexadecimal 
     * get the last 4 bits applying the mask using the bitwise AND operator
    */ 
    unsigned char last_four_bits = this->interface_name & 0X0F;

    /* stop interface command for the first interface */
    std::string cmd_ecu = "sudo ip link set vcan" + std::to_string(first_four_bits) + " down";
    /* stop interface command for the second interface */
    std::string cmd_api = "sudo ip link set vcan" + std::to_string(last_four_bits) + " down";   

    if (system(cmd_ecu.c_str())) {
        std::cout << "Error when trying to turn down the first interface\n";
        return false;
    }

    if (system(cmd_api.c_str())) {
        std::cout << "Error when trying to turn down the second interface\n";
        return false;
    }

    return true;
}

/* Method to delete a vcan interface */
bool CreateInterface::delete_interface() 
{
    /** define the mask for the first 4 bits for the first interface in hexadecimal 
     * get the first 4 bits applying the mask using the bitwise AND operator and shift right
    */   
    unsigned char first_four_bits = (this->interface_name & 0xF0) >> 4;
    /** define the mask for the last 4 bits for the second interface in hexadecimal 
     * get the last 4 bits applying the mask using the bitwise AND operator
    */ 
    unsigned char last_four_bits = this->interface_name & 0X0F;

    /* delete interface command for the first interface */
    std::string cmd_ecu = "sudo ip link delete vcan" + std::to_string(first_four_bits) + " type can";
    /* delete interface command for the second interface */
    std::string cmd_api = "sudo ip link delete vcan" + std::to_string(last_four_bits) + " type can";

    if (system(cmd_ecu.c_str())) {
        std::cout << "Error when trying to delete the first interface\n";
        return false;
    }

    if (system(cmd_api.c_str())) {
        std::cout << "Error when trying to delete the second interface\n";
        return false;
    }

    return true;
}

/* Method to get the first socket descriptor */
int CreateInterface::get_socketECU()
{
    return _socketECU;
}

/* Method to get the second socket descriptor */
int CreateInterface::get_socketAPI()
{
    return _socketAPI;
}
