#include "../include/SecurityAccess.h"
#include "../../../mcu/include/MCUModule.h"

/* Set the default security access to false */
bool SecurityAccess::mcu_state = false;

std::vector<uint8_t> SecurityAccess::security_access_seed;

SecurityAccess::SecurityAccess(int socket, Logger& security_logger)
                :  security_logger(security_logger), socket(socket)
{}

bool SecurityAccess::getMcuState()
{
    return mcu_state;
}

std::vector<uint8_t> SecurityAccess::computeKey(const std::vector<uint8_t>& seed)
{
    std::vector<uint8_t> result;

    for (uint8_t num : seed)
    {
        uint8_t inverted = ~num;
        uint8_t twosComplement = inverted + 1;

        result.push_back(twosComplement);
    }

    return result;
}

std::vector<uint8_t> SecurityAccess::generateRandomBytes(size_t length)
{
    std::vector<uint8_t> bytes(length);
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<uint8_t> dis(0, 255);

    for (size_t i = 0; i < length; ++i)
    {
        bytes[i] = dis(gen);
    }

    return bytes;
}

void SecurityAccess::securityAccess(canid_t can_id, const std::vector<uint8_t>& request)
{
    std::vector<uint8_t> response;
    std::vector<uint8_t> key;

    /* Extract the first 8 bits of can_id */
    uint8_t lowerbits = can_id & 0xFF;
    uint8_t upperbits = can_id >> 8 & 0xFF;

    /* Reverse ids */
    can_id = ((lowerbits << 8) | upperbits);
    if (upperbits == 0xFA) 
    {
            generate_frames = new GenerateFrames(socket, security_logger);

            /* Incorrect message length or invalid format */
            if ((request.size() < 3) ||
                (request[2] == 0x02 && request[0] == 2)
                || (request.size() != static_cast<size_t>(request[0] + 1)))
            {
                generate_frames->negativeResponse(can_id,SECURITY_ACCESS_SID, IMLOIF);
            }
            else
            {
                uint8_t pci_length = request[0];
                uint8_t sf = request[2];
                /* Subfunction not supported, we use only 1st lvl of security access */
                if (sf != 0x01 && sf != 0x02)
                {
                    generate_frames->negativeResponse(can_id,SECURITY_ACCESS_SID, SFNS);
                }
                else if (sf == 0x01)
                {
                    /* Adjust the seed length between 1 and 5*/
                    size_t seed_length = 2;

                    /* std::vector<uint8_t> seed = generateRandomBytes(seed_length); */
                    std::vector<uint8_t> seed = {0x36,0x57};

                    /* PCI length = seed_length + 2(0x67 and 0x01)*/
                    response.push_back(2 + seed_length);

                    /* SID response */
                    response.push_back(0x67);

                    /*subfunction request seed*/
                    response.push_back(0x01);

                    /* complete the seed in frame if server is locked, or with 0x00 if unlocked */
                    for (auto e : seed)
                    {
                        response.push_back(mcu_state ? 0x00 : e);
                    }
                    generate_frames->sendFrame(can_id,response,DATA_FRAME);
                    security_access_seed = seed;                  
                }
                else if (sf == 0x02 && !mcu_state)
                {
                    /* Request sequence error, cannot receive sendKey before requestSeed */
                    if (security_access_seed.empty())
                    {
                        LOG_ERROR(security_logger.GET_LOGGER(), "Cannot have sendKey request before requestSeed.");
                        generate_frames->negativeResponse(can_id,SECURITY_ACCESS_SID,RSE);
                    }
                    else
                    {
                        std::vector<uint8_t> computed_key = computeKey(security_access_seed);
                        std::vector<uint8_t> received_key;
                        for (int i = 3; i <= pci_length; i++)
                        {
                            received_key.push_back(request[i]);
                        }
                        if(received_key == computed_key)
                        {
                            /* PCI length */
                            response.push_back(0x02);

                            /* SID response sendKey */
                            response.push_back(0x67);

                            /* subfunction sendkey */
                            response.push_back(0x02);

                            generate_frames->sendFrame(can_id,response,DATA_FRAME);
                            LOG_INFO(security_logger.GET_LOGGER(), "Security Access granted successfully");
                            mcu_state = true;
                        }
                        else
                        {   /* Invalid key, doesnt match with server's key. */
                            LOG_ERROR(security_logger.GET_LOGGER(), "Security Access denied. Invalid Key");
                            generate_frames->negativeResponse(can_id,SECURITY_ACCESS_SID, IK);
                        }
                    }
                }
                else
                {
                    LOG_INFO(security_logger.GET_LOGGER(), "Server is already unlocked");
                }
            }
    }
        else 
        {
            /* Handle the case where create_interface is null */
            LOG_ERROR(security_logger.GET_LOGGER(), "Create_interface is nullptr");
        }
} 