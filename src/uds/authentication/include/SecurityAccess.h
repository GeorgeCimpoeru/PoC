/**
 * @file SecurityAccess.h
 * @author Theodor Stoica
 * @brief UDS Service 0x27 Security Access
 * @version 0.1
 * @date 2024-07-02
 * 
 * @copyright Copyright (c) 2024
 * 
 */

#include <linux/can.h>
#include <vector>
#include <unordered_map>
#include <bitset>
#include <iomanip>
#include <random>

#include "../../../utils/include/GenerateFrames.h"
#include "../../utils/include/Logger.h"

#ifndef UDS_SECURITY_ACCESS_H
#define UDS_SECURITY_ACCESS_H

class SecurityAccess
{
    public:
    /* SID for SecurityAccess */
    static const uint8_t SECURITY_ACCESS_SID = 0x27;

    /* SubFunctionNotSupported */
    static const uint8_t SFNS = 0x12;

    /* IncorrectMesssageLengthOrInvalidFormat */
    static const uint8_t IMLOIF = 0x13;

    /* RequestSequenceError */
    static const uint8_t RSE = 0x24;

    /* Invalid key */
    static const uint8_t IK = 0x35;

    /* Exceeded nr of attempts */
    static const uint8_t ENOA = 0x36;

    /* Required time delay not expired */
    static const uint8_t RTDNE = 0x37;

    /**
    * @brief Default constructor
    */
    SecurityAccess(int socket, Logger& security_logger);

    void securityAccess(canid_t can_id, const std::vector<uint8_t>& data);

    std::vector<uint8_t> computeKey(const std::vector<uint8_t>& seed);

    std::vector<uint8_t> generateRandomBytes(size_t length);

    /**
    * @brief Getter for MCU state access
    * @return The current value of MCU state
    */
    static bool getMcuState();

    private:
    GenerateFrames* generate_frames;
    Logger& security_logger;
    int socket = -1;
    static bool mcu_state;
    static std::vector<uint8_t> security_access_seed;
};

#endif