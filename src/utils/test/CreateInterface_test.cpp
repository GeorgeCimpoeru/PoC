#include <gtest/gtest.h>
#include "../include/CreateInterface.h"
#include <fstream>
#include <iostream>

/* a method designed to check if the network interface (vcan0 is this case) has been successfully created */
bool is_interface_created(const std::string& interface_name)
{
    std::ifstream file("/sys/class/net/" + interface_name + "/ifindex");
    return file.good();
}

/* a method designed to check if the socket is valid */
bool is_valid_socket(int sockfd)
{
    return sockfd >= 0;
}

/** TEST SUITE 
* test the create_interface method
*/
TEST(InterfaceTestSuite, CreateInterface)
{
    CreateInterface interface(0x01);
    EXPECT_TRUE(is_interface_created("vcan0"));
    EXPECT_TRUE(is_interface_created("vcan1"));
    interface.delete_interface(); 
    EXPECT_FALSE(is_interface_created("vcan0"));
    EXPECT_FALSE(is_interface_created("vcan1"));
    EXPECT_EQ(interface.create_interface(),true);     
    EXPECT_TRUE(is_interface_created("vcan0"));
    EXPECT_TRUE(is_interface_created("vcan1"));
   
    EXPECT_EQ(interface.delete_interface(),true); 
}

/* test the start_interface method */
TEST(InterfaceTestSuite, StartInterface)
{
    CreateInterface interface(0x01);
    EXPECT_TRUE(is_interface_created("vcan0"));
    EXPECT_TRUE(is_interface_created("vcan1"));    
    EXPECT_EQ(interface.start_interface(),true); 
    /* clean up any existing interface to ensure a clean test environment for the next test */ 
    interface.delete_interface();
}

/* test the stop_interface method */
TEST(InterfaceTestSuite, StopInterface)
{
    CreateInterface interface(0x01);
    EXPECT_TRUE(is_interface_created("vcan0"));
    EXPECT_TRUE(is_interface_created("vcan1"));    
    EXPECT_EQ(interface.start_interface(),true); 
    EXPECT_EQ(interface.stop_interface(),true);
    /* clean up any existing interface to ensure a clean test environment for the next test */ 
    interface.delete_interface();
}

/* test the delete_interface method */
TEST(InterfaceTestSuite, DeleteInterface)
{
    CreateInterface interface(0x01);
    EXPECT_TRUE(is_interface_created("vcan0"));
    EXPECT_TRUE(is_interface_created("vcan1"));    
    EXPECT_EQ(interface.start_interface(),true); 
    EXPECT_EQ(interface.stop_interface(),true);
    EXPECT_EQ(interface.delete_interface(),true);  
}

/* test the delete_interface method */
TEST(InterfaceTestSuite, ErrorInterface)
{
    CreateInterface interface(0x01);
    EXPECT_TRUE(is_interface_created("vcan0"));
    EXPECT_TRUE(is_interface_created("vcan1"));   
    EXPECT_EQ(interface.delete_interface(),true); 
    EXPECT_EQ(interface.start_interface(),false); 
    EXPECT_EQ(interface.stop_interface(),false);
    EXPECT_EQ(interface.delete_interface(),false);
}

/* test if the interface was not created */
TEST(InterfaceTestSuite, InterfaceNotCreatedTest)
{
    CreateInterface interface(0x01);
    interface.delete_interface(); 
    EXPECT_EQ(interface.start_interface(),false);     
}
   
/* test the get_socketECU method */
TEST(InterfaceTestSuite, GetSocketECU)
{    
    CreateInterface interface(0X01);   
    EXPECT_TRUE(is_valid_socket(interface.get_socketECU()));  
    /* clean up any existing interface to ensure a clean test environment for the next test */ 
    interface.delete_interface();
}

/* test the get_socketAPI method */
TEST(InterfaceTestSuite, GetSocketAPI)
{    
    CreateInterface interface(0X01);   
    EXPECT_TRUE(is_valid_socket(interface.get_socketAPI()));  
    /* clean up any existing interface to ensure a clean test environment for the next test */ 
    interface.delete_interface();
}

int main(int argc, char* argv[])
{
    testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}



