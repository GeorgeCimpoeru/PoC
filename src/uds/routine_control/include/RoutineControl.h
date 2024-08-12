/**
 * @file RoutineControl.h
 * @author Mihnea Tanasevici
 * @brief This library represents the Routine Control UDS service.
 * This is used to start a specific routine based on a routine identifierd provided in the request
 */
#ifndef UDS_ROUTINE_CONTROL_H
#define UDS_ROUTINE_CONTROL_H

#include <linux/can.h>
#include <vector>
#include <bitset>

#include "../../../utils/include/GenerateFrames.h"
#include "../../utils/include/Logger.h"
#include "../../utils/include/MemoryManager.h"
#include "../../../utils/include/NegativeResponse.h"
#include "../../authentication/include/SecurityAccess.h"

class RoutineControl
{
    public:
    /* Define the service identifier for Routine Control */
    static constexpr uint8_t ROUTINE_CONTROL_SID = 0x31;
    /**
    * @brief Default constructor
    */
    RoutineControl(int socket, Logger& rdbi_logger);
    /**
    * @brief Method that retrieves some data based on a DID.
    * @param can_id The frame id.
    * @param request Data from a can frame that contains PCI, SID and routineIdentifier
    */
    void routineControl(canid_t can_id, const std::vector<uint8_t>& request);
    
    private:
    GenerateFrames generate_frames;
    int socket = -1;
    Logger& rc_logger;

};

#endif