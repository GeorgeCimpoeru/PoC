/*Author:Stoisor Miruna, 2024*/

#include "../include/HandleFrames.h"

/*Services to be included here*/


void HandleFrames::ProcessReceivedFrame(const struct can_frame &frame) {
    int id = frame.can_id;
    std::vector<int> data(frame.data, frame.data + frame.can_dlc);

    // Extract the Service Identifier (SID)
    int sid = data[1];
    int sub_function = data[2];
    switch (sid) {
        //UDS
        case 0x10: // SessionControl method --to be implemented
            if (sub_function == 0x01){
                std::cout << "Sub_function 0x01\n";
                //int resp = //SessionControl(id, data[2]);
            } else if (sub_function == 0x02){
                std::cout << "Sub_function 0x02\n";
                //int resp = //SessionControl(id, data[2]);
            } else {
                std::cerr << "Unknown sub_function: " << std::hex << sub_function << std::dec << "\n";
            }
            break;
        case 0x11: // EcuReset
            std::cout << "Service 0x11\n";
            //EcuReset(id);
            break;
        case 0x22: // ReadDataByIdentifier method --to be implemented
            std::cout << "Service 0x22\n";
            //ReadDataByIdentifier(id, data[2] * 256 + data[3]);
            break;
        case 0x27: // AuthenticationRequestSeed method --to be implemented
            if (data.size() > 2) {
                std::cout << "Service 0x27-n\n";
                //AuthenticationRequestSeed(id, true, std::vector<int>(data.begin() + 2, data.end()));
            } else {
                std::cout << "Service 0x27-f\n";
                //AuthenticationRequestSeed(id);
            }
            break;
        case 0x29: // AuthenticationSendKey method --to be implemented
            std::cout << "Service 0x29\n";
            //AuthenticationSendKey(frame.can_id, std::vector<int>(data.begin() + 2, data.end()));
            break;
        case 0x31: // RoutineControl method --to be implemented
            std::cout << "Service 0x31\n";
            //RoutineControl(id);
            break;
        case 0x3E: // TesterPresent method --to be implemented
            std::cout << "Service 0x3E\n";
            //TesterPresent(id);
            break;
        case 0x23: // ReadMemoryByAddress method --to be implemented
            std::cout << "Service 0x23\n";
            //ReadMemoryByAdress(id, (data[2] << 8) | data[3], data[4], (data[5] << 24) | (data[6] << 16) | (data[7] << 8) | data[8]);
            break;
        case 0x2E: // WriteDataByIdentifier method --to be implemented
            std::cout << "Service 0x2E\n";
            //WriteDataByIdentifier(id, (data[2] << 8) | data[3], std::vector<int>(data.begin() + 4, data.end()));
            break;
        case 0x19: // ReadDtcInformation method --to be implemented
            std::cout << "Service 0x19\n";
            //ReadDtcInformation(id, data[0] == 0x59);
            break;
        case 0x14: // ClearDiagnosticInfirmation method --to be implemented
            std::cout << "Service 0x14\n";
            //ClearDiagnosticInfirmation(id, data[0] == 0x54);
            break;
        case 0x83: // AccessTimingParameters method --to be implemented
            std::cout << "Service 0x83\n";
            //AccessTimingParameters(id, data[0] == 0xC3);
            break;
        case 0x7F: // NegativeResponse method --to be implemented
            std::cout << "Service 0x7F\n";
            //NegativeResponse(id, data[0], data[1] == 0x7F);
            break;
        // OTA
        case 0x34: // RequestDownload method --to be implemented
            std::cout << "Service 0x34\n";
            //RequestDownload(id, data[2], data[3], (data[4] << 24) | (data[5] << 16) | (data[6] << 8) | data[7], (data[8] << 24) | (data[9] << 16) | (data[10] << 8) | data[11]);
            break;
        case 0x36: // TransferData method --to be implemented
            std::cout << "Service 0x36\n";
            //TransferData(id, (data[2] << 8) | data[3], data[4]);
            break;
        case 0x37: // RequestTransferExit method --to be implemented
            std::cout << "Service 0x37\n";
            //RequestTransferExit(id, data[2]);
            break;
        case 0x32: // RequestUpdateStatus method --to be implemented
            std::cout << "Service 0x32\n";
            //RequestUpdateStatus(id);
            break;
        default:
            std::cerr << "Unknown service" << std::endl;
            break;
    }
}