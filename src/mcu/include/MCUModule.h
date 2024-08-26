/**
 * This class represents the MCU module that interacts with the CAN bus.
 * It provides methods to start and stop the module, as well as to receive CAN frames.
 * These methods are from the InterfaceModule and ReceiveFrames classes.
*/

#ifndef POC_MCU_MODULE_H
#define POC_MCU_MODULE_H

#include "../../utils/include/HandleFrames.h"
#include "../../utils/include/CreateInterface.h"
#include "ReceiveFrames.h"
#include "../include/MCULogger.h"
#include "../../uds/write_data_by_identifier/include/WriteDataByIdentifier.h"
#include "../../uds/tester_present/include/TesterPresent.h"

#include <thread>
#include <future>
#include <fstream>
#include <stdexcept>
#include <filesystem>

namespace MCU
{
    class MCUModule {
    public:
        /* Static dictionary to store SID and processing time */
        static std::map<uint8_t, double> timing_parameters;
        /* Store active timers for SIDs */
        static std::map<uint8_t, std::future<void>> active_timers;
        /* Stop flags for each SID. */
        static std::map<uint8_t, std::atomic<bool>> stop_flags;

        /* Variable to store mcu data */
        std::unordered_map<uint16_t, std::vector<uint8_t>> default_DID_MCU = 
        {
            {0x01E0, {IDLE}}
        };
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

        /**
         * @brief Get the Mcu Api Socket
         * 
         * @return Returns the socket id for API
         */
        int getMcuApiSocket() const;

        /**
         * @brief Get the Mcu Ecu Socket
         * 
         * @return Returns the socket id for ECU
         */
        int getMcuEcuSocket() const;

        /**
         * @brief Recreates and bind the socket of API on a given interface
         * 
         * @param interface_number The interface on which the API socket will be created
         */
        void setMcuApiSocket(uint8_t interface_number);

        /**
         * @brief Recreates and bind the socket of ECU on a given interface
         * 
         * @param interface_number The interface on which the ECU socket will be created
         */
        void setMcuEcuSocket(uint8_t interface_number);

    private:
        bool is_running;
        CreateInterface* create_interface;
        ReceiveFrames* receive_frames;
        int mcu_api_socket = -1;
        int mcu_ecu_socket = -1;
    };
extern MCUModule* mcu;
}
#endif