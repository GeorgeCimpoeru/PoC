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

void TesterPresent::setEndTimeProgrammingSession(bool isProgramming)
{
    if (!isProgramming)
    {
        end_time = std::chrono::steady_clock::now() + std::chrono::seconds(S3_TIMER);
        return;
    }
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
        AccessTimingParameter::stopTimingFlag(lowerbits, 0x3E);
        return;
    }
    uint8_t sf = request[2];
    /* Subfunction not supported, we use only 00 subfunction */
    if (sf != 0x00)
    {
        nrc.sendNRC(can_id_response,TESTER_PRESENT_SID,NegativeResponse::SFNS);
        AccessTimingParameter::stopTimingFlag(lowerbits, 0x3E);
        return;
    }
    /* Send positive response from tester present */
    generate_frames.testerPresent(can_id_response, true);
    AccessTimingParameter::stopTimingFlag(lowerbits, 0x3E);
    if(DiagnosticSessionControl::getCurrentSessionToString() == "DEFAULT_SESSION")
    {
        /* Change default session to programming session */
        sessionControl.sessionControl(can_id, 0x02);
        LOG_INFO(logger.GET_LOGGER(), "Default session changed into programming session");
    }
    end_time = std::chrono::steady_clock::now() + 
        std::chrono::seconds(S3_TIMER);
}