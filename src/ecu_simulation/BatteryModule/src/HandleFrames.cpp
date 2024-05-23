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
            std::cout << "Service 0x22 ReadDataByIdentifierLongResponseFF\n";
            if (isFirstFrame) {
                // Verify that there are enough elements in the data vector
                if (data.size() >= 8) { // Ensure there are at least 5 elements in the data vector
                    // Extract identifier from the data vector
                    int identifier = (data[3] << 8) | data[4];

                    // Extract the remaining data after the identifier
                    std::vector<int> rdata(data.begin() + 5, data.begin() + 8);
                    std::cout << "Valid data length\n";
                    // Store the first part of the data to be concatenated later
                    stored_data = rdata;
                    // Store the expected data size
                    expected_data_size = data[1];
                    //ReadDataByIdentifierLongResponse(id, identifier, rdata, isFirstFrame);
                } else {
                    // Print an error message if there are not enough elements in the data vector
                    std::cerr << "Invalid data length for ReadDataByIdentifierLongResponse\n";
                }
            } else if (isConsecutiveFrame) {
                std::cout << "Service 0x22 ReadDataByIdentifierLongResponseCF\n";
                // Verify that there are enough elements in the data vector
                if (data.size() > 1) { // Ensure there are at least 5 elements in the data vector
                    std::vector<int> rdata(data.begin() + 1, data.end()); // Skip the first byte which is the frame indicator
                    stored_data.insert(stored_data.end(), rdata.begin(), rdata.end());

                    // Once all parts are received, call the appropriate function
                    if (stored_data.size() == expected_data_size) {
                        int identifier = (stored_data[0] << 8) | stored_data[1];
                        std::vector<int> final_data(stored_data.begin() + 2, stored_data.end());
                        // Print a message indicating valid data length
                        std::cout << "Valid data length\n";
                        //ReadDataByIdentifierLongResponse(id, identifier, final_data, false);
                    }
                } else {
                    // Print an error message if there are not enough elements in the data vector
                    std::cerr << "Invalid data length for ReadDataByIdentifierLongResponse\n";
                }
            } else {
                std::cout << "Service 0x22 ReadDataByIdentifier\n";
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
            std::cout << "Service 0x29 AuthenticationSendKey\n";
            if (sid_position + 1 < data.size()) {  
                int sub_function = data[sid_position + 1];
                std::cout << "Valid data length\n";
                std::vector<int> key(data.begin() + sid_position + 2, data.end()); // Extract key from data
                //AuthenticationSendKey(id, key, false); // Call the method to send authentication key
        } else {
            std::cerr << "Invalid data length for AuthenticationSendKey\n";
        } 
        break;
            break;
        case 0x31: // RoutineControl method --to be implemented
            std::cout << "Service 0x31 RoutineControl\n";
            if (sid_position + 1 < data.size()) {  // sub_function data always found next to sid, has to be in valid data range
                int sub_function = data[sid_position + 1];
                std::cout << "Valid data length\n";
                // RoutineControl(id, sub_function, routin_identifier, response);
            } else {
                std::cerr << "Invalid data length for RoutineControl\n";
    }
            break;
        case 0x3E: // TesterPresent method --to be implemented
            std::cout << "Service 0x3E TesterPresent\n";
                if (sid_position + 1 < data.size()) {  // sub_function data always found next to sid, has to be in valid data range
                    int sub_function = data[sid_position + 1];
                    std::cout << "Valid data length\n";
                    // Call the appropriate function based on the sub-function
                    // TesterPresent(id, sub_function);
                } else {
                    std::cerr << "Invalid data length for TesterPresent\n";
                }
            break;
        case 0x23: // ReadMemoryByAddress method --to be implemented
            std::cout << "Service 0x23 ReadMemoryByAddress\n";
            //ReadMemoryByAdress(id, (data[2] << 8) | data[3], data[4], (data[5] << 24) | (data[6] << 16) | (data[7] << 8) | data[8]);
            // ReadMemoryByAdressLongResponse method --to be implemented
            std::cout << "Service 0x23 ReadMemoryByAdressLongResponse\n";
            break;
        case 0x2E: // WriteDataByIdentifier method --to be implemented
            std::cout << "Service 0x2E WriteDataByIdentifier\n";
            //WriteDataByIdentifier(id, (data[2] << 8) | data[3], std::vector<int>(data.begin() + 4, data.end()));
            // WriteDataByIdentifierLongData method --to be implemented
            std::cout << "Service 0x19 WriteDataByIdentifierLongData\n";
            break;
        case 0x19: // ReadDtcInformation method --to be implemented
            std::cout << "Service 0x19 ReadDtcInformation\n";
            if (sid_position + 2 < data.size()) {  // sub_function and dtc_status_mask data always found next to sid, has to be in valid data range
                int sub_function = data[sid_position + 1];
                int dtc_status_mask = data[sid_position + 2];
                std::cout << "Valid data length\n";
                // ReadDtcInformation(id, sub_function, dtc_status_mask);
            } else {
                std::cerr << "Invalid data length for ReadDtcInformation\n";
            }
            break;
        case 0x05:// ReadDtcInformationResponse01 method --to be implemented
            std::cout << "Service 0x29 ReadDtcInformationResponse01\n";
            break;
        case 0x14: // ClearDiagnosticInformation method --to be implemented
            std::cout << "Service 0x14 ClearDiagnosticInfirmationFF\n";
            if (isFirstFrame) {
                // Verify that there are enough elements in the data vector
                if (data.size() >= 2) { // Ensure there are at least 2 elements in the data vector
                    // Extract group_of_dtc from the data vector
                    std::vector<int> group_of_dtc(data.begin() + 2, data.end());

                    std::cout << "Valid data length\n";
                    //ClearDiagnosticInformation(id, group_of_dtc, response);
                } else {
                    std::cerr << "Invalid data length for ClearDiagnosticInformation\n";
                }
            } else if (isConsecutiveFrame) {
                std::cout << "Service 0x14 ClearDiagnosticInformationCF\n";
                // ClearDiagnosticInformation(id, pci, data);
            } else {
                std::cout << "Service 0x14 ClearDiagnosticInformation\n";
                // Process the response frame if needed
            }
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
            // TransferDataLong method --to be implemented
            std::cout << "Service 0x29 TransferDataLong\n";
            std::cout << "Service 0x36 TransferData\n";
            if (sid_position + 2 < data.size()) {  
                int block_sequence_counter = data[sid_position + 1];
                std::cout << "Valid data length\n";
                std::vector<int> transfer_request(data.begin() + sid_position + 2, data.end()); // Extract transfer request from frame
                //TransferData(can_id, block_sequence_counter, transfer_request); // Call the method to transfer data
            } else {
                std::cerr << "Invalid data length for TransferData\n";
            }            
            break;
        case 0x37: // RequestTransferExit method --to be implemented
            std::cout << "Service 0x37 RequestTransferExit\n";
            if (sid_position + 1 < data.size()) {  
                std::cout << "Valid data length\n";
                bool response = data[sid_position + 1];
                //RequestTransferExit(id, response);
            } else {
                std::cerr << "Invalid data length for RequestTransferExit\n";
            }
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