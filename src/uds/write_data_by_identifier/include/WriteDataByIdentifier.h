#include <iostream>
#include <map>
#include <vector>
#include <cstdint>
#include<stdlib.h>
#include <cstring>
#include <string>
#include <sys/ioctl.h>
#include <sys/socket.h>
#include <net/if.h>
#include <unistd.h>
#include <fcntl.h>

#include<linux/can.h>
#include "../../../mcu/include/MCULogger.h"

class WriteDataByIdentifierService
{
private:
    int write_socket;

    /* Declaring a dictionary for DIDs with example values */
    typedef uint16_t DID;
    typedef std::vector<uint8_t> DataParameter;
    std::map<DID, DataParameter> ecu_memory = {
        {0xF190, {0x11, 0x44}},
        {0xF111, {0x11, 0x44,0x11, 0x44}},
        {0xF113, {0x11, 0x44}},
        {0xF124, {0x11, 0x44, 0x11, 0x44}},
        {0xF188, {0x11, 0x44}},
        {0xF18A, {0x11}},
        {0xF18B, {0x11, 0x44}},
        {0xF18C, {0x11, 0x44, 0x11, 0x44, 0x11, 0x44}},
        {0xF18D, {0x11, 0x44}},
        {0xF18E, {0x11, 0x44, 0xf2}}
    };
public:
    WriteDataByIdentifierService(canid_t frame_id, std::vector<uint8_t> frame_data);
    ~WriteDataByIdentifierService();
    void WriteDataByIdentifier(canid_t frame_id, std::vector<uint8_t> frame_data);
    void sendFrame(can_frame frame);
    int getWriteSocket();
    int initAndBindSocket(char interfaceName[6]); 

};
