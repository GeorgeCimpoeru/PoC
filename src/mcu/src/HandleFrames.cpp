/**
 * @brief 
 * @file HandleFrames.cpp
 * @author Tanasevici Mihnea
 * @date 2024-05-20
 * 
 */
#include "../include/HandleFrames.h"

void HandleFrames::HandleFrame(const struct can_frame &frame) {
    int frame_id = frame.can_id;
    std::vector<int> data(frame.data, frame.data + frame.can_dlc);
    // get Service ID
    int sid = data[1];

    switch (sid) {
        //UDS requests
        case 0x10:
            //DiagnosticSessionControl();
            break;
        case 0x11:
            //EcuReset();
            break;
        case 0x27:
            //SecurityAccess();
            break;
        case 0x29: 
            //Authentication();
            break;
        case 0x3E:
            //TesterPresent();
            break;
        case 0x83:
            //AccessTimingParameters();
            break;
        case 0x22:
            //ReadDataByIdentifier();
            break;
        case 0x23:
            //ReadMemoryByAdress();
            break;
        case 0x2E:
            //WriteDataByIdentifier();
            break;
        case 0x14:
            //ClearDiagnosticInformation();
            break;
        case 0x19:
            //ReadDtcInformation();
            break;
        case 0x31:
            //RoutineControl();
            break;
        //UDS responses
        case 0x50: // SID + 0x40 DiagnosticSessionControl response
            //SendToApi();
            break;
        case 0x51: // SID + 0x40 EcuReset response
            //SendToApi();
            break;
        case 0x67: // SID + 0x40 Security Access response
            //SendToApi();
            break;
        case 0x69: // SID + 0x40 Authentication response
            //SendToApi();
            break;
        case 0x7E: // SID + 0x40 TesterPresent response
            //SendToApi();
            break;
        case 0xC3: // SID + 0x40 AccessTimingParameters response
            //SendToApi();
            break;
        case 0x62: // SID + 0x40 ReadDataByIdentifier response
            //SendToApi();
            break;
        case 0x63: // SID + 0x40 ReadMemoryByAdress response
            //SendToApi();
            break;
        case 0x6E: // SID + 0x40 WriteDataByIdentifier response
            //SendToApi();
            break;
        case 0x54: // SID + 0x40 ClearDiagnosticInformation response
            //SendToApi();
            break;
        case 0x59: // SID + 0x40 ReadDtcInformation response
            //SendToApi();
            break;
        case 0x71: // SID + 0x40 RoutineControl response
            //SendToApi();
            break;
        case 0x7F: // Negative response
            //SendToApi();
            break;
        //OTA requests
        case 0x34:
            //RequestDownload();
            break;
        case 0x36:
            //TransferData();
            break;
        case 0x37:
            //RequestTransferExit();
            break;
        case 0x32:
            //RequestUpdateStatus();
            break;
        //OTA responses
        case 0x74: // SID + 0x40 RequestDownload response
            //SendToApi();
            break;
        case 0x76: // SID + 0x40 TransferData response
            //SendToApi();
            break;
        case 0x77: // SID + 0x40 RequestTransferExit response
            //SendToApi();
            break;
        case 0x72: // SID + 0x40 RequestUpdateStatus response
            //SendToApi();
            break;
        default:
            std::cerr << "Unknown service" << std::endl;
            break;
    }
    std::cout<<"Frame processed\n";
}