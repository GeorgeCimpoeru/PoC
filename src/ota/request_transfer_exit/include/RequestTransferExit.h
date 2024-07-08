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

#include "../../utils/include/Logger.h"
#include "../../../utils/include/CreateInterface.h"
#include "../../../utils/include/GenerateFrames.h"
#include "../../../ecu_simulation/BatteryModule/include/BatteryModule.h"

/* Define the callback function type */
using transferCompleteCallBack = std::function<bool(bool)>;

class RequestTransferExit
{
public:
    /**
   * @brief Constructor.   
   */
    /* constructor to initialize the transfer exit */
    RequestTransferExit(Logger& RTESLogger);

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
    bool requestTransferExit(int id, std::vector<uint8_t>stored_data, Logger& RTESlogger,bool transferSucces);
   
    /**
     * @brief Response method from Request Transfer Exit service 
     *
     * @param id
     * @param RTESlogger
     */
    void requestTransferExitResponse(int id, Logger& RTESlogger);

private:
    /* Member variable to store the callback function */
    transferCompleteCallBack callback_;
    
    GenerateFrames generate_frames;
};

#endif /* REQUEST_TRANSFER_EXIT_H_ */