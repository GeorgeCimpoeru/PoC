#include "../include/TesterPresent.h"
#include "../../../ecu_simulation/BatteryModule/include/BatteryModule.h"
#include "../../../ecu_simulation/EngineModule/include/EngineModule.h"
#include "../../../ecu_simulation/DoorsModule/include/DoorsModule.h"
#include "../../../ecu_simulation/HVACModule/include/HVACModule.h"
#include "../../../mcu/include/MCUModule.h"

TesterPresent::TesterPresent(int socket, Logger& logger,  DiagnosticSessionControl& sessionControl)
                :  socket(socket), logger(logger),generate_frames(socket, logger),
                sessionControl(sessionControl)
{}
std::chrono::steady_clock::time_point TesterPresent::end_time =
        std::chrono::steady_clock::now() + std::chrono::hours(24 * 365);

uint32_t TesterPresent::time_left = 0;

std::chrono::steady_clock::time_point TesterPresent::getEndTimeProgrammingSession()
{
    return end_time;
}
void TesterPresent::stopAccessTimingFlags(uint32_t can_id)
{
    uint8_t receiver_id = can_id & 0xFF;;
    switch(receiver_id)
    {
        case 0x10:
            MCU::mcu->stop_flags[0x3E] = false;
            break;
        case 0x11:
            battery->stop_flags[0x3E] = false;
            break;
        case 0x12:
            engine->stop_flags[0x3E] = false;
            break;
        case 0x13:
            doors->stop_flags[0x3E] = false;
            break;
        case 0x14:
            hvac->_ecu->stop_flags[0x3E] = false;
            break;
        default:
            LOG_ERROR(logger.GET_LOGGER(), "Module with id {:x} not supported.", receiver_id);
            break; 
    }
}

void TesterPresent::setEndTimeProgrammingSession()
{
    end_time = std::chrono::steady_clock::now() + std::chrono::hours(24 * 365);
}

void TesterPresent::handleTesterPresent(uint32_t can_id, std::vector<uint8_t> request)
{
    uint8_t lowerbits = can_id & 0xFF;
    uint8_t upperbits = can_id >> 8 & 0xFF;
    uint32_t can_id_response;
    NegativeResponse nrc(socket, logger);

    /* Reverse ids */
    can_id_response = ((lowerbits << 8) | upperbits);
    if ((request.size() != 3) ||
        (request.size() != static_cast<size_t>(request[0] + 1)))
    {
        nrc.sendNRC(can_id_response,TESTER_PRESENT_SID,NegativeResponse::IMLOIF);
        stopAccessTimingFlags(can_id);
        return;
    }
    uint8_t sf = request[2];
    /* Subfunction not supported, we use only 00 subfunction */
    if (sf != 0x00)
    {
        nrc.sendNRC(can_id_response,TESTER_PRESENT_SID,NegativeResponse::SFNS);
        stopAccessTimingFlags(can_id);
        return;
    }
    /* Send positive response from tester present */
    generate_frames.testerPresent(can_id_response, true);
    stopAccessTimingFlags(can_id);
    if(DiagnosticSessionControl::getCurrentSessionToString() == "DEFAULT_SESSION")
    {
        /* Change default session to programming session */
        sessionControl.sessionControl(can_id, 0x02);
        LOG_INFO(logger.GET_LOGGER(), "Default session changed into programming session");
    }
    end_time = std::chrono::steady_clock::now() + 
        std::chrono::seconds(S3_TIMER);
}