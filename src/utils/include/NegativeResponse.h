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