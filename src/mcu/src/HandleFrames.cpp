/**
 * @brief 
 * @file HandleFrames.cpp
 * @author Tanasevici Mihnea
 * @date 2024-05-20
 * 
 */
#include "../include/HandleFrames.h"

/* Method to handle a can_frame*/
void HandleFrames::handleFrame(const struct can_frame &frame) {
    /* get frame id */
    int frame_id = frame.can_id;
    /* get data field */
    std::vector<int> data(frame.data, frame.data + frame.can_dlc);
    /* get Service ID */
    int sid = data[1];

    switch (sid) {
    /* UDS Requests */
        case 0x10:
            /* DiagnosticSessionControl(); */
            std::cout << "DiagnosticSessionControl called." << std::endl;
            break;
        case 0x11:
            /* EcuReset(); */
            break;
        case 0x27:
            /* SecurityAccess(); */
            break;
        case 0x29:
            /* Authentication(); */
            break;
        case 0x3E:
            /* TesterPresent(); */
            break;
        case 0x83:
            /* AccessTimingParameters(); */
            break;
        case 0x22:
            /* ReadDataByIdentifier(); */
            break;
        case 0x23:
            /* ReadMemoryByAddress(); */
            break;
        case 0x2E:
            /* WriteDataByIdentifier(); */
            break;
        case 0x14:
            /* ClearDiagnosticInformation(); */
            break;
        case 0x19:
            /* ReadDtcInformation(); */
            break;
        case 0x31:
            /* RoutineControl(); */
            break;

        /* OTA Requests */
        case 0x34:
            /* RequestDownload(); */
            std::cout << "Request download called." << std::endl;
            break;
        case 0x36:
            /* TransferData(); */
            break;
        case 0x37:
            /* RequestTransferExit(); */
            break;
        case 0x32:
            /* RequestUpdateStatus(); */
            break;
        default:
            std::cerr << "Unknown service." << std::endl;
            break;
    }
} 