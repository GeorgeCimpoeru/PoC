#include "../include/ClearDtc.h"
#include "../../../ecu_simulation/BatteryModule/include/BatteryModule.h"
#include "../../../ecu_simulation/EngineModule/include/EngineModule.h"
#include "../../../ecu_simulation/DoorsModule/include/DoorsModule.h"
#include "../../../ecu_simulation/HVACModule/include/HVACModule.h"
#include "../../../mcu/include/MCUModule.h"

ClearDtc::ClearDtc(std::string path_to_dtc, Logger& logger, int socket)
                : path_to_dtc(path_to_dtc), logger(logger), socket(socket)
{

}

void ClearDtc::clearDtc(int id, std::vector<uint8_t> data)
{
    LOG_INFO(logger.GET_LOGGER(), "Clear DTC Service");

    this->generate = new GenerateFrames(socket, logger);
    uint8_t lowerbits = id & 0xFF;
    uint8_t upperbits = id >> 8 & 0xFF;
    uint32_t group_of_dtc = data[2] * 0x10000 + data[3] * 0x100 + data[4];
    int new_id = ((lowerbits << 8) | upperbits);

    /* NRC 0x13 */
    if (data.size() != 5)
    {
        LOG_ERROR(logger.GET_LOGGER(), "Incorrect message length or invalid format");
        this->generate->negativeResponse(new_id, 0x14, 0x13);
        AccessTimingParameter::stopTimingFlag(lowerbits, 0x14);
        return;
    }

    /* NRC 0x31 */
    if (!verifyGroupDtc(group_of_dtc))
    {
        LOG_ERROR(logger.GET_LOGGER(), "RequestOutOfRange NRC:Specified Group of DTC parameter is not supported");
        this->generate->negativeResponse(new_id, 0x14, 0x31);
        AccessTimingParameter::stopTimingFlag(lowerbits, 0x14);
        return;
    }

    /* Read existing DTCs from the file */
    std::ifstream input_file(this->path_to_dtc);
    if (!input_file.is_open())
    {
        LOG_ERROR(logger.GET_LOGGER(), "conditionsNotCorrect NRC: Error trying to open the DTC file");
        this->generate->negativeResponse(new_id, 0x14, 0x22);
        AccessTimingParameter::stopTimingFlag(lowerbits, 0x14);
        return;
    }

    std::string line;
    std::vector<std::string> remaining_dtcs;

    while (std::getline(input_file, line))
    {
        /* If the DTC does not belong to the group that needs to be cleared, keep it */
        if (extractGroup(line) != group_of_dtc)
        {
            remaining_dtcs.push_back(line);
        }
    }

    input_file.close();

    std::ofstream file_dtc;
    if (group_of_dtc != 0xFFFFFF)
    {
        file_dtc.open(this->path_to_dtc, std::ios::out | std::ios::trunc);

        /* Now overwrite the file with only the remaining DTCs */
        for (const auto& dtc : remaining_dtcs)
        {
            file_dtc << dtc << std::endl;
        }

        file_dtc.close();
    }
    else
    {
        /* Open the file for writing, clearing its contents */
        file_dtc.open(this->path_to_dtc, std::ios::out | std::ios::trunc);  
        file_dtc.close();
    }

    LOG_INFO(logger.GET_LOGGER(), "DTCs cleared successfully");
    this->generate->clearDiagnosticInformation(new_id, {}, true);
    AccessTimingParameter::stopTimingFlag(lowerbits, 0x14);
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
    std::vector<uint32_t> group_of_dtcs = {0x000AAA, 0x010AAA, 0x020AAA,0x030AAA, 0xFFFFFF};
    for ( auto dtc : group_of_dtcs)
    {
        if (group_of_dtc == dtc)
        {
            return true;
            LOG_INFO(logger.GET_LOGGER(), "Grupul e valid");
        }
    }
    return false;
}