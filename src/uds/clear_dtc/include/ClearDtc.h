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
        int socket;
        Logger* logger;
    public:
        /**
         * @brief Construct a new Clear Dtc object
         * 
         * @param path_to_dtc 
         * @param logger 
         * @param socket 
         */
        ClearDtc(std::string path_to_dtc, Logger& logger, int socket);
        /**
         * @brief Main method to delete dtc
         * 
         * @param id 
         * @param data 
         */
        void clearDtc(int id, std::vector<uint8_t> data);
    private:
        /**
         * @brief Method for extract groupDTC base on the DTC
         * 
         * @param dtc 
         */
        uint32_t extractGroup(std::string dtc);
        /**
         * @brief Method to verify groupOfDTCs availables
         * 
         * @param group_of_dtc 
         */
        bool verifyGroupDtc(uint32_t group_of_dtc);
};

#endif