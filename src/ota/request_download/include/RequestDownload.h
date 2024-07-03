#ifndef REQUEST_DOWNLOAD_SERVICE_H
#define REQUEST_DOWNLOAD_SERVICE_H

#include <iostream>
#include <vector>
#include <sstream>
#include <linux/can.h>
#include <net/if.h>
#include <cstring>
#include <string>
#include <sys/ioctl.h>
#include "../../../utils/include/CreateInterface.h"

#include "../../../utils/include/GenerateFrames.h"
#include "../../utils/include/Logger.h"

class RequestDownloadService {
public:
    //Logger& logger; // Declare logger as a non-static member variable
    RequestDownloadService();
    RequestDownloadService(Logger& RDSlogger);
    ~RequestDownloadService();
    void requestDownloadRequest(int id, std::vector<uint8_t>stored_data, Logger& RDSlogger);
private:
    
    GenerateFrames generateFrames;
    CreateInterface* create_interface;
    bool isInProgrammingSession();
    bool isValidMemoryRange(const std::vector<int>& memory_address, const std::vector<int>& memory_size, Logger &RDSlogger);
    bool isRequestAuthenticated(uint8_t id, uint8_t dataFormatIdentifier);
};

#endif /* REQUEST_DOWNLOAD_SERVICE_H */ 