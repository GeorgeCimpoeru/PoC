#ifndef NEGATIVE_RESPONSE_H
#define NEGATIVE_RESPONSE_H

#include <string>
#include <unordered_map>
#include <cstdint>
#include <vector>
#include "../../../../src/utils/include/GenerateFrames.h"  
#include "../../../../src/utils/include/Logger.h" 

enum class NegativeResponseCode : uint8_t {
    SERVICE_NOT_SUPPORTED = 0x11,
    INCORRECT_MESSAGE_LENGTH_OR_INVALID_FORMAT = 0x13, 
    RESPONSE_TOO_LONG = 0x14,
    NO_RESPONSE_FROM_SUBNET_COMPONENT = 0x25,
    AUTHENTICATION_FAILED = 0x34,
    RESOURCE_TEMPORARILY_UNAVAILABLE = 0x94
};

class NegativeResponseHandler {
public:
    NegativeResponseHandler();
    NegativeResponseHandler(int socket, Logger& logger);
    std::string getErrorMessage(NegativeResponseCode responseCode) const;
    void handleNegativeResponse(uint32_t can_id, uint8_t sid, NegativeResponseCode nrc) const;
    int sendFrame(uint32_t can_id, std::vector<uint8_t> data, FrameType frame_type = FrameType::DATA_FRAME); 
    int getSocket() const;

private:
    void initializeNegativeResponseContent();
    std::unordered_map<NegativeResponseCode, std::string> negativeResponseContent;
    int socket;
    GenerateFrames frameGenerator; 
};

#endif
