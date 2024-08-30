#include "../include/SecurityAccess.h"
#include "../../../ecu_simulation/BatteryModule/include/BatteryModule.h"
#include "../../../mcu/include/MCUModule.h"

/* Set the default security access to false. */
bool SecurityAccess::mcu_state = false;

/* Adjust the default nr of attempts as needed here. */
uint8_t SecurityAccess::nr_of_attempts = MAX_NR_OF_ATTEMPTS;

std::vector<uint8_t> SecurityAccess::security_access_seed;

/* By default we dont have delay timer */
uint32_t SecurityAccess::time_left = 0;

std::chrono::steady_clock::time_point SecurityAccess::end_time =
        std::chrono::steady_clock::now() + std::chrono::hours(24 * 365);

std::chrono::steady_clock::time_point SecurityAccess::end_time_security =
        std::chrono::steady_clock::now() + std::chrono::hours(24 * 365);

SecurityAccess::SecurityAccess(int socket_api, Logger& security_logger)
                :  security_logger(security_logger), socket_api(socket_api)
{}

std::chrono::steady_clock::time_point SecurityAccess::getEndTimeSecurity()
{
    return end_time_security;
}

bool SecurityAccess::getMcuState(Logger& security_logger)
{
    auto security_now = std::chrono::steady_clock::now();
    if (security_now >= end_time_security)
    {
        mcu_state = false;
    }

    if (security_now < end_time_security && mcu_state)
    {
    uint32_t time_left_security = static_cast<uint32_t>
    (
        std::chrono::duration_cast<std::chrono::milliseconds>
        (
            SecurityAccess::getEndTimeSecurity() - security_now
        ).count()
    );
    uint32_t seconds = time_left_security / 1000;
    uint32_t milliseconds = time_left_security % 1000;
    LOG_INFO(security_logger.GET_LOGGER(), "Security timer activated." \
        " {} seconds and {} milliseconds until the security expires.",
    seconds,milliseconds);
    LOG_INFO(security_logger.GET_LOGGER(), "Server is unlocked.");
    }
    
    return mcu_state;
}

