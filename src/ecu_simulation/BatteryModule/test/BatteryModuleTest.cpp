#include <gtest/gtest.h>
#include "../include/BatteryModule.h"



/* Helper function to create a socket for sending CAN frames */
int createSocket()
{
    /* Create socket */
    std::string nameInterface = "vcan0";
    struct sockaddr_can addr;
    struct ifreq ifr;
    int s = socket(PF_CAN, SOCK_RAW, CAN_RAW);
    if (s < 0)
    {
        std::cout << "Error trying to create the socket\n";
        return 1;
    }

    /* Giving name and index to the interface created */
    strcpy(ifr.ifr_name, nameInterface.c_str());
    ioctl(s, SIOCGIFINDEX, &ifr);

    /* Set addr structure with info */
    addr.can_family = AF_CAN;
    addr.can_ifindex = ifr.ifr_ifindex;

    /* Bind the socket to the CAN interface */
    if (bind(s, (struct sockaddr *)&addr, sizeof(addr)) < 0)
    {
        std::cout << "Error binding\n";
        return 1;
    }

    return s;
}

struct BatteryModuleTest : testing::Test
{
    BatteryModule* battery;
    BatteryModuleTest()
    {
        battery = new BatteryModule();
    }
    ~BatteryModuleTest()
    {
        delete battery;
    }
};

class BatteryModulePopenFailMock : public BatteryModule
{
public:
    /* Override the exec function to simulate popen failure */
    std::string exec(const char *cmd) override
    {
        throw std::runtime_error("popen() failed!");
    }
};

TEST_F(BatteryModuleTest, DefaultMCUStateTest)
{
    char s[20] = "kefkef";
    battery->exec(s);
}

/* Main function to run all tests */
int main(int argc, char **argv)
{
    testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}