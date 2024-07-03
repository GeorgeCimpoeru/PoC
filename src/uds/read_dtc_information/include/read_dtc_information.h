/**
 * @file read_dtc_information.h
 * @author Mujdei Ruben
 * @brief UDS Service 0x19 Read DTC(Data Trouble Code) information.
 * @version 0.1
 * @date 2024-07-02
 * 
 * @copyright Copyright (c) 2024
 * 
 */
#ifndef READ_DTC_INFROMATION_H
#define READ_DTC_INFROMATION_H

#include "../../../utils/include/GenerateFrames.h"

#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <map>
#include <chrono>

#include <poll.h>
#include <sys/ioctl.h>
#include <net/if.h>

class ReadDTC
{
    private:
        std::string path_folder;
        GenerateFrames* generate;
        int socket_;
        Logger logger;

    public:
        /**
         * @brief Construct a new ReadDTC object
         * 
         * @param path_folder path to the file containing the dtcs
         */
        ReadDTC(int socket_, Logger logger, std::string path_folder = "default");
        /**
         * @brief Destroy the Read DTC object
         * 
         */
        ~ReadDTC();
        /**
         * @brief Main method to the 0x19 read DTC UDS service 
         * 
         * @param id can id
         * @param sub_function sub-function required
         * @param dtc_status_mask status mask filter
         */
        void read_dtc(int id, int sub_function, int dtc_status_mask);

    private:
        /**
         * @brief method for 0x01 sub-function. Count the DTC. Send a frame
         * with the amound of DTCs
         * 
         * @param id ca id
         * @param dtc_status_mask status mask filter
         */
        void number_of_dtc(int id, int dtc_status_mask);
        /**
         * @brief method for 0x02 sub-function. Raport DTCs by status mask
         * 
         * @param id can id
         * @param dtc_status_mask status mask filter
         */
        void report_dtcs(int id, int dtc_status_mask);
        /**
         * @brief Method from char to int
         * 
         * @param c character
         * @return int
         */
        int to_int(char c);
        /**
         * @brief Receive through the can-bus the flow controll frame
         * 
         * @return true 
         * @return false 
         */
        bool receive_flow_control();
        /**
         * @brief Transform the raw DTCs in to hex values
         * 
         * @param dtc String dtc
         * @return int 
         */
        int dtc_to_hex(std::string dtc);
};

#endif