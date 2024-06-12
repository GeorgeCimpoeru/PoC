/**
 * @brief 
 * @file HandleFrames.cpp
 * @author Tanasevici Mihnea
 * @date 2024-05-20
 * 
 */
#include "../include/HandleFrames.h"

/* Method to handle a can frame */
void HandleFrames::handleFrame(const struct can_frame &frame) 
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
        std::cout << "Single Frame received:" << std::endl;
        for (uint8_t data_pos = 0; data_pos < frame.can_dlc; ++data_pos) 
        {
            frame_data.push_back(frame.data[data_pos]);
        }
        std::cout << std::endl;
        is_multi_frame = false;
        /* Enter the switch case */
        processFrameData(frame.can_id, sid, frame_data, is_multi_frame);
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
        std::cout << "Multi-frame Sequence with " << (int)expected_frames << " frames:" << std::endl;

        /* Clear the multi_frame_data vector when receiving the first frame */
        frame_data.clear();

        /* Concatenate the data from the first frame into multi_frame_data */
        for (uint8_t data_pos = 5; data_pos < frame.can_dlc; ++data_pos) 
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
            std::cerr << "Invalid consecutive frame sequence: expected " 
                      << std::hex << int(expected_sequence_number) << " but received " 
                      << std::hex << int(frame.data[0]) << std::endl;
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
            std::cout << "data is: ";
            for (uint8_t data_pos = 0; data_pos < (int)frame_data.size(); ++data_pos) 
            {
                std::cout << std::hex << int(frame_data[data_pos]) << " ";
            }
            std::cout << std::endl;
            is_multi_frame = true;
            /* Enter the switch case */
            processFrameData(frame.can_id, sid, frame_data, is_multi_frame);
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
        std::cerr << "Invalid frame type" << std::endl;
    }
}

