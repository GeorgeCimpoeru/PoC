/*The HandleFrames module processes received CAN frames, extracting relevant data and 
invoking appropriate service methods for further handling. Upon reception, it analyzes the 
frame to determine its type (e.g., first frame, consecutive frame, or single frame) 
and extracts the Service Identifier (SID) for service identification. Based on the SID, 
it directs the frame to corresponding methods for further processing. The module ensures completeness 
of multi-frame messages and handles single-frame messages accordingly. */

/*Author:Stoisor Miruna, 2024*/

#ifndef HANDLE_FRAME_H_
#define HANDLE_FRAME_H_

#include <vector>
#include <iostream>
#include <linux/can.h>
#include <algorithm>
#include <iterator>

class HandleFrames 
{
private:
    
    std::vector<int> stored_data;                /*Vector to store received data*/ 
    size_t expected_data_size;                   /*Expected total size of data*/ 
    size_t sid_position;                         /*Position of the Service Identifier (SID) in the received data*/ 
    int flag;                                    /*Flag to track the number of consecutive frames expected*/ 
    int pci;                                     /*Protocol Control Information (PCI) byte*/ 
    int sid;                                     /*Service Identifier (SID)*/ 
    bool isFirstFrame;                           /*Flag indicating if the frame is the first frame of a multi-frame message*/ 
    bool isConsecutiveFrame;                     /*Flag indicating if the frame is a consecutive frame of a multi-frame message*/ 
    bool isSingleFrame;                          /*Flag indicating if the frame is a single frame message*/ 
public:
    HandleFrames() : expected_data_size(0), flag(0) {}
    
    void processReceivedFrame(const struct can_frame &frame);
    void handleCompleteData(int id, const std::vector<int>& data, bool isSingleFrame);
};

#endif // HANDLE_FRAME_H_