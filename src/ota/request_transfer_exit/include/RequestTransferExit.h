/**
 * @brief This class is used to manage the service request transfer exit 0x37
 *        The method 
 * @file RequestTransferExit.h
 * @author Loredana Dugulean
 * @date 2024-05-20
 *
*/

#ifndef REQUEST_TRANSFER_EXIT_H_
#define REQUEST_TRANSFER_EXIT_H_

#include <iostream>
#include <vector>
#include <functional>

#include "src/utils/include/Logger.h"
#include "src/utils/include/GenerateFrames.h"

/* Define the callback function type */
using transferCompleteCallBack = std::function<bool(bool)>;

class RequestTransferExit
{
public:    
    /**
    * @brief Constructor for transfer exit object
    * @param RTESLogger local reference to the parent logger
    */
    RequestTransferExit(int socket, Logger& RTESLogger);

    /**
   * @brief Destructor.
   */
    ~RequestTransferExit();

    /**
     * @brief Method to set the callback function.    
     * @param[in] callback A function to be called when the transfer is completed.
    */
    void setTransferCompleteCallBack(transferCompleteCallBack callback );

    /** Method to handle the request transfer exit 0x37.
    *   This method will invoke the callback with the result of the transfer
    *   and return true if the transfer should continue or false if it should stop
    *   @param[in] transferSucces A boolean that indicates if the transfer was successfull.
    *   @return Returns true if the transfer should continue, false if it should stop.
    */
    bool requestTransferExit(int id, bool transferSuccess);

     /**
     * @brief method used to facilitate the transfer exit
     * @param can_id frame id that contains the sender and receiver
     * @param request_transfer_exit_data Data from a can frame that contains PCI, SID and transfer request parameter record
     */
    void requestTRansferExitRequest(canid_t can_id, const std::vector<uint8_t>& request_transfer_exit_data);

private:
    /* Member variable to store the callback function */
    transferCompleteCallBack callback_;  

    Logger& RTESLogger;  
    GenerateFrames generate_frames;
    int socket = -1;  
};

#endif /* REQUEST_TRANSFER_EXIT_H_ */