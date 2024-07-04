#include "../include/NegativeResponse.h"
#include <iostream>
#include <iomanip>

NegativeResponseHandler::NegativeResponseHandler() : socket(-1), frameGenerator(-1, *(new Logger())) {
    initializeNegativeResponseContent();
}

NegativeResponseHandler::NegativeResponseHandler(int socket, Logger& logger) : socket(socket), frameGenerator(socket, logger) {
    initializeNegativeResponseContent();
}

void NegativeResponseHandler::initializeNegativeResponseContent() {
    negativeResponseContent = {
        {NegativeResponseCode::SERVICE_NOT_SUPPORTED, "Service not supported"},
        {NegativeResponseCode::INCORRECT_MESSAGE_LENGTH_OR_INVALID_FORMAT, "Incorrect message length or invalid format"},
        {NegativeResponseCode::RESPONSE_TOO_LONG, "Response too long"},
        {NegativeResponseCode::NO_RESPONSE_FROM_SUBNET_COMPONENT, "No response from subnet component"},
        {NegativeResponseCode::AUTHENTICATION_FAILED, "Authentication failed"},
        {NegativeResponseCode::RESOURCE_TEMPORARILY_UNAVAILABLE, "Resource temporarily unavailable"}
    };
}

std::string NegativeResponseHandler::getErrorMessage(NegativeResponseCode nrc) const {
    auto it = negativeResponseContent.find(nrc);
    if (it != negativeResponseContent.end()) {
        return it->second;
    }
    return "Invalid negative response code";
}

void NegativeResponseHandler::handleNegativeResponse(uint32_t can_id, uint8_t sid, NegativeResponseCode nrc) const {
    std::cerr << "CAN ID: " << std::hex << can_id << ", SID: " << std::hex << static_cast<int>(sid) 
              << ", NRC: " << std::hex << static_cast<int>(nrc) << " - " << getErrorMessage(nrc) << std::endl;
}

int NegativeResponseHandler::getSocket() const {
    return this->socket;
}

int NegativeResponseHandler::sendFrame(uint32_t can_id, std::vector<uint8_t> data, FrameType frame_type) {
    return frameGenerator.sendFrame(can_id, data, frame_type); 
}
