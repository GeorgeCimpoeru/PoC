/*Author:Stoisor Miruna, 2024*/

#include "../include/HandleFrames.h"


/*Services to be included here*/

/*Method for processing received CAN frame*/ 
void HandleFrames::processReceivedFrame(const struct can_frame &frame) 
{
    int id = frame.can_id;
    std::vector<int> data(frame.data, frame.data + frame.can_dlc);

    /*Extract the PCI byte*/ 
    pci = data[0];
    isFirstFrame = (pci & 0xF0) == 0x10;
    isConsecutiveFrame = (pci & 0xF0) == 0x20;
    isSingleFrame = !isFirstFrame && !isConsecutiveFrame;

    /*Extract the Service Identifier (SID)*/ 
    sid = -1;
    sid_position = -1;

    /*Handle First Frame*/ 
    if (isFirstFrame) 
    {
        std::cout << "\nFirst frame received\n";
        stored_data = std::vector<int>(frame.data, frame.data + frame.can_dlc); /*Store initial data including PCI byte*/ 
        expected_data_size = ((data[1] & 0x0F) << 8) + data[2]; /*Expected total size of data*/ 
        std::cout << "data size " << expected_data_size << std::endl;
        flag = expected_data_size / 7 + (expected_data_size % 7 != 0); /*Calculate the number of consecutive frames expected*/ 
        std::cout << "no of frames " << flag << std::endl;

        /*SID is at position 1 in the data vector after excluding PCI byte*/ 
        sid = data[2];
        sid_position = 2;
    } 
    /*Handle Consecutive Frames*/ 
    else if (isConsecutiveFrame) 
    {
        std::cout << "\nConsecutive frame received\n";
        stored_data.insert(stored_data.end(), data.begin()+1, data.end()); /*Concatenate data excluding PCI byte*/ 
        flag--;
        std::cout << "no of frames- " << flag << std::endl;
    }
    /*Handle Single Frames*/ 
    else 
    {
        /*Search for SID normally*/ 
        std::cout << "\nSingle frame received\n";
        for (size_t i = 0; i < data.size(); ++i) 
        {
            if (data[i] >= 0x10) 
            { /*Assuming SID values are always greater than 0x10*/ 
                sid = data[i];
                sid_position = i;
                break;
            }
        }
        stored_data = data; /*Store the data including the PCI byte*/ 
        flag = 0;

        // /*SID is at position 0 in the data vector after excluding PCI byte*/
        /*sid = data[0];*/ 
        /*sid_position = 0;*/ 
    }

    /*If all frames have been received or it's a single frame*/ 
    if (flag == 0) 
    {
        std::cout << "am intrat\n";
        if (isFirstFrame && isConsecutiveFrame) {
            std::cout << "Complete multi-frame message received\n";
            handleCompleteData(id, stored_data, isSingleFrame); /*Process the complete data*/ 
            stored_data.clear();
            expected_data_size = 0;
        } else {
            std::cout << "Single frame message received\n";
            handleCompleteData(id, stored_data, isSingleFrame); /*Process the complete data*/ 
            stored_data.clear();
            expected_data_size = 0;
        }
        
    }

    if (sid == -1) 
    {
        std::cerr << "No valid SID found in frame\n";
        return;
    }
    std::cout << "SID: 0x" << std::hex << sid << " found at position: " << sid_position << std::dec << "\n"<<std::flush;
}

