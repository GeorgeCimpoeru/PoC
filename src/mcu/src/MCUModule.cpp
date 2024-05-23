#include "../include/MCUModule.h"
#include <thread>

// Constructor
MCUModule::MCUModule(int interfaceNumber) : 
                interfaceModule("vcan" + std::to_string(interfaceNumber)), 
                isRunning(false),
                receiveFrames(nullptr) {
    interfaceModule.create_interface();
    interfaceModule.start_interface();
    receiveFrames = new ReceiveFrames(interfaceModule.get_socket());
}

MCUModule::MCUModule() : interfaceModule("vcan0"), 
                                            isRunning(false),
                                            receiveFrames(nullptr) {}

MCUModule::~MCUModule() {
    interfaceModule.stop_interface();
    delete receiveFrames;
}

void MCUModule::StartModule() { isRunning = true; }

void MCUModule::StopModule() { isRunning = false; }
