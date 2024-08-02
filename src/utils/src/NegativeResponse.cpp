#include "../include/NegativeResponse.h"

const std::map<uint8_t, std::string> NegativeResponse::nrcMap =
{
    {0x12, "SubFunction Not Supported"},
    {0x13, "Incorrect Message Length Or Invalid Format"},
    {0x14, "Response Too Long"},
    {0x22, "Conditions Not Correct"},
    {0x24, "Request Sequence Error"},
    {0x25, "No Response From Subnet Component"},
    {0x31, "Request Out Of Range"},
    {0x33, "Security Access Denied"},
    {0x34, "Authentication Required"},
    {0x35, "Invalid Key"},
    {0x36, "Exceeded Number Of Attempts"},
    {0x37, "Required Time Delay NotExpired"},
    {0x70, "Upload Download Not Accepted"},
    {0x71, "Transfer Data Suspended"},
    {0x72, "General Programming Failure"},
    {0x73, "Wrong Block Sequence Counter"},
    {0x92, "Voltage Too High"},
    {0x93, "Voltage Too Low"}
};

NegativeResponse::NegativeResponse(int socket, Logger& nrc_logger)
                :  nrc_logger(nrc_logger), socket(socket)
{}

std::string NegativeResponse::getDescription(uint8_t code)
{
    auto it = nrcMap.find(code);
    if (it != nrcMap.end())
    {
        return it->second;
    }
    else
    {
        return "Negative Response not supported";
    }
}

void NegativeResponse::sendNRC(int id, uint8_t sid, uint8_t nrc)
{
    GenerateFrames generate_frames(socket, nrc_logger);
    generate_frames.negativeResponse(id, sid, nrc);
    LOG_ERROR(nrc_logger.GET_LOGGER(), fmt::format(getDescription(nrc) + " for requested service with SID 0x{:x}",sid));
}