std::vector<uint8_t> SecurityAccess::computeKey(const std::vector<uint8_t>& seed)
{
    std::vector<uint8_t> result;

    for (uint8_t num : seed)
    {
        result.push_back(~num + 1);
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

/* Function to convert the time value to a 5-byte representation in big-endian format */
std::vector<uint8_t> SecurityAccess::convertTimeToCANFrame(uint32_t timeInSeconds)
{
    std::vector<uint8_t> frame = {0x07, 0x7F, SECURITY_ACCESS_SID, NegativeResponse::RTDNE};

    /* Calculate number of non-zero bytes needed to represent timeInSeconds */
    std::vector<uint8_t> timeBytes;
    for (int i = 3; i >= 0; --i)
    {
        uint8_t byte = (timeInSeconds >> (8 * i)) & 0xFF;
        if (!timeBytes.empty() || byte != 0)
        {
            timeBytes.push_back(byte);
        }
    }

    /* Calculate how many padding bytes we need */
    size_t numPaddingBytes = 4 - timeBytes.size();
    
    /* Add the padding bytes */
    for (size_t i = 0; i < numPaddingBytes; ++i)
    {
        frame.push_back(0x00);
    }

    frame.insert(frame.end(), timeBytes.begin(), timeBytes.end());

    return frame;
}

void SecurityAccess::securityAccess(canid_t can_id, const std::vector<uint8_t>& request)
{
    std::vector<uint8_t> response;
    std::vector<uint8_t> key;

    /* Extract the first 8 bits of can_id. */
    uint8_t lowerbits = can_id & 0xFF;
    uint8_t upperbits = can_id >> 8 & 0xFF;
    NegativeResponse nrc(socket_api, security_logger);

    /* Reverse ids */
    can_id = ((lowerbits << 8) | upperbits);
    if (upperbits == 0xFA) 
    {
        generate_frames = new GenerateFrames(socket_api, security_logger);
        GenerateFrames generate_frames_ECU(socket_api, security_logger);
        /** 
         * Check if the delay timer has expired.
         * Set the nr of attempts to default. Delay timer will be 0.
        */
        auto now = std::chrono::steady_clock::now();
        if (now >= end_time)
        {
            time_left = 0;
            nr_of_attempts = MAX_NR_OF_ATTEMPTS;
            LOG_INFO(security_logger.GET_LOGGER(), "Delay timer expired. You can attempt to send the key again.");
            end_time = std::chrono::steady_clock::now() + std::chrono::hours(24 * 365);
        }
        /** Incorrect message length or invalid format.
         *  Frame data must have at least pci_length + SID + subfunction.
         *  If subfunction is 2, we must have at least 1 byte with key.
         *  Size of the framedata must be always pci_length + 1.
        */
        if ((request.size() < 3) ||
            (request[2] == 0x02 && request[0] == 2)
            || (request.size() != static_cast<size_t>(request[0] + 1)))
        {
            nrc.sendNRC(can_id,SECURITY_ACCESS_SID,NegativeResponse::IMLOIF);
        }
        else
        {
            uint8_t pci_length = request[0];
            uint8_t sf = request[2];
            /* Subfunction not supported, we use only 1st lvl of security access. */
            if (sf != 0x01 && sf != 0x02)
            {
                nrc.sendNRC(can_id,SECURITY_ACCESS_SID,NegativeResponse::SFNS);
            }
            else if (sf == 0x01)
            {
                /* Adjust the seed length between 1 and 5.*/
                std::srand(static_cast<uint8_t>(std::time(nullptr)));
                size_t seed_length = std::rand() % 5 + 1;

                std::vector<uint8_t> seed = generateRandomBytes(seed_length);

                /* PCI length = seed_length + 2(0x67 and 0x01)*/
                response.push_back(static_cast<uint8_t>(2 + seed_length));

                /* SID response */
                response.push_back(0x67);

                /* subfunction request seed */
                response.push_back(0x01);

                /* complete the seed in frame if server is locked, or with 0x00 if unlocked. */
                for (auto e : seed)
                {
                    response.push_back(mcu_state ? 0x00 : e);
                }
                generate_frames->sendFrame(can_id,response,DATA_FRAME);
                security_access_seed = seed;
                LOG_INFO(security_logger.GET_LOGGER(), "Seed was sent.");
            }
            else if (sf == 0x02 && !mcu_state)
            {
                /* Request sequence error, cannot receive sendKey before requestSeed. */
                if (security_access_seed.empty())
                {
                    LOG_ERROR(security_logger.GET_LOGGER(), "Cannot have sendKey request before requestSeed.");
                    nrc.sendNRC(can_id,SECURITY_ACCESS_SID,NegativeResponse::RSE);
                }
                else if (time_left == 0)
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
                        LOG_INFO(security_logger.GET_LOGGER(), "Security Access granted successfully.");
                        mcu_state = true;
                        response.clear();
                        end_time_security = std::chrono::steady_clock::now() +
                                    std::chrono::seconds(SECURITY_TIMEOUT_IN_SECONDS);
                        auto security_now = std::chrono::steady_clock::now();
                        uint32_t time_left_security = static_cast<uint32_t>
                        (
                            std::chrono::duration_cast<std::chrono::milliseconds>
                            (
                                end_time_security - security_now
                            ).count()
                        );
                        uint32_t seconds = time_left_security / 1000;
                        uint32_t milliseconds = time_left_security % 1000;
                        LOG_INFO(security_logger.GET_LOGGER(), "Security timer activated." \
                            " {} seconds and {} milliseconds until the security expires.",
                        seconds,milliseconds);
                        
                    }
                    else
                    {   
                        if (nr_of_attempts > 0)
                        {
                            /* Invalid key, doesnt match with server's key. */
                            nrc.sendNRC(can_id,SECURITY_ACCESS_SID,NegativeResponse::IK);
                            nr_of_attempts--;
                            LOG_INFO(security_logger.GET_LOGGER(), "{} attempts left.", nr_of_attempts);
                        }
                        else
                        {
                            /** 
                             * Send if an expected ‘sendKey’ SubFunction is received, the value of the key does not 
                             * match the server’s internally stored/calculated key, and the delay timer is activated
                             * by this request(i.e. due to reaching the limit of false access attempts which activate
                             * the delay timer).
                            */
                            nrc.sendNRC(can_id,SECURITY_ACCESS_SID,NegativeResponse::ENOA);
                                
                            /* Start the delay timer clock. */
                            end_time = std::chrono::steady_clock::now() + std::chrono::seconds(TIMEOUT_IN_SECONDS);
                            now = std::chrono::steady_clock::now();
                            time_left = static_cast<uint32_t>
                            (
                                std::chrono::duration_cast<std::chrono::milliseconds>
                                (
                                    end_time - now
                                ).count()
                            );
                            LOG_INFO(security_logger.GET_LOGGER(), "Delay timer activated.");
                            uint32_t time_left_copy = time_left;
                            uint32_t seconds = time_left_copy / 1000;
                            uint32_t milliseconds = time_left_copy % 1000;
                            LOG_ERROR(security_logger.GET_LOGGER(), "Please wait {} seconds and {} milliseconds" \
                                " before sending key again.", seconds,milliseconds);
                        }
                    }
                }
                else
                {
                    /** Send if a ‘requestSeed’ SubFunction is received and the delay timer is active 
                     * for the requested security level.
                    */
                    now = std::chrono::steady_clock::now();
                    time_left = static_cast<uint32_t>
                    (
                        std::chrono::duration_cast<std::chrono::milliseconds>
                        (
                            end_time - now
                        ).count()
                    );
                    uint32_t time_left_copy = time_left;
                    uint32_t seconds = time_left_copy / 1000;
                    uint32_t milliseconds = time_left_copy % 1000;
                    LOG_ERROR(security_logger.GET_LOGGER(), "Please wait {} seconds and {} milliseconds" \
                        " before sending key again.", seconds,milliseconds);
                    response = convertTimeToCANFrame(time_left);
                    generate_frames->sendFrame(can_id,response,DATA_FRAME);
                }
            }
            else
            {
                LOG_ERROR(security_logger.GET_LOGGER(), "Server is already unlocked.");
            }
        }
        delete generate_frames;
    }
    else
    {
        LOG_ERROR(security_logger.GET_LOGGER(), "Security service can be accessed only from API.");
    }
}