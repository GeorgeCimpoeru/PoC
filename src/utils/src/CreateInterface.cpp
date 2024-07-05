#include"../include/CreateInterface.h"

/* Initialize static instance to nullptr */
CreateInterface* CreateInterface::create_interface_instance = nullptr;

/* Private constructor */
CreateInterface::CreateInterface(uint8_t interface_name, Logger& logger)
    : logger(logger), interface_name(interface_name),
      socket_ecu_read(-1), socket_api_read(-1),
      socket_ecu_write(-1), socket_api_write(-1) 
{
    createInterface();
    startInterface();
}
/* Static method to get the singleton instance */
CreateInterface* CreateInterface::getInstance(uint8_t interface_name, Logger& logger) {
    if (create_interface_instance == nullptr) {
        create_interface_instance = new CreateInterface(interface_name, logger);
    }
    return create_interface_instance;
}

/* Return interface name (used for ECU Reset)*/
uint8_t CreateInterface::getInterfaceName()
{
    return this->interface_name;
}

/* Method that sets the socket to not block the reading operation */
int CreateInterface::setSocketBlocking()
{
    int flags = fcntl(socket_ecu_read, F_GETFL, 0);
    if (flags == -1) {
        LOG_ERROR(logger.GET_LOGGER(), "Error for obtaining flags on socket: {}", strerror(errno));
        return 1;
    }
    // Set the O_NONBLOCK flag to make the socket non-blocking
    flags |= O_NONBLOCK;
    if (fcntl(socket_ecu_read, F_SETFL, flags) == -1) {
        LOG_ERROR(logger.GET_LOGGER(), "Error setting flags: {}", strerror(errno));
        return -1;
    }
}
/* Method to create a vcan interface */
bool CreateInterface::createInterface()
{
    /** define the mask for the first 4 bits for the first interface in hexadecimal 
     * get the first 4 bits applying the mask using the bitwise AND operator and shift right
    */   
    unsigned char first_four_bits = ( this->interface_name & 0xF0) >> 4;
    /** define the mask for the last 4 bits for the second interface in hexadecimal 
     * get the last 4 bits applying the mask using the bitwise AND operator
    */ 
    unsigned char last_four_bits =  this->interface_name & 0X0F;

    /* flag to track the succes or failure of each command */
    bool command_check = true;

    /* Create interface command for the first interface (for ECU communication) */
    std::string cmd_ecu = "sudo ip link add vcan" + std::to_string(first_four_bits) + " type vcan";   
    /* Create interface command for the second interface (for API communication) */
    std::string cmd_api = "sudo ip link add vcan" + std::to_string(last_four_bits) + " type vcan";
    
    if (system(cmd_ecu.c_str())) {        
        LOG_ERROR(logger.GET_LOGGER(),"Error when trying to create the first interface");
        /* Set the flag to false if the first command fails */
        command_check  = false;
    }

    if (system(cmd_api.c_str())) {        
        LOG_ERROR(logger.GET_LOGGER(),"Error when trying to create the second interface");
        /* Set the flag to false if the second command fails */
        command_check = false;
    }

    return command_check;
}

