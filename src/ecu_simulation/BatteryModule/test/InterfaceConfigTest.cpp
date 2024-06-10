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

TEST_F(InterfaceConfigTest, ConstructorInterfaceNameSet) 
{
    const std::string interfaceName = "vcan2";
    SocketCanInterface batteryInterface(interfaceName);
    EXPECT_EQ(batteryInterface.getInterfaceName(), interfaceName);
}
TEST_F(InterfaceConfigTest, ConstructorCreateInterface)
{
    
    std::ostringstream output;
    {
        CoutRedirect guard(output.rdbuf());
        auto interface = SocketCanInterface("vcan3");
    }
    EXPECT_EQ(output.str(), "vcan3 initialised\n");
}

TEST_F(InterfaceConfigTest, GetInterfaceName) 
{
    EXPECT_EQ(globalInterface.getInterfaceName(), globalInterfaceName);
}

TEST_F(InterfaceConfigTest, SystemCall) 
{
    std::string cmd = "wrong command";
    std::string cmdFailedOutput = cmd + " failed\n";

    std::ostringstream output;
    {
        CoutRedirect guard(output.rdbuf());
        globalInterface.callSystem(cmd);
    }
    EXPECT_EQ(output.str(), cmdFailedOutput);

    cmd = "pwd";
    output.str("");
    output.clear();
    {
        CoutRedirect guard(output.rdbuf());
        globalInterface.callSystem(cmd);
    }
    EXPECT_NE(output.str(), cmdFailedOutput);
}

TEST_F(InterfaceConfigTest, GetSocketFd) 
{
    auto interface = SocketCanInterface("vcan1");
    int defaultFd = -1;
    
    EXPECT_NE(interface.getSocketFd(), defaultFd);
}

TEST_F(InterfaceConfigTest, DestructorTest) 
{
    std::string interfaceName = "vcan0";
    std::ostringstream output;
    {
        CoutRedirect guard(output.rdbuf());
        auto interface = SocketCanInterface(interfaceName);
        output.clear();
    }
    std::string failOutput = "Can't close socket with fd = -1";
    EXPECT_NE(output.str(), failOutput);
}
