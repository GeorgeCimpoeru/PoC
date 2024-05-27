#include <gtest/gtest.h>
#include "../include/InterfaceModule.h"
#include <fstream>
#include <iostream>

// a method that designed to check if the network interface (vcan0 is this case) has been successfully created
bool is_interface_created(const std::string& interface_name)
{
    std::ifstream file("/sys/class/net/" + interface_name + "/ifindex");
    return file.good();
}

// TEST SUITE 1
// test the create_interface method
TEST(InterfaceTestSuite1, CreateInterface)
{
    INTERFACE_module interface("vcan0");
    ASSERT_EQ(interface.create_interface(),0);   

    ASSERT_TRUE(is_interface_created("vcan0"));
}

// test the start_interface method
TEST(InterfaceTestSuite1, StartInterface)
{
    INTERFACE_module interface("vcan0");
    ASSERT_EQ(interface.start_interface(),0);
}

// test the stop_interface method
TEST(InterfaceTestSuite1, StopInterface)
{
    INTERFACE_module interface("vcan0");
    ASSERT_EQ(interface.stop_interface(),0);
}

// test the delete_interface method
TEST(InterfaceTestSuite1, DeleteInterface)
{
    INTERFACE_module interface("vcan0");
    ASSERT_EQ(interface.delete_interface(),0);
}

// test the get_interface_name method
TEST(InterfaceTestSuite1, GetInterfaceName)
{
    INTERFACE_module interface("vcan0");   
    ASSERT_EQ(interface.get_interface_name(),"vcan0");  
   
}

// TEST SUITE 2
// check if the interface is started properly
TEST(InterfaceTestSuite2, StartInterface)
{
    INTERFACE_module interface("vcan0");
    interface.create_interface();  
    ASSERT_EQ(interface.start_interface(),0);  
}

//check if the interface stopped properly
TEST(InterfaceTestSuite2, StopInterface)
{
    INTERFACE_module interface("vcan0");
    interface.create_interface();  
    interface.start_interface();  
    ASSERT_EQ(interface.stop_interface(),0);  
}

//check if the interface has been deleted properly 
TEST(InterfaceTestSuite2, DeleteInterface)
{
    INTERFACE_module interface("vcan0");
    interface.create_interface();  
    interface.start_interface();  
    interface.stop_interface(); 
    ASSERT_EQ(interface.delete_interface(),0);  

    ASSERT_FALSE(is_interface_created("vcan0"));
}


int main(int argc, char* argv[])
{
    testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}