/*Method for handling complete data after reception*/
void HandleFrames::handleCompleteData(int id,const std::vector<int>& data, bool isSingleFrame) 
{
    /*Extract the Service Identifier (SID)*/ 
    std::cout << "Handling complete data for SID: 0x" << std::hex << sid << std::dec << "\n";

    switch (sid) 
    {
        /*UDS*/
        case 0x10: /*SessionControl method --to be implemented*/ 
            std::cout << "Service 0x10 SessionControl\n"<<std::flush;
            std::cout << "SID pos: " << sid_position << std::endl;
            std::cout << "Data size: " << data.size() << std::endl;
            if (sid_position + 1 < data.size()) 
            {  /*sub_function data always found next to sid, has to be in valid data range*/
                int sub_function = data[sid_position + 1];
                std::cout << "Valid data length\n"<<std::flush;
                /*sessionControl(id, sub_function);*/
            } else 
            {
                std::cerr << "Invalid data length for SessionControl\n";
            }         
            break;
        case 0x11: /*EcuReset*/ 
            std::cout << "Service 0x11 EcuReset\n";
            if (sid_position + 1 < data.size()) 
            {  /*sub_function data always found next to sid, has to be in valid data range*/
                int sub_function = data[sid_position + 1];
                std::cout << "Valid data length\n";
                /*ecuReset(id, sub_function);*/
            } else 
            {
                std::cerr << "Invalid data length for SessionControl\n";
            }  
            break;
        case 0x22: /*ReadDataByIdentifier method --to be implemented*/ 
            std::cout << "Service 0x22 ReadDataByIdentifier\n";
            std::cout << "SID pos: " << sid_position << std::endl;
            std::cout << "Data size: " << data.size() << std::endl;
            
            if (stored_data.size() >= 3) 
            {
                int identifier = (stored_data[1] << 8) | stored_data[2];
                std::cout << "Stored data: " << identifier << std::endl;
                std::cout << "Valid data length\n";
                std::vector<int> rdata(stored_data.begin() + 3, stored_data.end());
                if (isSingleFrame) 
                {
                    std::cout << "Service 0x22 ReadDataByIdentifierSingleFrame\n";
                    /*readDataByIdentifier(id, identifier, rdata);*/
                } else 
                {
                    std::cout << "Service 0x22 ReadDataByIdentifierMoreFrames\n";
                    /*readDataByIdentifierLongResponse(id, identifier, rdata, false);*/
                }
            } else 
            {
                std::cerr << "Invalid data length for ReadDataByIdentifier\n";
            }
            break;
        case 0x27: /*AuthenticationRequestSeed method --to be implemented*/ 
            if (data.size() > 2) 
            {
                std::cout << "Service 0x27-n AuthenticationRequestSeed\n";
                /*authenticationRequestSeed(id, true, std::vector<int>(data.begin() + 2, data.end()));*/
            } else 
            {
                std::cout << "Service 0x27-f AuthenticationRequestSeed\n";
                /*authenticationRequestSeed(id);*/
            }
            break;
        case 0x29: /*AuthenticationSendKey method --to be implemented*/ 
            std::cout << "Service 0x29 AuthenticationSendKey\n";
            /*authenticationSendKey(frame.can_id, std::vector<int>(data.begin() + 2, data.end()));*/
            std::cout << "Service 0x29 AuthenticationSendKey\n";
            if (sid_position + 1 < data.size()) 
            {  
                int sub_function = data[sid_position + 1];
                std::cout << "Valid data length\n";
                std::vector<int> key(data.begin() + sid_position + 2, data.end()); /*Extract key from data*/ 
                /*authenticationSendKey(id, key, false); // Call the method to send authentication key*/
            } else 
            {
            std::cerr << "Invalid data length for AuthenticationSendKey\n";
            } 
            break;
        case 0x31: /*RoutineControl method --to be implemented*/ 
            std::cout << "Service 0x31 RoutineControl\n";
            if (sid_position + 1 < data.size()) 
            {  /*sub_function data always found next to sid, has to be in valid data range*/
                int sub_function = data[sid_position + 1];
                std::cout << "Valid data length\n";
                /*routineControl(id, sub_function, routin_identifier, response);*/ 
            } else 
            {
                std::cerr << "Invalid data length for RoutineControl\n";
            }
            break;
        case 0x3E: /*TesterPresent method --to be implemented*/ 
            std::cout << "Service 0x3E TesterPresent\n";
            if (sid_position + 1 < data.size()) 
            {  /*sub_function data always found next to sid, has to be in valid data range*/
                int sub_function = data[sid_position + 1];
                std::cout << "Valid data length\n";
                /*Call the appropriate function based on the sub-function*/ 
                /*testerPresent(id, sub_function);*/ 
            } else 
            {
                std::cerr << "Invalid data length for TesterPresent\n";
            }
            break;
        case 0x23: /*ReadMemoryByAddress method --to be implemented*/ 
            std::cout << "Service 0x23 ReadMemoryByAddress\n";
            /*readMemoryByAdress(id, (data[2] << 8) | data[3], data[4], (data[5] << 24) | (data[6] << 16) | (data[7] << 8) | data[8]);*/
            /*readMemoryByAdressLongResponse method --to be implemented*/ 
            std::cout << "Service 0x23 ReadMemoryByAdressLongResponse\n";
            break;
        case 0x2E: /*WriteDataByIdentifier method --to be implemented*/ 
            std::cout << "Service 0x2E WriteDataByIdentifier\n";
            /*writeDataByIdentifier(id, (data[2] << 8) | data[3], std::vector<int>(data.begin() + 4, data.end()));*/
            /*writeDataByIdentifierLongData method --to be implemented*/ 
            std::cout << "Service 0x19 WriteDataByIdentifierLongData\n";
            break;
        case 0x19: /*ReadDtcInformation method --to be implemented*/ 
            std::cout << "Service 0x19 ReadDtcInformation\n";
            if (sid_position + 2 < data.size()) 
            {  /*sub_function and dtc_status_mask data always found next to sid, has to be in valid data range*/ 
                int sub_function = data[sid_position + 1];
                int dtc_status_mask = data[sid_position + 2];
                std::cout << "Valid data length\n";
                /*readDtcInformation(id, sub_function, dtc_status_mask);*/ 
            } else 
            {
                std::cerr << "Invalid data length for ReadDtcInformation\n";
            }
            break;
        case 0x05:/*ReadDtcInformationResponse01 method --to be implemented*/ 
            std::cout << "Service 0x29 ReadDtcInformationResponse01\n";
            break;
        case 0x14: /*ClearDiagnosticInformation method --to be implemented*/ 
            std::cout << "Service 0x14 ClearDiagnosticInfirmationFF\n";
            if (isFirstFrame) 
            {
                /*Verify that there are enough elements in the data vector*/ 
                if (data.size() >= 2) 
                { /*Ensure there are at least 2 elements in the data vector*/ 
                    /*Extract group_of_dtc from the data vector*/ 
                    std::vector<int> group_of_dtc(data.begin() + 2, data.end());

                    std::cout << "Valid data length\n";
                    /*clearDiagnosticInformation(id, group_of_dtc, response);*/
                } else 
                {
                    std::cerr << "Invalid data length for ClearDiagnosticInformation\n";
                }
            } else if (isConsecutiveFrame) 
            {
                std::cout << "Service 0x14 ClearDiagnosticInformationCF\n";
                /*clearDiagnosticInformation(id, pci, data);*/ 
            } else 
            {
                std::cout << "Service 0x14 ClearDiagnosticInformation\n";
                /*Process the response frame if needed*/ 
            }
            break;
        case 0x83: /*AccessTimingParameters method --to be implemented*/ 
            std::cout << "Service 0x83 AccessTimingParameters\n";
            /*accessTimingParameters(id, data[0] == 0xC3);*/
            break;
        case 0x7F: /*NegativeResponse method --to be implemented*/ 
            std::cout << "Service 0x7F NegativeResponse\n";
            /*negativeResponse(id, data[0], data[1] == 0x7F);*/
            break;
        // OTA
        case 0x34: /*RequestDownload method --to be implemented*/ 
            std::cout << "Service 0x34 RequestDownload\n";
            /*requestDownload(id, data[2], data[3], (data[4] << 24) | (data[5] << 16) | (data[6] << 8) | data[7], (data[8] << 24) | (data[9] << 16) | (data[10] << 8) | data[11]);*/
            break;
        case 0x36: /*TransferData method --to be implemented*/ 
            std::cout << "Service 0x29 TransferDataLong\n";
            std::cout << "Service 0x36 TransferData\n";
            if (sid_position + 2 < data.size()) 
            {  
                int block_sequence_counter = data[sid_position + 1];
                std::cout << "Valid data length\n";
                std::vector<int> transfer_request(data.begin() + sid_position + 2, data.end()); /*Extract transfer request from frame*/ 
                /*transferData(can_id, block_sequence_counter, transfer_request); // Call the method to transfer data*/
            } else 
            {
                std::cerr << "Invalid data length for TransferData\n";
            }            
            break;
        case 0x37: /*RequestTransferExit method --to be implemented*/ 
            std::cout << "Service 0x37 RequestTransferExit\n";
            if (sid_position + 1 < data.size()) 
            {  
                std::cout << "Valid data length\n";
                bool response = data[sid_position + 1];
                /*requestTransferExit(id, response);*/
            } else 
            {
                std::cerr << "Invalid data length for RequestTransferExit\n";
            }
            /*requestTransferExit(id, data[2]);*/
            break;
        case 0x32: /*requestUpdateStatus method --to be implemented*/ 
            std::cout << "Service 0x32 RequestUpdateStatus\n";
            /*requestUpdateStatus(id);*/
            break;
        default:
            std::cerr << "Unknown service" << std::endl;
            break;
    }
}
