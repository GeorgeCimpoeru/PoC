#define BOOST_TEST_MODULE  InterfaceConfig_Test
#include <boost/test/unit_test.hpp> 
#include <boost/test/included/unit_test.hpp>
#include "../include/InterfaceConfig.h"
#include <boost/test/tools/output_test_stream.hpp>
#include <iostream>

BOOST_AUTO_TEST_SUITE(InterfaceConfig_Test)

    const std::string globalInterfaceName = "vcan1";
    auto globalInterface = SocketCanInterface(globalInterfaceName);

    struct cout_redirect {
        cout_redirect( std::streambuf * new_buffer ) 
            : old( std::cout.rdbuf( new_buffer ) )
        { }

        ~cout_redirect( ) {
            std::cout.rdbuf( old );
        }

    private:
        std::streambuf * old;
    };

BOOST_AUTO_TEST_CASE(Constructor_ValidInterfaceName)
{
    const std::string interfaceName = "vcan2";

    auto batteryInterface = SocketCanInterface(interfaceName);
    BOOST_CHECK_MESSAGE(batteryInterface.getInterfaceName() == interfaceName, 
        "EXPECTED INTERFACE NAME: " << interfaceName << " BUT GOT: " << batteryInterface.getInterfaceName());
}

BOOST_AUTO_TEST_CASE(GetInterfaceName)
{
    BOOST_CHECK_MESSAGE(globalInterface.getInterfaceName() == globalInterfaceName,
        "EXPECTED INTERFACE NAME: " << globalInterfaceName << " BUT GOT: " << globalInterface.getInterfaceName());
}

BOOST_AUTO_TEST_CASE(SetInterfaceName)
{
    std::string expectedInterfaceName = "vcan2";
    std::string previousInterfaceName = globalInterface.getInterfaceName();

    globalInterface.setInterfaceName(expectedInterfaceName);

    BOOST_CHECK_MESSAGE(globalInterface.getInterfaceName() == expectedInterfaceName,
        "PREVIOUS INTERFACE NAME " << previousInterfaceName << " NEW INTERFACE SHOULD BE: " << expectedInterfaceName 
        << " BUT GOT "<< globalInterface.getInterfaceName());
}

BOOST_AUTO_TEST_CASE(system_call)
{
    std::string cmd = "wrong command";
    std::string expectedOutput = cmd + " failed\n";

    boost::test_tools::output_test_stream output;
    {
        cout_redirect guard( output.rdbuf( ) );

        globalInterface.callSystem(cmd);
    }
    BOOST_CHECK_MESSAGE(output.is_equal(expectedOutput), "EXPECTED " << expectedOutput << "BUT GOT " << output.str());

    cmd = "pwd";
    expectedOutput = cmd + " succesfull\n";
    {
        cout_redirect guard( output.rdbuf( ) );
        globalInterface.callSystem(cmd);
    }
    BOOST_CHECK_MESSAGE(output.is_equal(expectedOutput), "EXPECTED " << expectedOutput << "BUT GOT " << output.str());
}

BOOST_AUTO_TEST_CASE(init_interface)
{

}

BOOST_AUTO_TEST_CASE(get_socketFd)
{
    // auto interface = SocketCanInterface("vcan1");
    // int defaultFd = -1;

    // BOOST_CHECK_MESSAGE(interface.getSocketFd() == defaultFd, "EXPECTED BEFORE INIT FD " << defaultFd << " BUT GOT " << interface.getSocketFd());
    
    // interface.init();
    // BOOST_CHECK_MESSAGE(interface.getSocketFd() != defaultFd, "EXPECTED AFTER INIT FD > 0 BUT GOT " << interface.getSocketFd());
}

BOOST_AUTO_TEST_CASE(destructor_test)
{   
    std::string interfaceName = "vcan0";
    boost::test_tools::output_test_stream output;
    {
        cout_redirect guard( output.rdbuf( ) );
        auto interface = SocketCanInterface(interfaceName);
    }
    std::string expectedOutput = "Can't close socket with fd = -1";
    BOOST_CHECK_MESSAGE(output.is_equal(expectedOutput), "EXPECTED " << expectedOutput << " BUT GOT " << output.str());

}
BOOST_AUTO_TEST_SUITE_END()