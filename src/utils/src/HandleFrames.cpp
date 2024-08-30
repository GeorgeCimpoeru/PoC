
#include "../include/HandleFrames.h"

HandleFrames::HandleFrames(int socket, Logger& logger)
            : _socket(socket), _logger(logger), mcuDiagnosticSessionControl(_logger, _socket)
{

}

/* Method to handle a can frame */
void HandleFrames::handleFrame(int can_socket, const struct can_frame &frame) 
{
    /* Indicates whether the first frame has been received */
    static bool first_frame_received = false;
    /* Number of expected bytes of payload for multi-frame sequence */
    static uint8_t expected_payload = 0;
    /* frame data - for both single and multi frame sequence */
    static std::vector<uint8_t> frame_data;
    /* remember SID in case of multi-frame sequence*/
    static uint8_t sid = 0;
    /* check if the data is split across multiple frames */
    static bool is_multi_frame = false;
    /* Expected sequence number for consecutive frames */
    static uint8_t expected_sequence_number = 0x21;

    /* frame integrity checks will remain in Receive class for now */
    /* id < 0x10 == single frame*/
    if (frame.data[0] < 0x10) 
    {
        /* if frame is negative response, 0x7F takes sid's place so sid comes next to it */
        if (frame.data[1] == 0x7F)
        {
            sid = frame.data[2];
        }
        else
        {
            sid = frame.data[1];  
        }
        LOG_INFO(_logger.GET_LOGGER(), "Single Frame received:");
        for (uint8_t data_pos = 0; data_pos < frame.can_dlc; ++data_pos) 
        {
            frame_data.push_back(frame.data[data_pos]);
        }
        std::cout << std::endl;
        is_multi_frame = false;
        /* Enter the switch case */
        processFrameData(can_socket, frame.can_id, sid, frame_data, is_multi_frame);
        /* clear data to be used by a future frame */
        frame_data.clear();
    /* id == 0x10 == first frame */
    } 
    else if (frame.data[0] == 0x10) 
    {   
        /* Number of expected bytes of payload */
        expected_payload = frame.data[1];
        /* Number of expected frames */
        uint8_t expected_frames = frame.data[1] / 7;
        if(frame.data[1] % 7 > 0)
        {
            expected_frames++;
        }
        /* get SID from the first frame */
        sid = frame.data[2];
        LOG_INFO(_logger.GET_LOGGER(), "Multi-frame Sequence with {} {}", (int)expected_frames, "frames");

        /* Clear the multi_frame_data vector when receiving the first frame */
        frame_data.clear();

        /* Concatenate the data from the first frame into multi_frame_data */
        for (uint8_t data_pos = 1; data_pos < frame.can_dlc; ++data_pos) 
        {
            frame_data.push_back(frame.data[data_pos]);
        }
        /* Make sure sequenceNumber is set after receiving the first frame */
        expected_sequence_number = 0x21;
        first_frame_received = true;
    /* id >= 0x20 == consecutive frame */
    } 
    else if (frame.data[0] >= 0x21 && frame.data[0] < 0x30) 
    {
        if (!first_frame_received) 
        {
            /* Ignore consecutive frames until the first frame is received */
            return;
        }
        /* Check if the frame's sequence number matches the expected sequence number */
        if (frame.data[0] != expected_sequence_number) 
        {
            LOG_ERROR(_logger.GET_LOGGER(), "Invalid consecutive frame sequence: expected {} {} {}", int(expected_sequence_number), "but received", int(frame.data[0]));
            return;
        }
        /* Concatenate the data from consecutive frames into multi_frame_data */
        for (uint8_t data_pos = 1; data_pos < frame.can_dlc; ++data_pos) 
        {
            frame_data.push_back(frame.data[data_pos]);
        }
        /* Increment sequenceNumber after each concatenation*/
        expected_sequence_number++;
        /* Check if all multi-frames have been received */
        if (frame_data.size() >= expected_payload) 
        { 
            LOG_INFO(_logger.GET_LOGGER(), "Data is: ");
            for (uint8_t data_pos = 0; data_pos < (int)frame_data.size(); ++data_pos) 
            {
                LOG_INFO(_logger.GET_LOGGER(), int(frame_data[data_pos]));
            }
            std::cout << std::endl;
            is_multi_frame = true;
            /* Enter the switch case */
            processFrameData(can_socket, frame.can_id, sid, frame_data, is_multi_frame);
            /* Reset sequenceNumber for the next sequence */
            expected_sequence_number = 0x21;
            /* Reset variables */
            first_frame_received = false;
            expected_payload = 0;
            frame_data.clear();
        }
    } 
    else 
    {
        LOG_ERROR(_logger.GET_LOGGER(), "Invalid frame type.");
    }
}

