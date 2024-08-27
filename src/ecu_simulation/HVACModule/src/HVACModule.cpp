#include "../include/HVACModule.h"

Logger *hvacModuleLogger = nullptr;

HVACModule::HVACModule()
{
    initHVAC();
}

void HVACModule::initHVAC()
{
    _ecu = new ECU(HVAC_ECU_ID, _logger);

    /* Insert the default DID values in the file */
    std::ofstream outfile("hvac_data.txt");
    if (!outfile.is_open())
    {
        throw std::runtime_error("Failed to open file: hvac_data.txt");
    }

    for (const auto& [data_identifier, data] : hvac_data)
    {
        outfile << std::hex << std::setw(4) << std::setfill('0') << data_identifier << " ";
        for (uint8_t byte : data) 
        {
            outfile << std::hex << std::setw(2) << std::setfill('0') << static_cast<int>(byte) << " ";
        }
        outfile << "\n";
    }
    outfile.close();
}

int HVACModule::getSocket()
{
    return _ecu->_ecu_socket;
}

HVACModule::~HVACModule()
{
    delete _ecu;
}