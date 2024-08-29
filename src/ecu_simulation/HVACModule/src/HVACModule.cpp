#include "../include/HVACModule.h"

Logger *hvacModuleLogger = nullptr;
HVACModule *hvac = nullptr;

HVACModule::HVACModule() : _logger(*hvacModuleLogger)
{
    initHVAC();
}

void HVACModule::initHVAC()
{
    /* ECU object responsible for socket and frames (communication) */
    _ecu = new ECU(HVAC_ECU_ID, _logger);
    writeDataToFile();
}

void HVACModule::fetchHvacData()
{
    generateData();
    writeDataToFile();

    LOG_INFO(_logger.GET_LOGGER(), "HVAC data file updated with random values.");
}

void HVACModule::generateData()
{
    /* Seed for the random number generator */
    std::srand(static_cast<unsigned>(std::time(nullptr)));

    /* Ambient Max Temperature is 50 */
    default_DID_hvac[AMBIENT_TEMPERATURE_DID].front() = static_cast<uint8_t>((std::rand() % HVAC_AMBIENT_TEMPERATURE_MOD) + HVAC_MIN_AMBIENT_TEMPERATURE);
    /* Cabin Max Temperature is 30 */
    default_DID_hvac[CABIN_TEMPERATURE_DID].front() = static_cast<uint8_t>((std::rand() % HVAC_CABIN_TEMPERATURE_MOD) + HVAC_MIN_CABIN_TEMPERATURE);
    /* HVAC Max Temperature is between 16 and 25 degrees */
    default_DID_hvac[HVAC_SET_TEMPERATURE_DID].front() = static_cast<uint8_t>((std::rand() % HVAC_SET_TEMPERATURE_MOD) + HVAC_MIN_SET_TEMPERATURE);
    /* Max Fan Speed is 100 */
    default_DID_hvac[FAN_SPEED_DID].front() = static_cast<uint8_t>((std::rand() % HVAC_FAN_SPEED_MOD) + HVAC_MIN_FAN_SPEED);
    /* Each bit represents a mode, no specific logic */
    default_DID_hvac[HVAC_MODES_DID].front() = static_cast<uint8_t>(std::rand() % HVAC_MODES_MOD);
}

void HVACModule::writeDataToFile()
{
    /* Insert the default DID values in the file */
    std::ofstream hvac_data_file("hvac_data.txt");
    if (!hvac_data_file.is_open())
    {
        throw std::runtime_error("Failed to open file: hvac_data.txt");
    }

    for (const auto& [data_identifier, data] : default_DID_hvac)
    {
        hvac_data_file << std::hex << std::setw(4) << std::setfill('0') << data_identifier << " ";
        for (uint8_t byte : data) 
        {
            hvac_data_file << std::hex << std::setw(2) << std::setfill('0') << static_cast<int>(byte) << " ";
        }
        hvac_data_file << "\n";
    }
    hvac_data_file.close();
}

void HVACModule::printHvacInfo()
{
    uint8_t fan_speed_level = static_cast<uint8_t>((default_DID_hvac[FAN_SPEED_DID].front() / HVAC_MAX_FAN_SPEED) * HVAC_MAX_FAN_SPEED_LEVELS);
    uint8_t hvac_modes = default_DID_hvac[HVAC_MODES_DID].front();

    LOG_INFO(_logger.GET_LOGGER(), "\n----------HVAC INFO-----------\n"
    "Ambient temperature is {}C\n"
    "Cabin temperature is {}C\n"
    "Cabin temperature is set to {}C\n"
    "Fan speed is set to level {}\n"
    "HVAC current operating modes are:\n"
    "   AC STATUS:         {}\n"
    "   LEGS:              {}\n"
    "   FRONT:             {}\n"
    "   AIR RECIRCULATION: {}\n"
    "   DEFROST:           {}\n"
    "   Reserved b5:       {}\n"
    "   Reserved b6:       {}\n"
    "   Reserved b7:       {}",
    default_DID_hvac[AMBIENT_TEMPERATURE_DID].front(),
    default_DID_hvac[CABIN_TEMPERATURE_DID].front(),
    default_DID_hvac[HVAC_SET_TEMPERATURE_DID].front(),
    fan_speed_level,
    (hvac_modes & AC_STATUS) ? "ON" : "OFF",
    (hvac_modes & LEGS) ? "ENABLED" : "DISABLED",
    (hvac_modes & FRONT) ? "ENABLED" : "DISABLED",
    (hvac_modes & AIR_RECIRCULATION) ? "ON" : "OFF",
    (hvac_modes & DEFROST) ? "ON" : "OFF",
    (hvac_modes & RESERVED_B5) ? "ENABLED" : "DISABLED",
    (hvac_modes & RESERVED_B6) ? "ENABLED" : "DISABLED",
    (hvac_modes & RESERVED_B7) ? "ENABLED" : "DISABLED");
    
}
int HVACModule::getSocket()
{
    return _ecu->_ecu_socket;
}

HVACModule::~HVACModule()
{
    delete _ecu;
}