#include "../include/HVACModule.h"

Logger *hvacModuleLogger = nullptr;

HVACModule::HVACModule()
{
    _ecu = new ECU(HVAC_ECU_ID, _logger);
}

int HVACModule::getSocket()
{
    return _ecu->_ecu_socket;
}

HVACModule::~HVACModule()
{
    delete _ecu;
}