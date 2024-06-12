#include"../include/InterfaceModule.h"

/* Constructor which initializes the interface indicator number */
INTERFACE_module::INTERFACE_module(uint8_t interface_name)
{
    this->interface_name = interface_name;  
    create_interface();
    start_interface();
}

/* Method to create a vcan interface */
int INTERFACE_module::create_interface()
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
    std::string cmd1 = "sudo ip link add vcan" + std::to_string(first_four_bits) + " type vcan";   
    /* Create interface command for the second interface (for API communication) */
    std::string cmd2 = "sudo ip link add vcan" + std::to_string(last_four_bits) + " type vcan";
    
    if (system(cmd1.c_str())) {
        std::cout << "Error when trying to create the first interface\n";
        return 1;
    }

    if (system(cmd2.c_str())) {
        std::cout << "Error when trying to create the second interface\n";
        return 1;
    }

    return 0;
}

/* Method to start a vcan interface */
int INTERFACE_module::start_interface()
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
    std::string cmd1 = "sudo ip link set vcan" + std::to_string(first_four_bits) + " up";
    /* start interface command for the second interface (for API communication)*/
    std::string cmd2 = "sudo ip link set vcan" + std::to_string(last_four_bits) + " up";   
    
    if (system(cmd1.c_str())) {
        std::cout << "Error when trying to start the first interface\n";
        return 1;
    }

     if (system(cmd2.c_str())) {
        std::cout << "Error when trying to start the second interface\n";
        return 1;
    }

    /* Create socket for the first interface */
    _socketECU = socket(PF_CAN, SOCK_RAW, CAN_RAW);
    if (_socketECU < 0) {
        std::cout<<"Error when trying to create the first socket\n";
        return 1;
    }    

    /* Binding socket */  
    std::string vcanInterface1 =  "vcan" + std::to_string(first_four_bits);    
    strcpy(ifr.ifr_name, vcanInterface1.c_str() );    
    ioctl(_socketECU, SIOCGIFINDEX, &ifr);   

    addr.can_family = AF_CAN;
    addr.can_ifindex = ifr.ifr_ifindex;

    int bnd1 = bind(_socketECU, (struct sockaddr*)&addr, sizeof(addr));

    if(bnd1 < 0)
    {
        std::cout<<"Error when trying to bind1\n";
        return 1;
    }    

    /* Create socket for the second interface (API) */
    _socketAPI = socket(PF_CAN, SOCK_RAW, CAN_RAW);
    if (_socketAPI < 0) {
        std::cout<<"Error when trying to create the second socket\n";
        return 1;
    }

    /* Binding socket */      
    std::string vcanInterface2 =  "vcan" + std::to_string(last_four_bits);
    strcpy(ifr.ifr_name, vcanInterface2.c_str() );
    ioctl(_socketAPI, SIOCGIFINDEX, &ifr);

    addr.can_family = AF_CAN;
    addr.can_ifindex = ifr.ifr_ifindex;

    
    int bnd2 = bind(_socketAPI, (struct sockaddr*)&addr, sizeof(addr));

    if(bnd2 < 0)
    {
        std::cout<<"Error when trying to bind2\n";
        return 1;
    }    

    return 0;
}

/* Method to stop a vcan interface */
int INTERFACE_module::stop_interface() 
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
    std::string cmd1 = "sudo ip link set vcan" + std::to_string(first_four_bits) + " down";
    /* stop interface command for the second interface */
    std::string cmd2 = "sudo ip link set vcan" + std::to_string(last_four_bits) + " down";   

    if (system(cmd1.c_str())) {
        std::cout << "Error when trying to turn down the first interface\n";
        return 1;
    }

    if (system(cmd2.c_str())) {
        std::cout << "Error when trying to turn down the second interface\n";
        return 1;
    }

    return 0;
}

/* Method to delete a vcan interface */
int INTERFACE_module::delete_interface() 
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
    std::string cmd1 = "sudo ip link delete vcan" + std::to_string(first_four_bits) + " type can";
    /* delete interface command for the second interface */
    std::string cmd2 = "sudo ip link delete vcan" + std::to_string(last_four_bits) + " type can";

    if (system(cmd1.c_str())) {
        std::cout << "Error when trying to delete the first interface\n";
        return 1;
    }

    if (system(cmd2.c_str())) {
        std::cout << "Error when trying to delete the second interface\n";
        return 1;
    }

    return 0;
}

/* Method to get the first socket descriptor */
int INTERFACE_module::get_socketECU()
{
    return _socketECU;
}

/* Method to get the second socket descriptor */
int INTERFACE_module::get_socketAPI()
{
    return _socketAPI;
}
