#include <gtest/gtest.h>
#include "../include/InterfaceConfig.h"
#include <iostream>

class InterfaceConfigTest : public ::testing::Test
{
protected:
    const std::string globalInterfaceName = "vcan1";
    SocketCanInterface globalInterface = SocketCanInterface(globalInterfaceName);

    struct CoutRedirect {
        CoutRedirect(std::streambuf* new_buffer)
            : old(std::cout.rdbuf(new_buffer)) { }

        ~CoutRedirect() {
            std::cout.rdbuf(old);
        }

    private:
        std::streambuf* old;
    };
};

TEST_F(InterfaceConfigTest, Constructor_ValidInterfaceName) 
{
    const std::string interfaceName = "vcan2";
    SocketCanInterface batteryInterface(interfaceName);
    EXPECT_EQ(batteryInterface.getInterfaceName(), interfaceName);
}

TEST_F(InterfaceConfigTest, GetInterfaceName) 
{
    EXPECT_EQ(globalInterface.getInterfaceName(), globalInterfaceName);
}

TEST_F(InterfaceConfigTest, SetInterfaceName) 
{
    std::string expectedInterfaceName = "vcan2";
    std::string previousInterfaceName = globalInterface.getInterfaceName();

    globalInterface.setInterfaceName(expectedInterfaceName);

    EXPECT_EQ(globalInterface.getInterfaceName(), expectedInterfaceName);
}

TEST_F(InterfaceConfigTest, SystemCall) 
{
    std::string cmd = "wrong command";
    std::string expectedOutput = cmd + " failed\n";

    std::ostringstream output;
    {
        CoutRedirect guard(output.rdbuf());
        globalInterface.callSystem(cmd);
    }
    EXPECT_EQ(output.str(), expectedOutput);

    cmd = "pwd";
    expectedOutput = cmd + " succesfull\n";
    output.str("");
    output.clear();
    {
        CoutRedirect guard(output.rdbuf());
        globalInterface.callSystem(cmd);
    }
    EXPECT_EQ(output.str(), expectedOutput);
}

TEST_F(InterfaceConfigTest, InitInterface) 
{
    auto interface = SocketCanInterface("vcan1");

}

TEST_F(InterfaceConfigTest, GetSocketFd) 
{
    auto interface = SocketCanInterface("vcan1");
    int defaultFd = -1;

    EXPECT_THROW(interface.getSocketFd(), std::runtime_error);
    
    interface.init();
    EXPECT_NO_THROW(interface.getSocketFd());
    EXPECT_NE(interface.getSocketFd(), defaultFd);
}

TEST_F(InterfaceConfigTest, DestructorTest) 
{
    std::string interfaceName = "vcan0";
    std::ostringstream output;
    {
        CoutRedirect guard(output.rdbuf());
        auto interface = SocketCanInterface(interfaceName);
    }
    std::string expectedOutput = "Can't close socket with fd = -1";
    EXPECT_EQ(output.str(), expectedOutput);
}
