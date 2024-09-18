/**
 * @file RoutineControl.h
 * @author Mihnea Tanasevici
 * @brief This library represents the Routine Control UDS service.
 * This is used to start a specific routine based on a routine identified provided in the request
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
    * 
    * @param socket Socket to send frames.
    * @param rc_logger A logger instance used to record information and errors during the execution.
    */
    RoutineControl(int socket, Logger& rc_logger);
    /**
    * @brief Method that retrieves some data based on a DID.
    * 
    * @param can_id The frame id.
    * @param request Data from a can frame that contains PCI, SID and routineIdentifier.
    */
    void routineControl(canid_t can_id, const std::vector<uint8_t>& request);
    /**
    * @brief Method to send response.
    * 
    * @param can_id The frame id.
    * @param request Data from a can frame that contains PCI, SID and routineIdentifier.
    * @param routine_identifier Routine identifier.
    */
    void routineControlResponse(canid_t can_id, const std::vector<uint8_t>& request, const uint16_t& routine_identifier);
    
    /**
    * @brief Method to return a path string based on the receiver id.
    * 
    * @param can_id The frame id.
    * @return Returns a path string
    */

    /**
     * @brief 
     * 
     * @return true 
     * @return false 
     */
    bool initialiseOta();

    /**
     * @brief 
     * 
     * @return true 
     * @return false 
     */
    bool activateSoftware();

    /**
     * @brief 
     * 
     * @return true 
     * @return false 
     */
    bool verifySoftware();
    
    std::string selectEcuPath(canid_t can_id);

    /**
     * @brief Stop the processing timer for the module with an id equal to the receiver id
     * 
     * @param receiver_id 
     */
    void stopTimingFlag(uint8_t receiver_id );

    private:
    GenerateFrames generate_frames;
    int socket = -1;
    Logger& rc_logger;

};

#endif