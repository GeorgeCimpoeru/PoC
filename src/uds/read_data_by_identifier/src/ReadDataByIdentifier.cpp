#include "../include/ReadDataByIdentifier.h"
#include "../../../ecu_simulation/BatteryModule/include/BatteryModule.h"
#include "../../../mcu/include/MCUModule.h"

ReadDataByIdentifier::ReadDataByIdentifier(int socket, Logger* rdbi_logger) 
            : generate_frames(socket, *rdbi_logger), rdbi_logger(*rdbi_logger)
{
    this->socket = socket;
}

/* Define the service identifier for Read Data By Identifier */
const uint8_t RDBI_SERVICE_ID = 0x22;

/* Function to handle the Read Data By Identifier request */
std::vector<uint8_t> ReadDataByIdentifier::readDataByIdentifier(canid_t can_id, const std::vector<uint8_t>& request, bool use_send_frame) {
    std::vector<uint8_t> response;

    /* Extract the first 8 bits of can_id */
    uint8_t lowerbits = can_id & 0xFF;
    uint8_t upperbits = can_id >> 8 & 0xFF;
    /* Check if the request size is less than 4 */
    if (request.size() < 4) {
        /* Invalid request length - prepare a negative response */
        response.push_back(0x03); /* PCI */
        response.push_back(0x7F); /* Negative response */
        response.push_back(RDBI_SERVICE_ID); /* Service ID */
        response.push_back(0x13); /* Incorrect message length or invalid format */

        if (use_send_frame) {
            /* Send the negative response frame */ 
            /* this will be replaced by NegativeResponse when done */
            generate_frames.readDataByIdentifier(can_id, 0, response);
        }

        /* Return early as the request is invalid */
        return response;
    }

    /* Extract the data identifier from the request */
    uint16_t data_identifier = (request[2] << 8) | request[3];

    /* Reverse IDs */
    can_id = ((lowerbits << 8) | upperbits);

    /* Determine which ECU data storage to use based on the first 8 bits of can_id */
    if ((lowerbits == 0x10 && MCU::mcu->mcu_data.find(data_identifier) != MCU::mcu->mcu_data.end()) ||
        (lowerbits == 0x11 && battery->ecu_data.find(data_identifier) != battery->ecu_data.end())) {

        /* Fetch the data based on the data identifier */
        if (lowerbits == 0x10) {
            response = MCU::mcu->mcu_data.at(data_identifier);
        } else {
            // battery->fetchBatteryData();
            LOG_ERROR(rdbi_logger.GET_LOGGER(), "am ajuns aici");
            response = (battery->ecu_data).at(data_identifier);
        }

    } else {
        /* Data identifier not found */
        LOG_ERROR(rdbi_logger.GET_LOGGER(), "Request out of range");
    }

    if (use_send_frame) {
        if (response.size() > 7) {
            generate_frames.readDataByIdentifierLongResponse(can_id, data_identifier, response, true);
        } else {
            generate_frames.readDataByIdentifier(can_id, data_identifier, response);
        }
    }
    return response;
}