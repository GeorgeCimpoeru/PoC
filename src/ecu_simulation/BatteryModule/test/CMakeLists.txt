cmake_minimum_required(VERSION 3.10)

project(BatteryModuleTest)

# Set the C++ standard
set(CMAKE_CXX_STANDARD 17)

# Add the directory containing the spdlog headers
include_directories(/usr/include/spdlog)

# Find the spdlog package
find_package(spdlog REQUIRED)

# Add the BatteryModule sources
set(SOURCES
    ../src/BatteryModule.cpp
    ../src/GenerateFrames.cpp
    ../src/HandleFrames.cpp
    ../src/InterfaceConfig.cpp
    ../src/ReceiveFrames.cpp
    ../../../utils/Logger/src/Logger.cpp
)

# Add the include directory
include_directories(../include)

# Find Google Test
find_package(GTest REQUIRED)
include_directories(${GTEST_INCLUDE_DIRS})

# Add coverage flags
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fprofile-arcs -ftest-coverage -O0 -g")
set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -fprofile-arcs -ftest-coverage")

# Define the Logger library (if it's not already defined elsewhere)
add_library(Logger STATIC ../../../utils/Logger/src/Logger.cpp)

# Create the test executables
add_executable(testFile InterfaceConfigTest.cpp ${SOURCES})
add_executable(test_battery_module BatteryModuleTest.cpp ${SOURCES})

# Link the testFile executable with Google Test and pthread
target_link_libraries(testFile gtest gtest_main pthread spdlog::spdlog)

# Link the test_battery_module executable with Logger, spdlog, Google Test, and pthread
target_link_libraries(test_battery_module PRIVATE Logger spdlog::spdlog gtest gtest_main pthread)

# Ensure SPDLOG_COMPILED_LIB is defined in the CMake configuration
target_compile_definitions(testFile PRIVATE SPDLOG_COMPILED_LIB)
target_compile_definitions(test_battery_module PRIVATE SPDLOG_COMPILED_LIB)
