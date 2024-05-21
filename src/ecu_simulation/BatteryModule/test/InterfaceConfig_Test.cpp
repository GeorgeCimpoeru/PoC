#define BOOST_TEST_MODULE  InterfaceConfig_Test
#include <boost/test/unit_test.hpp> 
#include <boost/test/included/unit_test.hpp>
#include "../include/InterfaceConfig.h"

BOOST_AUTO_TEST_SUITE(InterfaceConfig_Test)

    const std::string globalInterfaceName = "vcan1";
    auto globalInterface = SocketCanInterface(globalInterfaceName);

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

BOOST_AUTO_TEST_CASE(create_vcan)
{
    
}

BOOST_AUTO_TEST_CASE(open_socket)
{

}

BOOST_AUTO_TEST_CASE(close_socket)
{

}

BOOST_AUTO_TEST_CASE(destructor_test)
{

}
BOOST_AUTO_TEST_SUITE_END()