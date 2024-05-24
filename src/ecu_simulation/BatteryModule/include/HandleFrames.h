/*Author:Stoisor Miruna, 2024*/

#ifndef HANDLE_FRAME_H_
#define HANDLE_FRAME_H_

#include <vector>
#include <iostream>
#include <linux/can.h>
#include <algorithm>
#include <iterator>

class HandleFrames {
private:
    // Reference to Service instance
    std::vector<int> stored_data;
    size_t expected_data_size;
    size_t sid_position;
    int flag;
    int pci;
    int sid;
    bool isFirstFrame;
    bool isConsecutiveFrame;
    bool isSingleFrame;
public:
    HandleFrames() : expected_data_size(0), flag(0) {}
    
    void processReceivedFrame(const struct can_frame &frame);
    void handleCompleteData(int id, const std::vector<int>& data, bool isSingleFrame);
};

#endif // HANDLE_FRAME_H_