#include <linux/can.h>
#include <vector>
#include <unordered_map>
#include <bitset>

#include "../../../utils/include/CreateInterface.h"
#include "../../../utils/include/GenerateFrames.h"
#include "../../../utils/include/Logger.h"

#ifndef UDS_RDBI_SERVICE
#define UDS_RDBI_SERVICE

class ReadDataByIdentifier
{   
    public:
    /**
    * @brief Default constructor
    */
    ReadDataByIdentifier();
    /**
    * @brief Parameterized constructor.
    * @param RDBILogger Logger reference to log in a specific file.
    */
    ReadDataByIdentifier(Logger& RDBILogger);
    /**
    * @brief Method that retrieves some data based on a DID.
    * @param can_id The frame id.
    * @param request Data from a can frame that contains PCI, SID and DID
    * @param RDBILogger Logger reference to log in a specific file.
    */
    void readDataByIdentifier(canid_t can_id, const std::vector<uint8_t>& request, Logger& RDBILogger);
    
    private:
    GenerateFrames generate_frames;
    CreateInterface* create_interface;

};

#endif