/* Method to call the service or handle the response*/
void HandleFrames::processFrameData(int can_socket, canid_t frame_id, uint8_t sid, std::vector<uint8_t> frame_data, bool is_multi_frame) 
{
    switch (sid) {
        case 0x10:
        {
            /* DiagnosticSessionControl(sid, frame_data[2]); */
            /* This service can be called in any session */
            LOG_INFO(_logger.GET_LOGGER(), "DiagnosticSessionControl called.");
            mcuDiagnosticSessionControl.sessionControl(frame_id, frame_data[2]);
            break;
        }
        case 0x11:
        {
            /* Negative response */
            LOG_INFO(_logger.GET_LOGGER(), "Service 0x11 EcuReset called");
            uint8_t sub_function = frame_data[2];
            LOG_INFO(_logger.GET_LOGGER(), "sub_function: {}", static_cast<int>(sub_function));

            /* Calls ECU Reset */      
            /* This service can be called in any session. */
            EcuReset ecu_reset(frame_id, sub_function, can_socket, _logger);
            ecu_reset.ecuResetRequest();       
            break;
        }
        case 0x27:
        {
            /* This service can be called in PROGRAMMING_SESSION */
            if(DiagnosticSessionControl::getCurrentSessionToString() == "PROGRAMMING_SESSION")
            {
                LOG_INFO(_logger.GET_LOGGER(), "SecurityAccess called.");
                SecurityAccess security_access(can_socket,_logger);
                security_access.securityAccess(frame_id, frame_data);
            }
            else
            {
                int new_id = ((frame_id & 0xFF) << 8) | ((frame_id >> 8) & 0xFF);
                NegativeResponse negative_response(can_socket, _logger);
                negative_response.sendNRC(new_id, 0x83, 0x7E);
            }
            break;
        }
        case 0x29:
        {
            /* Authentication(); */
            LOG_INFO(_logger.GET_LOGGER(), "Authentication called.");
            break;
        }
        case 0x3E:
        {
            LOG_INFO(_logger.GET_LOGGER(), "TesterPresent called.");
            TesterPresent tester_present(_logger, &mcuDiagnosticSessionControl, can_socket, 1000);
            tester_present.handleTesterPresent(frame_id, frame_data);
            break;
        }
        case 0x83:
        {
            LOG_INFO(_logger.GET_LOGGER(), "AccessTimingParameters called.");
             AccessTimingParameter access_timing_parameter(_logger, can_socket);
            if(frame_data.size() < 4)
            {
                access_timing_parameter.handleRequest(frame_id, frame_data[2], {});
            }
            else
            {
                std::vector<uint8_t> timing_parameters(frame_data.begin() + 3, frame_data.end());
                access_timing_parameter.handleRequest(frame_id, frame_data[2], timing_parameters);
            }
            break;
        }
        case 0x22:
        {
            /* ReadDataByIdentifier(sid, frame_data[2] << 8) | frame_data[3]); */
            /* This service can be called in any session */
            LOG_INFO(_logger.GET_LOGGER(), "ReadDataByIdentifier called.");
            ReadDataByIdentifier read_data_by_identifier(can_socket, _logger);
            read_data_by_identifier.readDataByIdentifier(frame_id, frame_data, true);
            break;
        }
        case 0x23:
        {
            /* ReadMemoryByAddress(frame_data[2], frame_data[3] << 8) | frame_data[4], frame_data[5] << 8) | frame_data[6]); */
            LOG_INFO(_logger.GET_LOGGER(), "ReadMemoryByAddress called.");
            if(frame_data[1] == 0x7F)
            {
                LOG_INFO(_logger.GET_LOGGER(), "Negative Response received.");
                NegativeResponse negative_response(can_socket, _logger);
                negative_response.sendNRC(frame_id, 0x23, frame_data[3]);
            }
            else
            {
                LOG_INFO(_logger.GET_LOGGER(), "ReadMemoryByAddress called.");

                /* Assuming that frame_data[2-5] contains the memory address and frame_data[6-7] contains the memory size */
                off_t memory_address = (frame_data[2] << 24) | (frame_data[3] << 16) | (frame_data[4] << 8) | frame_data[5];
                off_t memory_size = (frame_data[6] << 8) | frame_data[7];

                /* Create instances of MemoryManager and GenerateFrames as needed */
                MemoryManager memoryManager(memory_address, "/dev/loop25", _logger);
                GenerateFrames frameGenerator(_socket, _logger);
                ReadMemoryByAddress read_memory_by_address(&memoryManager, frameGenerator, _socket, _logger);

                read_memory_by_address.handleRequest(frame_id, memory_address, memory_size);
            }
            break;
        }
        case 0x2E:
        {
            /* WriteDataByIdentifier(sid, frame_data[2] << 8) | frame_data[3], data_parameter?); */
            /* This service can be called in any session. */
            LOG_INFO(_logger.GET_LOGGER(), "WriteDataByIdentifier service called!");
            WriteDataByIdentifier write_data_by_identifier(_logger, can_socket);
            write_data_by_identifier.WriteDataByIdentifierService(frame_id, frame_data);
            break;
        }
        case 0x14:
        {
            /* ClearDiagnosticInformation(); */
            /* This service can be called in any session */
            LOG_INFO(_logger.GET_LOGGER(), "ClearDiagnosticInformation called.");
            ClearDtc clear_dtc("../uds/read_dtc_information/dtcs.txt", _logger, can_socket);
            clear_dtc.clearDtc(frame_id, frame_data);
            break;  
        }
        case 0x19:
        {
            /* ReadDtcInformation(); */
            /* This service can be called in any session */
            LOG_INFO(_logger.GET_LOGGER(), "ReadDtcInformation called.");
            /* verify_frame() */
            ReadDTC readDtc(_logger, "../uds/read_dtc_information/dtcs.txt", can_socket);
            readDtc.read_dtc(frame_id, frame_data);
            break;
        }
        case 0x31:
        {
            /* RoutineControl(sid, frame_data[2], frame_data[3] << 8) | frame_data[4]); */
            /* This service can be called in any session. */
            LOG_INFO(_logger.GET_LOGGER(), "RoutineControl called.");
            RoutineControl routine_control(can_socket, _logger);
            routine_control.routineControl(frame_id, frame_data);
            break;
        }
        /* UDS Responses */
        case 0x50:
        {
            /* Response for DiagnosticSessionControl */
            LOG_INFO(_logger.GET_LOGGER(), "Response for DiagnosticSessionControl received.");
            break;
        }
        case 0x51:
        {
            /* Response from ECU Reset service */
            LOG_INFO(_logger.GET_LOGGER(), "Response for ECUReset received.");
            break;
        }
        case 0x67:
        {
            /* Response from SecurityAccess() service */
            LOG_INFO(_logger.GET_LOGGER(), "Response from SecurityAccess received.");
            break;
        }
        case 0x69:
        {
            /* Response from Authentication() service */
            LOG_INFO(_logger.GET_LOGGER(), "Response from Authentication received.");
            break;
        }
        case 0x7E:
        {
            /* Response from TesterPresent() service */
            LOG_INFO(_logger.GET_LOGGER(), "Response from TesterPresent received.");
            break;
        }
        case 0xC3:
        {
            /* Response from AccessTimingParameters() service */
            LOG_INFO(_logger.GET_LOGGER(), "Response from AccessTimingParameters received.");
            break;
        }
        case 0x62:
        {
            /* Response from ReadDataByIdentifier() service */
            if(is_multi_frame)
            {
                LOG_INFO(_logger.GET_LOGGER(),"Response from ReadDataByIdentifier received.");
                LOG_INFO(_logger.GET_LOGGER(), "Received multiple frames containing {} {}", frame_data.size(), "bytes of data");
            }
            else 
            {
                LOG_INFO(_logger.GET_LOGGER(),"Response from ReadDataByIdentifier received in one frame.");
            }
            break;
        }
        case 0x63:
        {
            /* Response from ReadMemoryByAddress() service */
            if(is_multi_frame)
            {
                LOG_INFO(_logger.GET_LOGGER(),"Response from ReadMemoryByAdress received.");
                LOG_INFO(_logger.GET_LOGGER(),"Received multiple frames containing {} {}", frame_data.size(), "bytes of data");
            }
            else 
            {
                LOG_INFO(_logger.GET_LOGGER(),"Response from ReadMemoryByAdress received in one frame.");
            }
            break;
        }
        case 0x6E:
        {
            /* Response from WriteDataByIdentifier() service */
            LOG_INFO(_logger.GET_LOGGER(), "Response from WriteDataByIdentifier received.");
            break;
        }
        case 0x54:
        {
            /* Response from ClearDiagnosticInformation() service */
            LOG_INFO(_logger.GET_LOGGER(), "Response from ClearDiagnosticInformation received.");
            break;
        }
        case 0x59:
        {
            /* Response from ReadDtcInformation() service */
            LOG_INFO(_logger.GET_LOGGER(), "Response from ReadDtcInformation received.");
            break;
        }
        case 0x71:
        {
            /* Response from RoutineControl() service */
            LOG_INFO(_logger.GET_LOGGER(), "Response from RoutineControl received.");
            break;
        }
        /* OTA Requests */
        case 0x34:
        {
            /* This service can be called in PROGRAMMING_SESSION */
            if(DiagnosticSessionControl::getCurrentSessionToString() == "PROGRAMMING_SESSION")
            {
                RequestDownloadService requestDownload(can_socket, _logger);
                ReadDataByIdentifier software_version(can_socket, _logger);
                requestDownload.requestDownloadRequest(frame_id, frame_data);
            }
            else
            {
                int new_id = ((frame_id & 0xFF) << 8) | ((frame_id >> 8) & 0xFF);
                NegativeResponse negative_response(can_socket, _logger);
                negative_response.sendNRC(new_id, 0x34, 0x7F);
            }
            break;
        }
        case 0x36:
        {
            /* TransferData(sid, frame_data[2], frame_data[3], frame_data[4]); */
            if(is_multi_frame)
            {
                LOG_INFO(_logger.GET_LOGGER(), "TransferData called with multiple frames.");
            }
            else 
            {
                /* This service can be called in PROGRAMMING_SESSION */
                if(DiagnosticSessionControl::getCurrentSessionToString() == "PROGRAMMING_SESSION")
                {
                    TransferData transfer_data(can_socket, _logger);
                    transfer_data.transferData(frame_id, frame_data);
                    LOG_INFO(_logger.GET_LOGGER(), "TransferData called with one frame.");
                }
                else
                {
                    int new_id = ((frame_id & 0xFF) << 8) | ((frame_id >> 8) & 0xFF);
                    NegativeResponse negative_response(can_socket, _logger);
                    negative_response.sendNRC(new_id, 0x36, 0x7F);
                }
            }
            break;
        }
        case 0x37:
        {
            /* RequestTransferExit(sid, frame_data[2]); */
            /* This service can be called in PROGRAMMING_SESSION */
            if(DiagnosticSessionControl::getCurrentSessionToString() == "PROGRAMMING_SESSION")
            {
                LOG_INFO(_logger.GET_LOGGER(), "Request Transfer Exit Service 0x37 called");
                RequestTransferExit request_transfer_exit(can_socket, _logger);
                request_transfer_exit.requestTRansferExitRequest(frame_id, frame_data);
            }
            else
            {
                int new_id = ((frame_id & 0xFF) << 8) | ((frame_id >> 8) & 0xFF);
                NegativeResponse negative_response(can_socket, _logger);
                negative_response.sendNRC(new_id, 0x37, 0x7F);
            }
            break;
        }
        case 0x32:
        {
            /* RequestUpdateStatus(); */
            /* This service can be called in PROGRAMMING_SESSION */
            if(DiagnosticSessionControl::getCurrentSessionToString() == "PROGRAMMING_SESSION")
            {
                LOG_INFO(_logger.GET_LOGGER(), "RequestUpdateStatus called.");
                RequestUpdateStatus RUS(can_socket, _logger);
                RUS.requestUpdateStatus(frame_id, frame_data);
            }
            else
            {
                int new_id = ((frame_id & 0xFF) << 8) | ((frame_id >> 8) & 0xFF);
                NegativeResponse negative_response(can_socket, _logger);
                negative_response.sendNRC(new_id, 0x32, 0x7F);
            }
            break;
        }
        /* OTA Responses */
        case 0x74:
        {
            /* Response from RequestDownload() service */
            LOG_INFO(_logger.GET_LOGGER(), "Response from RequestDownload received.");
            break;
        }
        case 0x76:
        {
            /* Response from TransferData() service */
            LOG_INFO(_logger.GET_LOGGER(), "Response from TransferData received.");
            break;
        }
        case 0x77:
        {
            /* Response from RequestTransferExit() service */
            LOG_INFO(_logger.GET_LOGGER(), "Response from RequestTransferExit received.");
            break;
        }
        case 0x72:
        {
            /* Response from RequestUpdateStatus() service */
            LOG_INFO(_logger.GET_LOGGER(), "Response from RequestUpdateStatus received.");
            break;
        }
        default:
        {
            /* Unknown request/response */
            LOG_INFO(_logger.GET_LOGGER(), "Unknown request/response received.");
            break;
        }
    }
}