/* Method to start a vcan interface */
bool CreateInterface::startInterface()
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
    
    /* flag to track the succes or failure of each command */
    bool command_check = true; 

    if (system(cmd_ecu.c_str())) {        
        LOG_ERROR(logger.GET_LOGGER(),"Error when trying to start the first interface");
        /* Set the flag to false if the first command fails */
        command_check  = false;
    }

     if (system(cmd_api.c_str())) {        
        LOG_ERROR(logger.GET_LOGGER(),"Error when trying to start the second interface");
        /* Set the flag to false if the second command fails */
        command_check  = false;
    }

    /* Create the read socket for the first interface */
    socket_ecu_read = socket(PF_CAN, SOCK_RAW, CAN_RAW);
    if (socket_ecu_read < 0) 
    {
        LOG_ERROR(logger.GET_LOGGER(),"Error when trying to create the ECU read socket");
        return 1;
    }    

    /* Binding read socket */  
    std::string vcan_interface_ecu =  "vcan" + std::to_string(first_four_bits);    
    strcpy(ifr.ifr_name, vcan_interface_ecu.c_str() );    
    ioctl(socket_ecu_read, SIOCGIFINDEX, &ifr);   

    addr.can_family = AF_CAN;
    addr.can_ifindex = ifr.ifr_ifindex;

    int bind_ecu_read = bind(socket_ecu_read, (struct sockaddr*)&addr, sizeof(addr));

    if(bind_ecu_read < 0)
    {
        LOG_ERROR(logger.GET_LOGGER(),"Error when trying to bind ECU read socket");
        return 1;
    }    

    /* Create the read socket for the second interface (API) */
    socket_api_read = socket(PF_CAN, SOCK_RAW, CAN_RAW);
    if (socket_api_read < 0) 
    {       
        LOG_ERROR(logger.GET_LOGGER(),"Error when trying to create the API read socket");
        return 1;
    }
    /* Binding read socket */      
    std::string vcan_interface_api =  "vcan" + std::to_string(last_four_bits);
    strcpy(ifr.ifr_name, vcan_interface_api.c_str() );
    ioctl(socket_api_read, SIOCGIFINDEX, &ifr);

    addr.can_family = AF_CAN;
    addr.can_ifindex = ifr.ifr_ifindex;

    int bind_api_read = bind(socket_api_read, (struct sockaddr*)&addr, sizeof(addr));

    if(bind_api_read < 0)
    {
        LOG_ERROR(logger.GET_LOGGER(),"Error when trying to API read socket");
        return 1;
    }
    /* Create the write socket for the first interface */
    socket_ecu_write = socket(PF_CAN, SOCK_RAW, CAN_RAW);
    if (socket_ecu_write < 0) 
    {
        LOG_ERROR(logger.GET_LOGGER(),"Error when trying to create the ECU write socket");
        return 1;
    }    

    /* Binding write socket */     
    strcpy(ifr.ifr_name, vcan_interface_ecu.c_str() );    
    ioctl(socket_ecu_write, SIOCGIFINDEX, &ifr);   

    addr.can_family = AF_CAN;
    addr.can_ifindex = ifr.ifr_ifindex;

    int bind_ecu_write = bind(socket_ecu_write, (struct sockaddr*)&addr, sizeof(addr));

    if(bind_ecu_write < 0)
    {
        LOG_ERROR(logger.GET_LOGGER(),"Error when trying to bind ECU write socket");
        return 1;
    }    

    /* Create the write socket for the second interface (API) */
    socket_api_write = socket(PF_CAN, SOCK_RAW, CAN_RAW);
    if (socket_api_read < 0) 
    {       
        LOG_ERROR(logger.GET_LOGGER(),"Error when trying to create the API write socket");
        return 1;
    }
    /* Binding write socket */      
    strcpy(ifr.ifr_name, vcan_interface_api.c_str() );
    ioctl(socket_api_write, SIOCGIFINDEX, &ifr);

    addr.can_family = AF_CAN;
    addr.can_ifindex = ifr.ifr_ifindex;

    int bind_api_write = bind(socket_api_write, (struct sockaddr*)&addr, sizeof(addr));

    if(bind_api_write < 0)
    {
        LOG_ERROR(logger.GET_LOGGER(),"Error when trying to bind API write socket");
        return 1;
    }      
    return command_check;
}

/* Method to stop a vcan interface */
bool CreateInterface::stopInterface() 
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

    /* flag to track the succes or failure of each command */
    bool command_check = true;

    if (system(cmd_ecu.c_str())) 
    {        
        LOG_ERROR(logger.GET_LOGGER(),"Error when trying to stop the first interface");
        /* Set the flag to false if the first command fails */
        command_check  = false;
    }

    if (system(cmd_api.c_str())) 
    {        
        LOG_ERROR(logger.GET_LOGGER(),"Error when trying to stop the second interface");
        /* Set the flag to false if the second command fails */
        command_check  = false;
    }

    return command_check;
}

/* Method to delete a vcan interface */
bool CreateInterface::deleteInterface() 
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

    /* flag to track the succes or failure of each command */
    bool command_check = true;

    if (system(cmd_ecu.c_str())) 
    {        
        LOG_ERROR(logger.GET_LOGGER(),"Error when trying to delete the first interface");
        /* Set the flag to false if the first command fails */
        command_check  = false;
    }

    if (system(cmd_api.c_str())) 
    {        
        LOG_ERROR(logger.GET_LOGGER(),"Error when trying to delete the second interface");
        /* Set the flag to false if the second command fails */
        command_check  = false;
    }

    return command_check;
}

/* Method to get the ecu read socket descriptor */
int CreateInterface::getSocketEcuRead()
{
    return socket_ecu_read;
}

/* Method to get the api read socket descriptor */
int CreateInterface::getSocketApiRead()
{
    return socket_api_read;
}

/* Method to get the ecu write socket descriptor */
int CreateInterface::getSocketEcuWrite()
{
    return socket_ecu_write;
}

/* Method to get the api write socket descriptor */
int CreateInterface::getSocketApiWrite()
{
    return socket_api_write;
}