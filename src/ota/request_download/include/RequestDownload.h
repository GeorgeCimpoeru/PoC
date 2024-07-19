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
#include <mutex>
#include "../../../utils/include/CreateInterface.h"
#include "../../../utils/include/GenerateFrames.h"
#include "../../utils/include/Logger.h"
#include "../../uds/diagnostic_session_control/include/DiagnosticSessionControl.h"
#include "../../uds/authentication/include/SecurityAccess.h"
#include "../../uds/read_data_by_identifier/include/ReadDataByIdentifier.h"
#include "../../utils/include/MemoryManager.h"

class RequestDownloadService
{
public:
    /**
     * @brief Construct a new Request Download Service object
     * 
     * @param RDSlogger 
     */
    RequestDownloadService(Logger &RDSlogger);
    /**
     * @brief Construct a new Request Download Service object
     * 
     * @param socket 
     * @param RDSlogger 
     */
    RequestDownloadService(int socket, Logger &RDSlogger);
    /**
     * @brief Destroy the Request Download Service object
     *
     */
    ~RequestDownloadService();
    /**
     * @brief Request download method to validate and prepare the system to receive data
     * 
     * @param id 
     * @param stored_data 
     * @param RDSlogger 
     * @param mcuDiagnosticSessionControl 
     * @param software_version 
     * @param logged_in 
     */
    void requestDownloadRequest(int id, std::vector<uint8_t> stored_data, Logger &RDSlogger, DiagnosticSessionControl mcuDiagnosticSessionControl, ReadDataByIdentifier software_version, SecurityAccess logged_in);
    /**
     * @brief Response method from manual Request Download service to start the download in MCU
     * 
     * @param id 
     * @param max_number_block 
     * @param RDSlogger 
     */
    void requestDownloadResp(int id, int max_number_block, Logger &RDSlogger);

private:
    int socket = -1;
    GenerateFrames generateFrames;
    /**
     * @brief Method for checking if the MCU is in the programming session
     * 
     * @param mcuDiagnosticSessionControl 
     * @param RDSlogger 
     * @return true 
     */
    bool isInProgrammingSession(DiagnosticSessionControl mcuDiagnosticSessionControl, Logger &RDSlogger);
    /**
     * @brief Method for validation of the provided memory address and size, ensuring they are within acceptable bounds and logical ranges.
     *
     * @param memory_address
     * @param memory_size
     * @param RDSlogger
     * @return true
     */
    bool isValidMemoryRange(const std::vector<int> &memory_address, const std::vector<int> &memory_size, Logger &RDSlogger);
    /**
     * @brief Method to authenticate the download request based on the provided identifiers
     * 
     * @param logged_in 
     * @param RDSlogger 
     * @return true 
     */
    bool isRequestAuthenticated(SecurityAccess logged_in, Logger &RDSlogger);
    /**
     * @brief Method to check last software version
     * 
     * @param software_version 
     * @param RDSlogger 
     * @return true 
     * @return false 
     */
    bool isLatestSoftwareVersion(ReadDataByIdentifier software_version, Logger &RDSlogger);
};

#endif /* REQUEST_DOWNLOAD_SERVICE_H */