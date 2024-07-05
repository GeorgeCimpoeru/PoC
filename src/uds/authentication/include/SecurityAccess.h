/**
 * @file SecurityAccess.h
 * @author Theodor Stoica
 * @brief This library represents the SecurityAccess UDS service.
 */

#include <linux/can.h>
#include <vector>
#include <unordered_map>
#include <bitset>
#include <iomanip>
#include <random>

#include "../../../utils/include/CreateInterface.h"
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

    /* ConditionsNotCorrect */
    static const uint8_t CNC = 0x22;

    /* RequestSequenceError */
    static const uint8_t RSE = 0x24;

    /* Invalid key */
    static const uint8_t IK = 0x35;

    /**
    * @brief Default constructor
    */
    SecurityAccess();

    void securityAccess(canid_t can_id, const std::vector<uint8_t>& data, Logger& securityLogger);

    std::vector<uint8_t> computeKey(const std::vector<uint8_t>& seed);

    std::vector<uint8_t> generateRandomBytes(size_t length);

    private:
    GenerateFrames* generate_frames;
    CreateInterface* create_interface;

};

#endif