/*Author:Stoisor Miruna, 2024*/

#include "../include/HandleFrames.h"


/*Services to be included here*/

void HandleFrames::ProcessReceivedFrame(const struct can_frame &frame) {
    int id = frame.can_id;
    std::vector<int> data(frame.data, frame.data + frame.can_dlc);

    // Extract the PCI byte
    int pci = data[0];
    bool isFirstFrame = (pci & 0xF0) == 0x10;
    bool isConsecutiveFrame = (pci & 0xF0) == 0x20;

    // Extract the Service Identifier (SID)
    int sid = -1;
    size_t sid_position = -1;

    if (isFirstFrame) {
        // For First Frame, the SID is at a specific position
        sid = data[2];
        sid_position = 2;
    } else if (!isConsecutiveFrame) {
        // For Single Frame, search for the SID normally
        for (size_t i = 0; i < data.size(); ++i) {
            if (data[i] >= 0x10) { // Assuming SID values are always greater than 0x10
                sid = data[i];
                sid_position = i;
                break;
            }
        }

    if (sid == -1) {
        std::cerr << "No valid SID found in frame\n";
        return;
    }

    std::cout << "\nSID: 0x" << std::hex << sid << " found at position: " << sid_position << std::dec << "\n";

    switch (sid) {
        //UDS
        case 0x10: // SessionControl method --to be implemented
            std::cout << "Service 0x10 SessionControl\n";
            if (sid_position + 1 < data.size()) {  //sub_function data always found next to sid, has to be in valid data range
                int sub_function = data[sid_position + 1];
                std::cout << "Valid data length\n";
                //SessionControl(id, sub_function);
            } else {
                std::cerr << "Invalid data length for SessionControl\n";
            }          
            break;
        case 0x11: // EcuReset
            std::cout << "Service 0x11 EcuReset\n";
            if (sid_position + 1 < data.size()) {  //sub_function data always found next to sid, has to be in valid data range
                int sub_function = data[sid_position + 1];
                std::cout << "Valid data length\n";
                //EcuReset(id, sub_function);
            } else {
                std::cerr << "Invalid data length for SessionControl\n";
            }  
            break;
        case 0x22: // ReadDataByIdentifier method --to be implemented
            std::cout << "Service 0x22 ReadDataByIdentifier\n";
            if (isFirstFrame) {
                //ReadDataByIdentifierLongResponse(id, data, sid_position);
            } else if (isConsecutiveFrame) {
                //HandleConsecutiveFrame(id, pci, data);
            } else {
                if (sid_position + 3 < data.size()) {  //sub_function data always found next to sid, has to be in valid data range
                int identifier = (data[sid_position + 1] << 8) | data[sid_position + 2];
                std::vector<int> rdata(data.begin() + sid_position + 3, data.end());
                std::cout << "Valid data length\n";
                //ReadDataByIdentifier(id, identifier, rdata);
                //ReadDataByIdentifier(id, data[2] * 256 + data[3]);
                } else {
                    std::cerr << "Invalid data length for SessionControl\n";
                } 
            }
            break;
        case 0x03:// ReadMemoryByAdressLongResponse method --to be implemented
            std::cout << "Service 0x29 ReadMemoryByAdressLongResponse\n";
            break;
        case 0x27: // AuthenticationRequestSeed method --to be implemented
            if (data.size() > 2) {
                std::cout << "Service 0x27-n AuthenticationRequestSeed\n";
                //AuthenticationRequestSeed(id, true, std::vector<int>(data.begin() + 2, data.end()));
            } else {
                std::cout << "Service 0x27-f AuthenticationRequestSeed\n";
                //AuthenticationRequestSeed(id);
            }
            break;
        case 0x29: // AuthenticationSendKey method --to be implemented
            std::cout << "Service 0x29 AuthenticationSendKey\n";
            //AuthenticationSendKey(frame.can_id, std::vector<int>(data.begin() + 2, data.end()));
            break;
        case 0x31: // RoutineControl method --to be implemented
            std::cout << "Service 0x31 RoutineControl\n";
            //RoutineControl(id);
            break;
        case 0x3E: // TesterPresent method --to be implemented
            std::cout << "Service 0x3E TesterPresent\n";
            //TesterPresent(id);
            break;
        case 0x23: // ReadMemoryByAddress method --to be implemented
            std::cout << "Service 0x23 ReadMemoryByAddress\n";
            //ReadMemoryByAdress(id, (data[2] << 8) | data[3], data[4], (data[5] << 24) | (data[6] << 16) | (data[7] << 8) | data[8]);
            break;
        case 0x2E: // WriteDataByIdentifier method --to be implemented
            std::cout << "Service 0x2E WriteDataByIdentifier\n";
            //WriteDataByIdentifier(id, (data[2] << 8) | data[3], std::vector<int>(data.begin() + 4, data.end()));
            break;
        case 0x04:// WriteDataByIdentifierLongData method --to be implemented
            std::cout << "Service 0x19 WriteDataByIdentifierLongData\n";
            break;
        case 0x19: // ReadDtcInformation method --to be implemented
            std::cout << "Service 0x19 ReadDtcInformation\n";
            //ReadDtcInformation(id, data[0] == 0x59);
            break;
        case 0x05:// ReadDtcInformationResponse01 method --to be implemented
            std::cout << "Service 0x29 ReadDtcInformationResponse01\n";
            break;
        case 0x14: // ClearDiagnosticInfirmation method --to be implemented
            std::cout << "Service 0x14 ClearDiagnosticInfirmation\n";
            //ClearDiagnosticInfirmation(id, data[0] == 0x54);
            break;
        case 0x83: // AccessTimingParameters method --to be implemented
            std::cout << "Service 0x83 AccessTimingParameters\n";
            //AccessTimingParameters(id, data[0] == 0xC3);
            break;
        case 0x7F: // NegativeResponse method --to be implemented
            std::cout << "Service 0x7F NegativeResponse\n";
            //NegativeResponse(id, data[0], data[1] == 0x7F);
            break;
        // OTA
        case 0x34: // RequestDownload method --to be implemented
            std::cout << "Service 0x34 RequestDownload\n";
            //RequestDownload(id, data[2], data[3], (data[4] << 24) | (data[5] << 16) | (data[6] << 8) | data[7], (data[8] << 24) | (data[9] << 16) | (data[10] << 8) | data[11]);
            break;
        case 0x36: // TransferData method --to be implemented
            std::cout << "Service 0x36 TransferData\n";
            //TransferData(id, (data[2] << 8) | data[3], data[4]);
            break;
        case 0x06:// TransferDataLong method --to be implemented
            std::cout << "Service 0x29 TransferDataLong\n";
            break;
        case 0x37: // RequestTransferExit method --to be implemented
            std::cout << "Service 0x37 RequestTransferExit\n";
            //RequestTransferExit(id, data[2]);
            break;
        case 0x32: // RequestUpdateStatus method --to be implemented
            std::cout << "Service 0x32 RequestUpdateStatus\n";
            //RequestUpdateStatus(id);
            break;
        default:
            std::cerr << "Unknown service" << std::endl;
            break;
    }
}
}