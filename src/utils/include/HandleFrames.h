/**
 * @file HandleFrames.h
 * @author Iancu Daniel
 * @brief This class is used to call the service needed by analyzing the SID
 *        if frame-type is a request or to send the response to API if 
 *        frame-type is a response.
 *        The method handleFrame takes a parameter of type can_frame,
 *        parses the SID and calls the appropiate function within a switch-case.
 * @version 0.1
 * @date 2024-08-20
 * 
 * @copyright Copyright (c) 2024
 * 
 */

#ifndef HANDLE_FRAMES_H
#define HANDLE_FRAMES_H

#include <linux/can.h>
#include <iostream>
#include <vector>
#include <chrono>

#include "../../uds/read_data_by_identifier/include/ReadDataByIdentifier.h"
#include "../../uds/write_data_by_identifier/include/WriteDataByIdentifier.h"
#include "../../uds/ecu_reset/include/EcuReset.h"
#include "../../uds/tester_present/include/TesterPresent.h"
#include "../../uds/authentication/include/SecurityAccess.h"
#include "../../uds/diagnostic_session_control/include/DiagnosticSessionControl.h"
#include "../../uds/read_dtc_information/include/ReadDtcInformation.h"
#include "../../uds/routine_control/include/RoutineControl.h"
#include "../../uds/access_timing_parameters/include/AccessTimingParameter.h"
#include "../../ota/request_download/include/RequestDownload.h"
#include "../../ota/request_update_status/include/RequestUpdateStatus.h"
#include "../../ota/transfer_data/include/TransferData.h"
#include "../../uds/clear_dtc/include/ClearDtc.h"

class HandleFrames 
{
private:
    int module_id = -1;
    int _socket = -1;
    Logger& _logger;
    DiagnosticSessionControl mcuDiagnosticSessionControl;
public:
    /**
     * @brief Default constructor for Handle Frames object.
    */
    HandleFrames(int socket, Logger& logger);
    /**
     * @brief Method used to handle a can frame received from the ReceiveFrame class.
     * Takes a can_frame as parameter, checks if the frame is complete and then calls
     * processFrameData() with either a single or multi frame.
     * @param[in] frame The received frame.
    */
    void handleFrame(int can_socket, const struct can_frame &frame);
    /**
     * @brief Method used to call a service or handle a response.
     * It takes frame_id, service id(sid) and frame_data and calls the right service or
     * handles the response received based on the given parameters.
     * @param[in] frame_id The frame ID used for the handling.
     * @param[in] sid The service identifier.
     * @param[in] frame_data The data held by the frame.
     * @param[in] is_multi_frame Flag that checks if the frame is a multiframe.
    */
    void processFrameData(int can_socket, canid_t frame_id, uint8_t sid, std::vector<uint8_t> frame_data, bool is_multi_frame);
    /**
     * @brief Method used to send a frame based on the nrc(negative response code) received.
     * It takes as parameters frame_id, sid to identify the service, and nrc to send the correct
     * negative response code back to who made the request.
     * @param[in] frame_id The frame ID used for the handling.
     * @param[in] sid The service identifier.
     * @param[in] nrc The negative response code.
     */
    void processNrc(canid_t frame_id, uint8_t sid, uint8_t nrc);
    /**
     * @brief Method for checking the validity of the received CAN frame.
     * 
     * @param nbytes Number of bytes.
     * @param frame Frame to be checked
     * @return Returns true of the processing is done or false if an error is encountered. 
     */
    bool checkReceivedFrame(int nbytes, const struct can_frame &frame);
};
#endif /* HANDLE_FRAMES_H */