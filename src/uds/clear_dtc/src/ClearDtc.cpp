#include "../include/ClearDtc.h"
#include "../../../ecu_simulation/BatteryModule/include/BatteryModule.h"
#include "../../../mcu/include/MCUModule.h"

ClearDtc::ClearDtc(std::string path_to_dtc, Logger& logger, int socket)
{
    this->path_to_dtc = path_to_dtc;
    this->logger = &logger;
    this->socket = socket;
}

void ClearDtc::clearDtc(int id, std::vector<uint8_t> data)
{
    LOG_INFO(logger->GET_LOGGER(), "Clear DTC Service");

    this->generate = new GenerateFrames(socket,*logger);
    uint8_t lowerbits = id & 0xFF;
    uint8_t upperbits = id >> 8 & 0xFF;
    uint32_t group_of_dtc = data[2] * 0x10000 + data[3] * 0x100 + data[4];
    int new_id = ((lowerbits << 8) | upperbits);

    /* NRC 0x13 */
    if (data.size() != 5)
    {
        LOG_ERROR(logger->GET_LOGGER(), "Incorrect message length or invalid format");
        this->generate->negativeResponse(new_id, 0x14, 0x13);
        return;
    }

    /* NRC 0x31 */
    if (!verifyGroupDtc(group_of_dtc))
    {
        LOG_ERROR(logger->GET_LOGGER(), "RequestOutOfRange NRC:Specified Group of DTC parameter is not supported");
        this->generate->negativeResponse(new_id, 0x14, 0x31);
    }

    std::ofstream temp;
    temp.open("temp_dtc.txt");
    /* Clean all DTCs */
    if (group_of_dtc == 0xFFFFFF)
    {
        temp.close();
        const char * p = path_to_dtc.c_str();
        remove(p);
        rename("temp_dtc.txt", p);

        switch(lowerbits)
        {
            case 0x10:
                /* Send response frame */
                this->generate->clearDiagnosticInformation(new_id,{}, true);
                LOG_INFO(logger->GET_LOGGER(), "Service with SID {:x} successfully sent the response frame.", 0x14);
                MCU::mcu->stop_flags[0x14] = false;
                break;
            case 0x11:
                /* Send response frame */
                this->generate->clearDiagnosticInformation(new_id,{}, true);
                LOG_INFO(logger->GET_LOGGER(), "Service with SID {:x} successfully sent the response frame.", 0x14);
                battery->stop_flags[0x14] = false;
                break;
            default:
                LOG_ERROR(logger->GET_LOGGER(), "Module with id {:x} not supported.", lowerbits);
        }
        return;
    }

    std::ifstream file_dtc;
    try
    {
        file_dtc.open(this->path_to_dtc);
        if (!file_dtc.is_open())
        {
            throw std::runtime_error("Unable to open file");
        }
    }
    catch(const std::exception& e)
    {
        LOG_ERROR(logger->GET_LOGGER(), "conditionsNotCorrect NRC: Error trying to open the DTC file");
        this->generate->negativeResponse(new_id, 0x14, 0x22);
        return;
    }
    std::string line;
    while (std::getline(file_dtc,line))
    {
        if ( extractGroup(line) != group_of_dtc)
        {
            temp << line << std::endl;
        }
    }
    temp.close();
    file_dtc.close();

    const char * p = path_to_dtc.c_str();
    remove(p);
    rename("temp_dtc.txt", p);

    switch(lowerbits)
    {
        case 0x10:
            /* Send response frame */
            LOG_INFO(logger->GET_LOGGER(), "DTCs cleared succesffuly");
            this->generate->clearDiagnosticInformation(new_id,{}, true);
            MCU::mcu->stop_flags[0x14] = false;
            break;
        case 0x11:
            /* Send response frame */
            LOG_INFO(logger->GET_LOGGER(), "DTCs cleared succesffuly");
            this->generate->clearDiagnosticInformation(new_id,{}, true);
            battery->stop_flags[0x14] = false;
            break;
        default:
            LOG_ERROR(logger->GET_LOGGER(), "Module with id {:x} not supported.", lowerbits);
    }
}

uint32_t ClearDtc::extractGroup(std::string dtc)
{
    int group = 0;
    std::map<char,uint8_t> first_byte =
    {
        {'P',00}, {'C',01}, {'B',2}, {'U',3}
    };
    int hex = (first_byte[dtc[0]]);
    group = hex * 0x10000 + 0xAAA;
    return group;
}

bool ClearDtc::verifyGroupDtc(uint32_t group_of_dtc)
{
    std::vector<uint32_t> group_of_dtcs = {0xAAA, 0x010AAA, 0x020AAA,0x030AAA, 0xFFFFFF};
    for ( auto dtc : group_of_dtcs)
    {
        if (group_of_dtc == dtc)
        {
            return true;
        }
    }
    return false;
}