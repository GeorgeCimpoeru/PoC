#include "../include/ReadDataByIdentifier.h"
#include "../../../ecu_simulation/BatteryModule/include/BatteryModule.h"
#include "../../../mcu/include/MCUModule.h"

ReadDataByIdentifier::ReadDataByIdentifier()
{}

/* Define the service identifier for Read Data By Identifier */
const uint8_t RDBI_SERVICE_ID = 0x22;

/* Function to handle the Read Data By Identifier request */
std::vector<uint8_t> ReadDataByIdentifier::readDataByIdentifier(canid_t can_id, const std::vector<uint8_t>& request, Logger& rdbi_logger) {
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

        /* Send the response frame to the bus */
        create_interface = create_interface->getInstance(0x00, rdbi_logger);
        if (create_interface) 
        {
            /* Send to API or canbus based on the upperbits(sender) */
            generate_frames = new GenerateFrames(
                (upperbits == 0xFA) ? create_interface->getSocketApiWrite() : create_interface->getSocketEcuWrite(),
                rdbi_logger);
                /* Send the negative response frame */ 
                /* this will be replaced by NegativeResponse when done */
                generate_frames->readDataByIdentifier(can_id, 0, response);
        }
        else
        {
            /* Handle the case where create_interface is null */
            LOG_ERROR(rdbi_logger.GET_LOGGER(), "create_interface is nullptr");
        }
        /* Return early as the request is invalid */
        return response;
    }

    /* Extract the data identifier from the request */
    uint16_t data_identifier = (request[2] << 8) | request[3];

    /* Reverse IDs */
    can_id = ((lowerbits << 8) | upperbits);

    /* Vector to store data from ecuData */
    std::vector<uint8_t> stored_data;

    /* Determine which ECU data storage to use based on the first 8 bits of can_id */
    if ((lowerbits == 0x10 && MCU::mcu.mcu_data.find(data_identifier) != MCU::mcu.mcu_data.end()) ||
        (lowerbits == 0x11 && battery.ecu_data.find(data_identifier) != battery.ecu_data.end())) {

        /* Fetch the data based on the data identifier */
        if (lowerbits == 0x10) {
            stored_data = MCU::mcu.mcu_data.at(data_identifier);
        } else {
            battery.fetchBatteryData();
            stored_data = battery.ecu_data.at(data_identifier);
        }

        /* Prepare a positive response */
        response.clear();
        response.push_back(0x03); /* PCI */
        response.push_back(0x62); /* Response SID = Initial SID + 0x40 */
        response.push_back(request[2]); /* Data identifier, two bytes */
        response.push_back(request[3]);
        response.insert(response.end(), stored_data.begin(), stored_data.end());

    } else {
        /* Data identifier not found */
        /* response.push_back(0x03); */
        /* response.push_back(0x7F); */ /* Negative response */
        /* response.push_back(RDBI_SERVICE_ID); */
        /* response.push_back(0x31); */ /* Request out of range */
        /* 0x31 NRC is not yet defined so I log it instead */
        LOG_ERROR(rdbi_logger.GET_LOGGER(), "Request out of range");
    }

    /* Check if sender was API so MCU will send directly to API */
    create_interface = create_interface->getInstance(0x00, rdbi_logger);
    if (create_interface) {
        generate_frames = new GenerateFrames(
            (upperbits == 0xFA) ? create_interface->getSocketApiWrite() : create_interface->getSocketEcuWrite(),
            rdbi_logger
        );

        if (response.size() > 8) {
            generate_frames->readDataByIdentifierLongResponse(can_id, data_identifier, response, true);
        } else {
            generate_frames->readDataByIdentifier(can_id, data_identifier, response);
        }
    } else {
        /* Handle the case where create_interface is null */
        LOG_ERROR(rdbi_logger.GET_LOGGER(), "create_interface is nullptr");
    }

    return response;
}