/**
 * @file SecurityAccess.h
 * @author Theodor Stoica
 * @brief Negative Response Class
 * @version 0.1
 * @date 2024-07-18
 * 
 * @copyright Copyright (c) 2024
 * 
 */
#ifndef NEGATIVE_RESPONSE_H
#define NEGATIVE_RESPONSE_H

#include <linux/can.h>
#include <vector>
#include <unordered_map>
#include <cstdint>
#include <map>
#include <string>

#include "GenerateFrames.h"
#include "Logger.h"

class NegativeResponse
{
    public:
        static const std::map<uint8_t, std::string> nrcMap;
        /* SubFunction Not Supported */
        static constexpr uint8_t SFNS = 0x12;
        /* Incorrect Message Length Or Invalid Format */
        static constexpr uint8_t IMLOIF = 0x13;
        /* Response Too Long */
        static constexpr uint8_t RTL = 0x14;
        /* Conditions Not Correct */
        static constexpr uint8_t CNC = 0x22;
        /* Request Sequence Error */
        static constexpr uint8_t RSE = 0x24;
        /* No Response From Subnet Component */
        static constexpr uint8_t NRSC = 0x25;
        /* Request Out Of Range */
        static constexpr uint8_t ROOR = 0x31;
        /* Security Access Denied */
        static constexpr uint8_t SAD = 0x33;
        /* Authentication Required */
        static constexpr uint8_t AR = 0x34;
        /* Invalid Key */
        static constexpr uint8_t IK = 0x35;
        /* Exceeded Number Of Attempts */
        static constexpr uint8_t ENOA = 0x36;
        /* Required Time Delay Not Expired */
        static constexpr uint8_t RTDNE = 0x37;
        /* Upload Download Not Accepted */
        static constexpr uint8_t UDNA = 0x70;
        /* Transfer Data Suspended */
        static constexpr uint8_t TDS = 0x71;
        /* General Programming Failure */
        static constexpr uint8_t GPF = 0x72;
        /* Wrong Block Sequence Counter */
        static constexpr uint8_t WBSC = 0x73;
        /* Voltage Too High */
        static constexpr uint8_t VTH = 0x92;
        /* Voltage Too Low */
        static constexpr uint8_t VTL = 0x93;

    private:
        GenerateFrames* generate_frames;
        Logger& nrc_logger;
        int socket = -1;

    public:
        NegativeResponse(int socket, Logger& nrc_logger);
        std::string getDescription(uint8_t code);
        void sendNRC(int id, uint8_t sid, uint8_t nrc);
 
};

#endif