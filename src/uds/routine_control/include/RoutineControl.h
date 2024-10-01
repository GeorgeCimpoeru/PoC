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

#define ERASE_MEMORY_RC_ID (0x0101)
#define INSTALL_UPDATES_RC_ID (0x0201)
#define WRITE_TO_FILE_RC_ID (0x0301)
#define INIT_OTA_RC_ID (0x0401)
#define VERIFY_SW_RC_ID (0x0501)
#define ROLLBACK_SW_RC_ID (0x0601)
#define ACTIVATE_SW_RC_ID (0x0701)

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
    void routineControlResponse(canid_t can_id, uint8_t sub_function, const uint16_t& routine_identifier, std::vector<uint8_t>& routine_result);
    
    /**
     * @brief 
     * 
     * @return true 
     * @return false 
     */
    bool initialiseOta(uint8_t target_ecu, const std::vector<uint8_t>& request, std::vector<uint8_t>& routine_result);

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
    
    bool getCurrentProcessInfo(pid_t& pid, std::string& pname);

    private:
    GenerateFrames generate_frames;
    int socket = -1;
    Logger& rc_logger;

};

#endif