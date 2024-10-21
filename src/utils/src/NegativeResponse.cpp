#include "../include/NegativeResponse.h"

const std::map<uint8_t, std::string> NegativeResponse::nrcMap =
{
    {SFNS, "SubFunction Not Supported"},
    {IMLOIF, "Incorrect Message Length Or Invalid Format"},
    {RTL, "Response Too Long"},
    {CNC, "Conditions Not Correct"},
    {RSE, "Request Sequence Error"},
    {NRSC, "No Response From Subnet Component"},
    {ROOR, "Request Out Of Range"},
    {SAD, "Security Access Denied"},
    {AR, "Authentication Required"},
    {IK, "Invalid Key"},
    {ENOA, "Exceeded Number Of Attempts"},
    {RTDNE, "Required Time Delay Not Expired"},
    {UDNA, "Upload Download Not Accepted"},
    {TDS, "Transfer Data Suspended"},
    {GPF, "General Programming Failure"},
    {WBSC, "Wrong Block Sequence Counter"},
    {VTH, "Voltage Too High"},
    {VTL, "Voltage Too Low"},
    {RCR_RP, "Request Correctly Received-Response Pending"},
    {SFNSIAS, "SubFunction Not Supported In Active Session"},
    {FNSIAS, "Function Not Supported In Active Session"},
    {URDTC, "Unable to read DTCs"}
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
    if(nrc != 0x78)
    {
        LOG_ERROR(nrc_logger.GET_LOGGER(), fmt::format(getDescription(nrc) + " for requested service with SID 0x{:x}",sid));
    }
    else
    {
        LOG_WARN(nrc_logger.GET_LOGGER(), fmt::format(getDescription(nrc) + " for requested service with SID 0x{:x}",sid));
    }
}