/**
 * @brief This class is used to manage the service request transfer data 0x36
 *        The method 
 * @file transfer_data.h
 * @author Avramescu Anamaria
 * @date 2024-06-26
 *
*/

#ifndef TRANSFER_DATA_H_
#define TRANSFER_DATA_H_

#include <iostream>
#include <vector>
#include <functional>
#include "../../../utils/include/Logger.h"
// #include "../../../mcu/include/MCULogger.h"
#include "../include/RequestTransferExit.h" 

/* Define the callback function type */
using transferDataCallBack = std::function<bool(bool)>;

class TransferDataRequest
{
    private:
    Logger& TransferDataLog;
public:
    /**
   * @brief Constructor.   
   */
    /* constructor to initialize the transfer data */
    TransferDataRequest(Logger &logger);

    /**
   * @brief Destructor.
   */
    ~TransferDataRequest();
    /**
     * @brief Method to set the callback function.    
     * @param[in] callback A function to be called when the transfer is completed.
    */
    void setTransferDataCallBack(transferDataCallBack callback );


    /** Method to handle the request transfer data 0x36.
    *   This method will invoke the callback with the result of the transfer
    *   and return true if the transfer should continue or false if it should stop
    *   @param[in] transferSucces A boolean that indicates if the transfer was successfull.
    *   @return Returns true if the transfer should continue, false if it should stop.
    */
    bool transferDataRequest(bool transferSucces);
   
private:
    /* Member variable to store the callback function */
    transferDataCallBack callback_;

    /* Member for handling exit request */
      RequestTransferExit exitHandler_;
};



#endif /* TRANSFER_DATA_H_ */