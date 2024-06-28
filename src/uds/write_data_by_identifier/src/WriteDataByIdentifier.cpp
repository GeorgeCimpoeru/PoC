#include "../include/WriteDataByIdentifier.h"

WriteDataByIdentifierService::WriteDataByIdentifierService(canid_t frame_id, std::vector<uint8_t> frame_data)
{   
    char interfaceNameCANBus[6] = "vcan0";
    this->write_socket = initAndBindSocket(interfaceNameCANBus);
    WriteDataByIdentifier(frame_id, frame_data);
};

WriteDataByIdentifierService::~WriteDataByIdentifierService()
{
    close(write_socket);
};

void WriteDataByIdentifierService::WriteDataByIdentifier(canid_t frame_id, std::vector<uint8_t> frame_data)
{
    LOG_INFO(MCULogger.GET_LOGGER(), "Write Data By Identifier Service invoked.");

    /* Checks if frame_data has the required minimum length */
    if (frame_data.size() < 4)
    {
        LOG_ERROR(MCULogger.GET_LOGGER(),"Incorrect Message Length or Invalid Format");
        /* call NegativeResponseService */

    }

    /* Extract Data Identifier (DID) */
    DID did = (frame_data[2] << 8) | frame_data[3];

    // Check if the DID-ul is in ecu_memory
    auto it = ecu_memory.find(did);
    if (it == ecu_memory.end())
    {
        LOG_ERROR(MCULogger.GET_LOGGER(),"Request Out Of Range: Identifier not found in ECU memory");
    }

    /* Extract Data Parameter */
    DataParameter data_parameter(frame_data.begin() + 4, frame_data.end());

    /* Writes the data to the memory location associated with the DID */
    ecu_memory[did] = data_parameter;
    LOG_INFO(MCULogger.GET_LOGGER(), "Data written to DID 0x{:x}.", did);

    // LOG_INFO(MCULogger.GET_LOGGER(), fmt::format("ECU_MEMORY:"));
    // for (const auto& it : ecu_memory) {
    //     // Format the key (DID) and the value (DataParameter)
    //     std::string data_str;
    //     for (const auto& byte : it.second) {
    //         data_str += fmt::format("{:02X} ", byte); // Convert each byte to hex
    //     }

    //     LOG_INFO(MCULogger.GET_LOGGER(), fmt::format("ecu_memory element: {}", data_str));
    // }
    // LOG_INFO(MCULogger.GET_LOGGER(), fmt::format("Data written to DID: 0x{0:x}", int(did)));

    /* Create the response frame */
    can_frame response_frame;
    /* Invert sender and receiver ID */
    response_frame.can_id = ((frame_id & 0xFF) << 8) | ((frame_id >> 8) & 0xFF);
    /* PCI (1 byte) + SID (1 byte) + DID (2 bytes) */
    response_frame.can_dlc = 4;

    /* Construct the response data */
    response_frame.data[0] = 0x03;
    response_frame.data[1] = 0x6E;
    response_frame.data[2] = frame_data[1];
    response_frame.data[3] = frame_data[2];

    /* Send the response frame */
    sendFrame(response_frame);
};

void WriteDataByIdentifierService::sendFrame(can_frame frame)
{
    int nbytes = write(getWriteSocket(), &frame, sizeof(frame));
    if (nbytes < 0)
    {
        LOG_ERROR(MCULogger.GET_LOGGER(), "Error sending frame");
    }
    LOG_INFO(MCULogger.GET_LOGGER(), "Response frame with ID: 0x{0:x} sent on CANBus after writing data", int(frame.can_id));
};

int WriteDataByIdentifierService::getWriteSocket()
{
    return write_socket;
};

int WriteDataByIdentifierService::initAndBindSocket(char interfaceName[6]) 
{
    struct ifreq ifr;
    struct sockaddr_can addr;

    int s = socket(PF_CAN, SOCK_RAW, CAN_RAW);
    if (s < 0) {
        LOG_ERROR(MCULogger.GET_LOGGER(), "Error while opening socket");
        return -1;
    }

    std::strcpy(ifr.ifr_name, interfaceName);
    ioctl(s, SIOCGIFINDEX, &ifr);

    std::memset(&addr, 0, sizeof(addr));
    addr.can_family = AF_CAN;
    addr.can_ifindex = ifr.ifr_ifindex;

    if (bind(s, (struct sockaddr*)&addr, sizeof(addr)) < 0) {
        LOG_ERROR(MCULogger.GET_LOGGER(), "Error in socket bind");
        return -2;
    }

    return s;
}