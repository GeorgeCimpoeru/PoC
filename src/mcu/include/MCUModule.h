/**
 * This class represents the MCU module that interacts with the CAN bus.
 * It provides methods to start and stop the module, as well as to receive CAN frames.
 * These methods are from the InterfaceModule and ReceiveFrames classes.
*/

#ifndef POC_MCU_MODULE_H
#define POC_MCU_MODULE_H

#include "HandleFrames.h"
#include "../../utils/include/CreateInterface.h"
#include "ReceiveFrames.h"
#include "../include/MCULogger.h"
#include "../../uds/write_data_by_identifier/include/WriteDataByIdentifier.h"

#include <thread>
namespace MCU
{
    class MCUModule {
    public:
        /* Variable to store mcu data */
        std::unordered_map<uint16_t, std::vector<uint8_t>> mcu_data;
        /** 
         * @brief Constructor that takes the interface number as an argument.
         * When the constructor is called, it creates a new interface with the
         * given number and starts the interface.
         * @param interface_number The number of the vcan interface
        */
        MCUModule(uint8_t interfaces_number);
        
        /**
         * @brief Default constructor for MCU Module.
         */
        MCUModule();

        /**
         * @brief Destructor for MCU Module.
         */
        ~MCUModule();

        /**
         * @brief Method to start the module. Sets isRunning flag to true.
        */
        void StartModule();

        /**
         * @brief Method to stop the module. Sets isRunning flag to false.
        */
        void StopModule();

        /**
         * @brief Method to receive frames.
         * This method starts a thread to process the queue and receives frames
         * from the CAN bus.
        */
        void recvFrames();

        /**
         * @brief Getter for securityAccess_seed
         * @return The current value of securityAccess_seed
         */
        std::vector<uint8_t> getSecurityAccessSeed();

        /**
         * @brief Setter for securityAccess_seed
         * @param seed The new value to set for securityAccess_seed
         */
        void setSecurityAccessSeed(const std::vector<uint8_t>& seed);

        /**
         * @brief Getter for MCU state access
         * @return The current value of MCU state
         */
        bool getMCUState() const;

        /**
         * @brief Setter for MCU state access
         * @return The current value of MCU state
         */
        void setMCUState(bool state);

    private:
        bool is_running;
        CreateInterface* create_interface;
        ReceiveFrames* receive_frames;
        std::vector<uint8_t> securityAccess_seed;
        bool mcu_state = false;
    };
extern MCUModule mcu;
}
#endif