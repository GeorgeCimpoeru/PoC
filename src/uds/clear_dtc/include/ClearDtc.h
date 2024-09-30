/**
 * @file ClearDtc.h
 * @author Mujdei Ruben
 * @brief 
 * @version 0.1
 * @date 2024-07-23
 * 
 * @copyright Copyright (c) 2024
 * 
 */
#ifndef CLEAR_DTC_H
#define CLEAR_DTC_H

#include <string>
#include <vector>
#include <fstream>
#include <map>

#include "../../../utils/include/GenerateFrames.h"
#include "../../../utils/include/Logger.h"

class ClearDtc
{
    private:
        std::string path_to_dtc = "";
        GenerateFrames* generate;
        Logger& logger;
        int socket;
    public:
        /**
         * @brief Construct a new Clear Dtc object
         * 
         * @param path_to_dtc A string representing the file path to the DTC.
         * @param logger A logger instance used to record information and errors during the execution.
         * @param socket The socket descriptor used for communication over the CAN bus.
         */
        ClearDtc(std::string path_to_dtc, Logger& logger, int socket);
        /**
         * @brief Main method to delete dtc
         * 
         * @param id An unique identifier for DTC.
         * @param data Data from the frame.
         */
        void clearDtc(int id, std::vector<uint8_t> data);

    private:
        /**
         * @brief Method for extract groupDTC base on the DTC
         * 
         * @param dtc A string representing the DTC to be processed. This code is used to extract the group identifier for the corresponding diagnostic error.
         * @return Returns an uint32_t extracted group identifier for the provided DTC (diagnostic trouble code) string
         */
        uint32_t extractGroup(std::string dtc);
        /**
         * @brief Method to verify group of DTCs available
         * 
         * @param group_of_dtc The group identifier of a set of DTCs.
         * @return Returns a boolean if the provided DTC group is found or not in the list of valid group DTCs
         */
        bool verifyGroupDtc(uint32_t group_of_dtc);
};

#endif