/* Method to call the service or handle the response*/
void HandleFrames::processFrameData(canid_t frame_id, uint8_t sid, std::vector<uint8_t> frame_data, bool is_multi_frame) 
{

    switch (sid) {
        case 0x10:
            /* DiagnosticSessionControl(sid, frame_data[2]); */
            if(frame_data[1] == 0x7F)
            {
                processNrc(frame_id, sid, frame_data[3]);
            }
            else 
            {   
                LOG_INFO(MCULogger.getConsoleLogger(), "BLABLALBALBALBLA");
                std::cout << "DiagnosticSessionControl called." << std::endl;
            }
            break;
        case 0x11:
            /* EcuReset(sid, frame_data[2]); */
            if(frame_data[1] == 0x7F)
            {
                processNrc(frame_id, sid, frame_data[3]);
            }
            else 
            {
                std::cout << "EcuReset called." << std::endl;
            }
            break;
        case 0x27:
            /* SecurityAccess(sid, frame_data[2], key?); */
            if(frame_data[1] == 0x7F)
            {
                processNrc(frame_id, sid, frame_data[3]);
            }
            else 
            {
                std::cout << "SecurityAccess called." << std::endl;
            }
            break;
        case 0x29:
            /* Authentication(); */
            if(frame_data[1] == 0x7F)
            {
                processNrc(frame_id, sid, frame_data[3]);
            }
            else 
            {
                std::cout << "Authentication called." << std::endl;
            }
            break;
        case 0x3E:
            /* TesterPresent(sid, frame_data[2]); */
            if(frame_data[1] == 0x7F)
            {
                processNrc(frame_id, sid, frame_data[3]);
            }
            else 
            {
                std::cout << "TesterPresent called." << std::endl;
            }
            break;
        case 0x83:
            /* AccessTimingParameters(sid, frame_data[2]); */
            if(frame_data[1] == 0x7F)
            {
                processNrc(frame_id, sid, frame_data[3]);
            }
            else 
            {
                std::cout << "AccessTimingParameters called." << std::endl;
            }
            break;
        case 0x22:
            /* ReadDataByIdentifier(sid, frame_data[2] << 8) | frame_data[3]); */
            if(frame_data[1] == 0x7F)
            {
                processNrc(frame_id, sid, frame_data[3]);
            }
            else 
            {
                std::cout << "ReadDataByIdentifier called." << std::endl;
            }
            break;
        case 0x23:
            /* ReadMemoryByAddress(frame_data[2], frame_data[3] << 8) | frame_data[4], frame_data[5] << 8) | frame_data[6]); */
            if(frame_data[1] == 0x7F)
            {
                processNrc(frame_id, sid, frame_data[3]);
            }
            else 
            {
                std::cout << "ReadMemoryByAddress called." << std::endl;
            }
            break;
        case 0x2E:
            /* WriteDataByIdentifier(sid, frame_data[2] << 8) | frame_data[3], data_parameter?); */
            if(frame_data[1] == 0x7F)
            {
                processNrc(frame_id, sid, frame_data[3]);
            }
            else if (is_multi_frame)
            {
                std::cout << "WriteDataByIdentifier called with multiple frames." << std::endl;
            }
            else 
            {
                std::cout << "WriteDataByIdentifier called with one frame." << std::endl;
            }
            break;
        case 0x14:
            /* ClearDiagnosticInformation(); */
            if(frame_data[1] == 0x7F)
            {
                processNrc(frame_id, sid, frame_data[3]);
            }
            else 
            {
                std::cout << "ClearDiagnosticInformation called." << std::endl;
            }
            break;
        case 0x19:
            /* ReadDtcInformation(); */
            if(frame_data[1] == 0x7F)
            {
                processNrc(frame_id, sid, frame_data[3]);
            }
            else 
            {
                std::cout << "ReadDtcInformation called." << std::endl;
            }
            break;
        case 0x31:
            /* RoutineControl(sid, frame_data[2], frame_data[3] << 8) | frame_data[4]); */
            if(frame_data[1] == 0x7F)
            {
                processNrc(frame_id, sid, frame_data[3]);
            }
            else 
            {
                std::cout << "RoutineControl called." << std::endl;
            }
            break;
        /* UDS Responses */
        case 0x50:
            /* Response for DiagnosticSessionControl */
            std::cout << "Response for DiagnosticSessionControl received." << std::endl;
            break;
        case 0x51:
            /* Response from EcuReset() service */
            std::cout << "Response from EcuReset received." << std::endl;
            break;
        case 0x67:
            /* Response from SecurityAccess() service */
            std::cout << "Response from SecurityAccess received." << std::endl;
            break;
        case 0x69:
            /* Response from Authentication() service*/
            std::cout << "Response from Authentication received." << std::endl;
            break;
        case 0x7E:
            /* Response from TesterPresent() service */
            std::cout << "Response from TesterPresent received." << std::endl;
            break;
        case 0xC3:
            /* Response from AccessTimingParameters() service*/
            std::cout << "Response from AccessTimingParameters received." << std::endl;
            break;
        case 0x62:
            /* Response from ReadDataByIdentifier() service */
            if(is_multi_frame)
            {
                std::cout << "Response from ReadDataByIdentifier received." << std::endl;
                std::cout << "Received multiple frames containing " << std::dec << frame_data.size() << " bytes of data" << std::endl;
            }
            else 
            {
                std::cout << "Response from ReadDataByIdentifier received in one frame." << std::endl;
            }
            break;
        case 0x63:
            /* Response from ReadMemoryByAddress() service */
            if(is_multi_frame)
            {
                std::cout << "Response from ReadMemoryByAdress received." << std::endl;
                std::cout << "Received multiple frames containing " << std::dec << frame_data.size() << " bytes of data" << std::endl;
            }
            else 
            {
                std::cout << "Response from ReadMemoryByAdress received in one frame." << std::endl;
            }
            break;
        case 0x6E:
            /* Response from WriteDataByIdentifier() service */
            std::cout << "Response from WriteDataByIdentifier received." << std::endl;
            break;
        case 0x54:
            /* Response from ClearDiagnosticInformation() service */
            std::cout << "Response from ClearDiagnosticInformation received." << std::endl;
            break;
        case 0x59:
            /* Response from ReadDtcInformation() service */
            std::cout << "Response from ReadDtcInformation received." << std::endl;
            break;
        case 0x71:
            /* Response from RoutineControl() service */
            std::cout << "Response from RoutineControl received." << std::endl;
            break;
        /* OTA Requests */
        case 0x34:
            /* RequestDownload(sid, frame_data[2], frame_data[3], frame_data[4], frame_data[5]); */
            if(frame_data[1] == 0x7F)
            {
                processNrc(frame_id, sid, frame_data[3]);
            }
            else 
            {
                std::cout << "RequestDownload called." << std::endl;
            }
            break;
        case 0x36:
            /* TransferData(sid, frame_data[2], frame_data[3], frame_data[4]); */
            if(frame_data[1] == 0x7F)
            {
                processNrc(frame_id, sid, frame_data[3]);
            }
            else if(is_multi_frame)
            {
                std::cout << "TransferData called with multiple frames." << std::endl;
            }
            else 
            {
                std::cout << "TransferData called with one frame" << std::endl;
            }
            break;
        case 0x37:
            /* RequestTransferExit(sid, frame_data[2]); */
            if(frame_data[1] == 0x7F)
            {
                processNrc(frame_id, sid, frame_data[3]);
            }
            else 
            {
                std::cout << "RequestTransferExit called." << std::endl;
            }
            break;
        case 0x32:
            /* RequestUpdateStatus(); */
            if(frame_data[1] == 0x7F)
            {
                processNrc(frame_id, sid, frame_data[3]);
            }
            else 
            {
                std::cout << "RequestUpdateStatus called." << std::endl;
            }
            break;
        /* OTA Responses */
        case 0x74:
            /* Response from RequestDownload() service */
            std::cout << "Response from RequestDownload received." << std::endl;
            break;
        case 0x76:
            /*Response from TransferData() service */
            std::cout << "Response from TransferData received." << std::endl;
            break;
        case 0x77:
            /* Response from RequestTransferExit() service */
            std::cout << "Response from RequestTransferExit received." << std::endl;
            break;
        case 0x72:
            /* Response from RequestUpdateStatus() service */
            std::cout << "Response from RequestUpdateStatus received." << std::endl;
            break;
        default:
            /* Unknown request/response */
            std::cout << "Unknown request/response received." << std::endl;
            break;
    }
}
void HandleFrames::processNrc(canid_t frame_id, uint8_t sid, uint8_t nrc)
{
    switch(nrc)
    {
        case 0x11:
            /* Service not supported */
            //GenerateFrames::negativeResponse(can_id, sid, nrc);
            LOG_ERROR(MCULogger.getConsoleLogger(), "BLABLALBALBALBLA");
            std::cout << "Error: Service not supported for service: " << std::hex << (int)sid << std::endl;
        break;
        case 0x13:
            /* Incorrect message length or invalid format */
            //GenerateFrames::negativeResponse(can_id, sid, nrc);
            std::cout << "Error: Incorrect message length or invalid format for service: " << std::hex << (int)sid << std::endl;
        break;
        case 0x14:
            /*  Response too long */
            //GenerateFrames::negativeResponse(can_id, sid, nrc);
            std::cout << "Error: Response too long for service: " << std::hex << (int)sid << std::endl;
        break;
        case 0x25:
            /* No response from subnet component */
            //GenerateFrames::negativeResponse(can_id, sid, nrc);
            std::cout << "Error: No response from subnet component for service: " << std::hex << (int)sid << std::endl;
        break;
        case 0x34:
            /* Authentication failed */
            //GenerateFrames::negativeResponse(can_id, sid, nrc);
            std::cout << "Error: Authentication failed for service: " << std::hex << (int)sid << std::endl;
        break;
        case 0x94:
            /* Resource temporarily unavailable */
            //GenerateFrames::negativeResponse(can_id, sid, nrc);
            std::cout << "Error: Resource temporarily unavailable for service: " << std::hex << (int)sid << std::endl;
        break;
        case 0x70:
            /* Upload download not accepted */
            //GenerateFrames::negativeResponse(can_id, sid, nrc);
            std::cout << "Error: Upload download not accepted for service: " << std::hex << (int)sid << std::endl;
        break;
        case 0x71:
            /* Transfer data suspended */
            //GenerateFrames::negativeResponse(can_id, sid, nrc);
            std::cout << "Error: Transfer data suspended for service: " << std::hex << (int)sid << std::endl;
        break;
        default:
            /* Unknown negative response code */
            //GenerateFrames::negativeResponse(can_id, sid, nrc);
            std::cout << "Error: Unknown negative response code for service: " << std::hex << (int)sid << std::endl;
        break;

    }
}