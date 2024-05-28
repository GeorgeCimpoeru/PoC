cmake_minimum_required(VERSION 3.10)

project(BatteryModuleTest)

# Set the C++ standard
set(CMAKE_CXX_STANDARD 17)

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

find_package(GTest REQUIRED)
include_directories(${GTEST_INCLUDE_DIRS})

# Create the test executable
add_executable(testFile InterfaceConfigTest.cpp ${SOURCES})
add_executable(test_battery_module BatteryModuleTest.cpp ${SOURCES})

# Link the test executable with Google Test and pthread
target_link_libraries(testFile gtest gtest_main pthread)
target_link_libraries(test_battery_module gtest gtest_main pthread)