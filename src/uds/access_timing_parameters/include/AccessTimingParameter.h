/**
 * @file AccessTimingParameter.h
 * @author Dirva Nicolae
 * @brief This library represents the Access Timing Parameter UDS service.
 * 
 * The Access Timing Parameter service manages the configuration and retrieval of timing parameters used in UDS communication.
 * Specifically, it handles the default and currently active P2 and P2* maximum time values.
 * The service allows for reading the default timing parameters, resetting them to their default values,
 * and setting them to new values based on the received CAN frame.
 * 
 * The request frame has the format:
 * frame.data = {PCI_L(1 byte), SID(1 byte = 0x83), SUB_FUNCTION(1 byte), ...}
 * 
 * The positive response frame sent by the service will have the format:
 * frame.data = {PCI_L(1 byte), RESPONSE_SID(1 byte = 0xC3), SUB_FUNCTION(1 byte), ...}
 * 
 * The negative response frame sent by the service will have the format:
 * frame.data = {PCI_L(1 byte), 0x7F, SID(1 byte = 0x83), NRC(1 byte)} indicating errors such as
 * unsupported sub-functions or incorrect message length.
 */

#ifndef UDS_ATP_SERVICE
#define UDS_ATP_SERVICE

#include "../../../utils/include/GenerateFrames.h"
#include "../../../utils/include/Logger.h"
#include "../../../utils/include/NegativeResponse.h"

#include <linux/can.h>
#include <cstdlib>
#include <sys/ioctl.h>
#include <sys/socket.h>

class AccessTimingParameter
{
public:
    /**
     * @brief Constructor
     * 
     * @param logger A logger instance used to record information and errors during the execution.
     * @param socket The socket descriptor used for communication over the CAN bus.
     */
    AccessTimingParameter(Logger logger, int socket);

    /**
     * @brief Destroy the Access Timing Parameter object
     * 
     */
    ~AccessTimingParameter();

    /**
     * @brief  This method handles the Access Timing Parameter request by calling
     * the appropriate function based on the sub-function received.
     * 
     * @param frame_id The id of the received frame. 
     * @param sub_function The sub-function indicating which timing parameter action to perform.
     * @param frame_data Data from the frame.
     */
    void handleRequest(canid_t frame_id, uint8_t sub_function, std::vector<uint8_t> frame_data);

    /**
     * @brief This method returns the default P2 and P2* time values. 
     * This method will provide both the default and currently active timing parameter values for the communication.
     * It allows the client to understand what the default and current timeout values are.
     * 
     * @param frame_id The id of the received frame.
     */
    void readExtendedTimingParameters(canid_t frame_id);

    /**
     * @brief This method sets the timing parameters to their default values.
     * This method resets the timing parameters (P2 and P2*) to their default values
     * It ensures that any custom timing settings are reverted back to the standard default values.
     * 
     * @param frame_id The id of the received frame.
     */
    void setTimingParametersToDefault(canid_t frame_id);

    /**
     * @brief This method returns the currently active P2 and P2* max time values.
     * This method provides the current timing parameter values that are actively being used by the communication link.
     * It allows the client to verify the current timeout settings without referring to the default values.
     * 
     * @param frame_id The id of the received frame.
     */
    void readCurrentlyActiveTimingParameters(canid_t frame_id);

    /**
     * @brief Sets the timing parameters to the provided values.
     * Configures the maximum time allowed for different session operations.
     * 
     * @param frame_id The id of the received frame.
     * @param data_frame Data from the frame.
     * @param p2_max_time Maximum time for default session operations.
     * @param p2_star_max_time Maximum time for programming session operations.
     */
    void setTimingParameters(canid_t frame_id, std::vector<uint8_t> data_frame);
    
    /* The default maximum time for the default session in milliseconds */
    static const uint16_t DEFAULT_P2_MAX_TIME;
    /* The default maximum time for the programming session in milliseconds */
    static const uint16_t DEFAULT_P2_STAR_MAX_TIME;

    static uint16_t p2_max_time;
    static uint16_t p2_star_max_time;

private:
    Logger atp_logger;
    int socket = -1;
    int module_id;
};

#endif