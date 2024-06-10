#include "../include/BatteryModule.h"

/** Constructor - initializes the BatteryModule with default values,
 * sets up the CAN interface, and prepares the frame receiver. */
BatteryModule::BatteryModule() : moduleId(0x101),
                                 voltage(12.5),
                                 current(5.0),
                                 temperature(20.0),
                                 running(false),
                                 canInterface("vcan0"),
                                 frameReceiver(nullptr)
{
#ifdef BATTERY_MODULE_DEBUG
    std::cout << "BatteryModule()" << std::endl;
    std::cout << "(BatteryModule)moduleId = " << this->moduleId << std::endl;
#endif
    /* Initialize the CAN interface */
    canInterface.init();

    /* Initialize the Frame Receiver */
    frameReceiver = new ReceiveFrames(canInterface.getSocketFd(), moduleId);
}

/* Parameterized Constructor - initializes the BatteryModule with provided interface number and module ID */
BatteryModule::BatteryModule(int _interfaceNumber, int _moduleId) : moduleId(_moduleId),
                                                                    voltage(12.5),
                                                                    current(5.0),
                                                                    temperature(20.0),
                                                                    running(false),
                                                                    canInterface("vcan" + std::to_string(_interfaceNumber)),
                                                                    frameReceiver(nullptr)
{
#ifdef BATTERY_MODULE_DEBUG
    std::cout << "BatteryModule(int interfaceNumber, int moduleId)" << std::endl;
    std::cout << "(BatteryModule)moduleId = " << this->moduleId << std::endl;
#endif
    /* Initialize the CAN interface */
    canInterface.init();

    /* Initialize the Frame Receiver */
    frameReceiver = new ReceiveFrames(canInterface.getSocketFd(), moduleId);
}

/* Destructor */
BatteryModule::~BatteryModule()
{
    stopBatteryModule();
    delete frameReceiver;
}

/* Start the simulation of battery */
void BatteryModule::simulate()
{
    /* Set the 'running' flag */
    running = true;
    /** Launch a new thread to run the startBatteryModule() method,
     *   which handles the simulation loop, in the context of the current BatteryModule instance */
    simulationThread = std::thread(&BatteryModule::startBatteryModule, this);
}

/* Run the battery module simulation loop */
void BatteryModule::startBatteryModule()
{
    while (running)
    {
        updateParamenters();
        std::this_thread::sleep_for(std::chrono::seconds(1));
    }
}

/* Stop the simulation and join the thread */
void BatteryModule::stopBatteryModule()
{
    /* Un-set the 'running' flag */
    running = false;

    /** Check if the simulation thread is joinable (i.e., it's running and not yet joined)
     *   If it is, wait for the thread to finish execution and join it with the main thread */
    if (simulationThread.joinable())
    {
        simulationThread.join();
    }
}

void BatteryModule::updateParamenters()
{
#ifdef BATTERY_MODULE_DEBUG
    std::cout << "Battery Module - updateParamenters()" << std::endl;
#endif
    /** Simulate some logic to update voltage, current, and temperature.
     *   This will change in further versions, so the readings will come from Laptop's battery */
    voltage += BATTERY_MODULE_PARAM_INCREMENT;
    current += BATTERY_MODULE_PARAM_INCREMENT;
    temperature += BATTERY_MODULE_PARAM_INCREMENT;
#ifdef BATTERY_MODULE_DEBUG
    std::cout << "Voltage : " << voltage << std::endl;
    std::cout << "Current : " << current << std::endl;
    std::cout << "Temperature : " << temperature << std::endl;
#endif
    /* Ensuring the parameters don't exceed certain values for simulation */
    if (voltage > BATTERY_MODULE_MAX_VOLTAGE)
    {
        voltage = BATTERY_MODULE_INIT_VOLTAGE;
    }

    if (current > BATTERY_MODULE_MAX_CURRENT)
    {
        current = BATTERY_MODULE_INIT_CURRENT;
    }

    if (temperature > BATTERY_MODULE_MAX_TEMP)
    {
        temperature = BATTERY_MODULE_INIT_TEMP;
    }
}

/* Function to receive CAN frames */
void BatteryModule::receiveFrames()
{
    /* Create a HandleFrames object to process received frames */
    HandleFrames handleFrames;

    /* Receive a CAN frame using the frame receiver and process it with handleFrames */
    frameReceiver->Receive(handleFrames);

    /* Sleep for 100 milliseconds before receiving the next frame to prevent busy-waiting */
    std::this_thread::sleep_for(std::chrono::milliseconds(100));
}

/* Functon to Stop receiving frames */
void BatteryModule::stopFrames()
{
    frameReceiver->Stop();
}

/* Getter function for voltage */
float BatteryModule::getVoltage() const
{
    return voltage;
}

/* Getter function for current */
float BatteryModule::getCurrent() const
{
    return current;
}

/* Getter function for temperature */
float BatteryModule::getTemperature() const
{
    return temperature;
}