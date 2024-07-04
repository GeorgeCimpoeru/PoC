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
    /* Extract the data identifier from the request */
    uint16_t data_identifier = (request[2] << 8) | request[3];
        
    /**Extract the first 8 bits of can_id */
    uint8_t lowerbits = can_id & 0xFF;
    uint8_t upperbits = can_id >> 8 & 0xFF;

    /* Vector to store data from ecuData */
    std::vector<uint8_t> stored_data;

    /* Reverse ids */
    can_id = ((lowerbits << 8) | upperbits);
    if (request.size() < 3) 
    {
        /** Invalid request length */
        response.push_back(0x03); /* PCI */
        response.push_back(0x7F); /** Negative response */
        response.push_back(RDBI_SERVICE_ID);
        response.push_back(0x13); /** Incorrect message length or invalid format */
    }
    else
    {
        /** Determine which ECU data storage to use based on the first 8 bits of can_id */
        if (lowerbits == 0x10 && MCU::mcu.mcu_data.find(data_identifier) != MCU::mcu.mcu_data.end()) 
        {
            stored_data = MCU::mcu.mcu_data.at(data_identifier);
            /* Positive response */
            response.clear();
            response.push_back(0x03); /* PCI */
            response.push_back(0x62); /* Response sid = initial sid + 0x40 */
            response.push_back(request[2]); /* Data identifier, two bytes */
            response.push_back(request[3]);
            /* Append the data corresponding to the identifier */
            response.insert(response.end(), stored_data.begin(), stored_data.end());
        } 
        else if (lowerbits == 0x11 && battery.ecu_data.find(data_identifier) != battery.ecu_data.end()) 
        {
            /* fetch the data from linux */
            battery.fetchBatteryData();
            stored_data = battery.ecu_data.at(data_identifier);
            /** Positive response */
            response.clear();
            response.push_back(0x03); /* PCI */
            response.push_back(0x62); /* Response sid = initial sid + 0x40 */
            response.push_back(request[2]); /* Data identifier, two bytes */
            response.push_back(request[3]);
            /** Append the data corresponding to the identifier */
            response.insert(response.end(), stored_data.begin(), stored_data.end());
        } 
        else 
        {
            /** Data identifier not found */
            /** response.push_back(0x03); */
            /** response.push_back(0x7F); */ /** Negative response */
            /** response.push_back(RDBI_SERVICE_ID); */
            /** response.push_back(0x31); */ /** Request out of range */

            /* 0x31 NRC is not yet defined so I log it instead */
            LOG_ERROR(rdbi_logger.GET_LOGGER(), "Request out of range");
        }
    }
    /* Check if sender was API so MCU will send directly to API */
    if (upperbits == 0xFA) 
    {
        create_interface = create_interface->getInstance(0x00, rdbi_logger);
        if (create_interface) 
        {   
            generate_frames = new GenerateFrames(create_interface->getSocketApiWrite(), rdbi_logger);
            if(response.size() > 8)
            {
                generate_frames->readDataByIdentifierLongResponse(can_id, data_identifier, response, true);
            }
            else
            {
                generate_frames->readDataByIdentifier(can_id, data_identifier, response);
            }
        } 
        else 
        {
            /* Handle the case where create_interface is null */
            LOG_ERROR(rdbi_logger.GET_LOGGER(), "create_interface is nullptr");
        }
    } 
    else 
    {
        create_interface = create_interface->getInstance(0x00, rdbi_logger);
        if (create_interface) 
        {
            generate_frames = new GenerateFrames(create_interface->getSocketEcuWrite(), rdbi_logger);
            if(response.size() > 8)
            {
                generate_frames->readDataByIdentifierLongResponse(can_id, data_identifier, response, true);
            }
            else
            {
                generate_frames->readDataByIdentifier(can_id, data_identifier, response);
            }
        } 
        else 
        {
            /* Handle the case where create_interface is null */
            LOG_ERROR(rdbi_logger.GET_LOGGER(), "create_interface is nullptr");
        }
    }
    return response;
}