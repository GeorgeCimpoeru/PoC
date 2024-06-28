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
/* test logger */
Logger logger;
/** TEST SUITE 
* test the createInterface method
*/
TEST(InterfaceTestSuite, CreateInterface)
{
    CreateInterface* interface = CreateInterface::getInstance(0x01, logger);
    EXPECT_TRUE(is_interface_created("vcan0"));
    EXPECT_TRUE(is_interface_created("vcan1"));
    interface->deleteInterface(); 
    EXPECT_FALSE(is_interface_created("vcan0"));
    EXPECT_FALSE(is_interface_created("vcan1"));
    EXPECT_EQ(interface->createInterface(),true);     
    EXPECT_TRUE(is_interface_created("vcan0"));
    EXPECT_TRUE(is_interface_created("vcan1"));
   
    EXPECT_EQ(interface->deleteInterface(),true); 
}

/* test the startInterface method */
TEST(InterfaceTestSuite, CreateInterfaceError)
{
    CreateInterface* interface = CreateInterface::getInstance(0x01, logger);
    /* redirect stdout to capture the output */
    interface->createInterface();
    testing::internal::CaptureStdout();
    interface->createInterface();
    std::string output = testing::internal::GetCapturedStdout();   
    EXPECT_NE(output.find("Error when trying to create the first interface"), std::string::npos);
    EXPECT_NE(output.find("Error when trying to create the second interface"), std::string::npos);      
    
    /* clean up any existing interface to ensure a clean test environment for the next test */ 
    interface->deleteInterface();
}

/* test the startInterface method */
TEST(InterfaceTestSuite, StartInterface)
{
    CreateInterface* interface = CreateInterface::getInstance(0x01, logger);
    interface->createInterface();
    EXPECT_TRUE(is_interface_created("vcan0"));
    EXPECT_TRUE(is_interface_created("vcan1"));    
    EXPECT_EQ(interface->startInterface(),true); 
    /* clean up any existing interface to ensure a clean test environment for the next test */ 
    interface->deleteInterface();
}

/* test the stopInterface method */
TEST(InterfaceTestSuite, StopInterface)
{
    CreateInterface* interface = CreateInterface::getInstance(0x01, logger);
    interface->createInterface();
    EXPECT_TRUE(is_interface_created("vcan0"));
    EXPECT_TRUE(is_interface_created("vcan1"));    
    EXPECT_EQ(interface->startInterface(),true); 
    EXPECT_EQ(interface->stopInterface(),true);
    /* clean up any existing interface to ensure a clean test environment for the next test */ 
    interface->deleteInterface();
}

/* test the deleteInterface method */
TEST(InterfaceTestSuite, DeleteInterface)
{
    CreateInterface* interface = CreateInterface::getInstance(0x01, logger);
    interface->createInterface();
    EXPECT_TRUE(is_interface_created("vcan0"));
    EXPECT_TRUE(is_interface_created("vcan1"));    
    EXPECT_EQ(interface->startInterface(),true); 
    EXPECT_EQ(interface->stopInterface(),true);
    EXPECT_EQ(interface->deleteInterface(),true);  
}

/* test if the interface was not started */
TEST(InterfaceTestSuite, ErrorStartInterfaceTest)
{
    CreateInterface* interface = CreateInterface::getInstance(0x01, logger);
    interface->deleteInterface();  
    testing::internal::CaptureStdout();
    interface->startInterface();
    std::string output = testing::internal::GetCapturedStdout();   
    EXPECT_NE(output.find("Error when trying to start the first interface"), std::string::npos);
    EXPECT_NE(output.find("Error when trying to start the second interface"), std::string::npos);       
}

/* test if the interface was not stopped */
TEST(InterfaceTestSuite, ErrorStopInterfaceTest)
{
    CreateInterface* interface = CreateInterface::getInstance(0x01, logger);
    interface->deleteInterface();  
    testing::internal::CaptureStdout();
    interface->stopInterface();
    std::string output = testing::internal::GetCapturedStdout();   
    EXPECT_NE(output.find("Error when trying to stop the first interface"), std::string::npos);
    EXPECT_NE(output.find("Error when trying to stop the second interface"), std::string::npos);       
}

/* test if the interface was not deleted*/
TEST(InterfaceTestSuite, ErrorDeleteInterfaceTest)
{
    CreateInterface* interface = CreateInterface::getInstance(0x01, logger);
    interface->deleteInterface();  
    testing::internal::CaptureStdout();
    interface->deleteInterface();
    std::string output = testing::internal::GetCapturedStdout();   
    EXPECT_NE(output.find("Error when trying to delete the first interface"), std::string::npos);
    EXPECT_NE(output.find("Error when trying to delete the second interface"), std::string::npos);       
}
   
/* test the getSocketEcuRead method */
TEST(InterfaceTestSuite, GetSocketECU)
{    
    CreateInterface* interface = CreateInterface::getInstance(0x01, logger);   
    EXPECT_TRUE(is_valid_socket(interface->getSocketEcuRead()));  
    /* clean up any existing interface to ensure a clean test environment for the next test */ 
    interface->deleteInterface();
}

/* test the getSocketApiRead method */
TEST(InterfaceTestSuite, GetSocketAPI)
{    
    CreateInterface* interface = CreateInterface::getInstance(0x01, logger);   
    EXPECT_TRUE(is_valid_socket(interface->getSocketApiRead()));  
    /* clean up any existing interface to ensure a clean test environment for the next test */ 
    interface->deleteInterface();
}

/* test the setSocketBlocking method */
TEST(InterfaceTestSuite, setSocketBlocking)
{    
    CreateInterface* interface = CreateInterface::getInstance(0x01, logger);   
    EXPECT_EQ(interface->setSocketBlocking(), 0 ); 
    /* clean up any existing interface to ensure a clean test environment for the next test */ 
    interface->deleteInterface(); 
}

/* test the setSocketBlocking method */
TEST(InterfaceTestSuite, setSocketBlockingError)
{    
    CreateInterface* interface = CreateInterface::getInstance(0x01, logger);       
    /* Close the file descriptor associated to the socket to simulate error */
    close(interface->getSocketEcuRead());
    EXPECT_EQ(interface->setSocketBlocking(), 1 ); 
    /* clean up any existing interface to ensure a clean test environment for the next test */ 
    interface->deleteInterface();
}

int main(int argc, char* argv[])
{
    testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}