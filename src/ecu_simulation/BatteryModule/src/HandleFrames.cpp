/*Author:Stoisor Miruna, 2024*/

#include "../include/HandleFrames.h"
#include "../include/GenerateFrames.h"
#include "../include/ReceiveFrames.h"
/*Services to be included here*/


void HandleFrame::ProcessReceivedFrame(const struct can_frame &frame) {
    int id = frame.can_id;
    std::vector<int> data(frame.data, frame.data + frame.can_dlc);

    // Extract the Service Identifier (SID)
    int sid = data[1];

    switch (sid) {
        //UDS
        case 0x10: // SessionControl method --to be implemented
            int sub_function = data[2];
            if (sub_function == 0x01){
                int resp = //SessionControl(id, data[2]);
            }else if (sub_function == 0x02){
                int resp = //SessionControl(id, data[2]);
            } else {
                std::cerr << "Unknown sub_function: " << std::hex << sub_function << std::dec << "\n";
            }
            break;
        case 0x11: // EcuReset
            //EcuReset(id);
            break;
        case 0x22: // ReadDataByIdentifier method --to be implemented
            //ReadDataByIdentifier(id, data[2] * 256 + data[3]);
            break;
        case 0x27: // AuthenticationRequestSeed method --to be implemented
            if (data.size() > 2) {
                //AuthenticationRequestSeed(id, true, std::vector<int>(data.begin() + 2, data.end()));
            } else {
                //AuthenticationRequestSeed(id);
            }
            break;
        case 0x29: // AuthenticationSendKey method --to be implemented
            //AuthenticationSendKey(frame.can_id, std::vector<int>(data.begin() + 2, data.end()));
            break;
        case 0x31: // RoutineControl method --to be implemented
            //RoutineControl(id);
            break;
        case 0x3E: // TesterPresent method --to be implemented
            //TesterPresent(id);
            break;
        case 0x23: // ReadMemoryByAddress method --to be implemented
            //ReadMemoryByAdress(id, (data[2] << 8) | data[3], data[4], (data[5] << 24) | (data[6] << 16) | (data[7] << 8) | data[8]);
            break;
        case 0x2E: // WriteDataByIdentifier method --to be implemented
            //WriteDataByIdentifier(id, (data[2] << 8) | data[3], std::vector<int>(data.begin() + 4, data.end()));
            break;
        case 0x19: // ReadDtcInformation method --to be implemented
            //ReadDtcInformation(id, data[0] == 0x59);
            break;
        case 0x14: // ClearDiagnosticInfirmation method --to be implemented
            //ClearDiagnosticInfirmation(id, data[0] == 0x54);
            break;
        case 0x83: // AccessTimingParameters method --to be implemented
            //AccessTimingParameters(id, data[0] == 0xC3);
            break;
        case 0x7F: // NegativeResponse method --to be implemented
            //NegativeResponse(id, data[0], data[1] == 0x7F);
            break;
        // OTA
        case 0x34: // RequestDownload method --to be implemented
            //RequestDownload(id, data[2], data[3], (data[4] << 24) | (data[5] << 16) | (data[6] << 8) | data[7], (data[8] << 24) | (data[9] << 16) | (data[10] << 8) | data[11]);
            break;
        case 0x36: // TransferData method --to be implemented
            //TransferData(id, (data[2] << 8) | data[3], data[4]);
            break;
        case 0x37: // RequestTransferExit method --to be implemented
            //RequestTransferExit(id, data[2]);
            break;
        case 0x32: // RequestUpdateStatus method --to be implemented
            //RequestUpdateStatus(id);
            break;
        default:
            std::cerr << "Unknown service" << std::endl;
            break;
    }
}