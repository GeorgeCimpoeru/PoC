cmake_minimum_required(VERSION 3.10)

project(BatteryModuleTest)

# Set the C++ standard
set(CMAKE_CXX_STANDARD 11)

# Add the BatteryModule sources
set(SOURCES
    ../src/BatteryModule.cpp
    ../src/GenerateFrames.cpp
    ../src/HandleFrames.cpp
    ../src/InterfaceConfig.cpp
    ../src/ReceiveFrames.cpp
)

# Add the include directory
include_directories(../include)

# Add the Google Test subdirectory
add_subdirectory(/usr/src/gtest gtest)

# Create the test executable
add_executable(test_battery_module BatteryModule_Test.cpp ${SOURCES})

# Link the test executable with Google Test and pthread
target_link_libraries(test_battery_module gtest gtest_main pthread)
