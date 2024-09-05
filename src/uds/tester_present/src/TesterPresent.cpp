#include "../include/TesterPresent.h"
#include "../../../ecu_simulation/BatteryModule/include/BatteryModule.h"
#include "../../../ecu_simulation/EngineModule/include/EngineModule.h"
#include "../../../mcu/include/MCUModule.h"

TesterPresent::TesterPresent(int socket, Logger& logger,  DiagnosticSessionControl& sessionControl)
                :  socket(socket), logger(logger),generate_frames(socket, logger),
                sessionControl(sessionControl)
{}

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
        return;
    }
    uint8_t sf = request[2];
    /* Subfunction not supported, we use only 00 subfunction */
    if (sf != 0x00)
    {
        nrc.sendNRC(can_id_response,TESTER_PRESENT_SID,NegativeResponse::SFNS);
        return;
    }
    /* Send positive response from tester present */
    generate_frames.testerPresent(can_id_response, true);
    if(DiagnosticSessionControl::getCurrentSessionToString() == "DEFAULT_SESSION")
    {
        /* Change default session to programming session */
        sessionControl.sessionControl(can_id, 0x02);
        LOG_INFO(logger.GET_LOGGER(), "Default session changed into programming session");
    }
}