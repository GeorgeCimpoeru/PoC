######### Makefile for MCU module
######### Author: Theodor Stoica (contact theodor.stoica@randstaddigital.com for any help)

#POC Project root path
PROJECT_PATH := $(shell cd $(shell pwd)/../../ && pwd)
SOFTWARE_VERSION = 0
PROJECT_DEFINES = -DPROJECT_PATH=\"$(PROJECT_PATH)\" -DSOFTWARE_VERSION=${SOFTWARE_VERSION}

#Python flags needed for pybing library
PYTHON_INCL_DIR=-I/usr/include/python3.8
PYTHON_LDFLAGS = -L/usr/lib/python3.8/config-3.8-x86_64-linux-gnu -lpython3.8 -lcrypt -ldl -lutil -lm -lpthread

# Compiler flags
CXX = g++
CFLAGS = -std=c++17 -g -Wall -Werror -pthread -I./include -I../utils/include ${PYTHON_INCL_DIR} ${PROJECT_DEFINES}
LDFLAGS = -lspdlog -lfmt -lzip ${PYTHON_LDFLAGS}

# Test flags
CFLAGSTST = -std=c++17 -I/include -I/usr/include/python3.8 -fprofile-arcs -ftest-coverage
CFLAGSTST2 = -lgtest -lgtest_main -lpthread
CFLAGSTST += -DUNIT_TESTING_MODE

# Directories
SRC_DIR = src
SRC_TEST = test
UTILS_DIR = ../utils/src
UTILS_TEST = ../utils/test
UDS_DIR = ../uds
BATTERY_DIR = ../ecu_simulation/BatteryModule/src
ENGINE_DIR = ../ecu_simulation/EngineModule/src
DOORS_DIR = ../ecu_simulation/DoorsModule/src
HVAC_DIR = ../ecu_simulation/HVACModule/src
OTA_DIR = ../ota
OBJ_DIR = obj

#----------------------------------------------------MCU MODULE-----------------------------------------------------

# Object files
MCU_OBJS = $(OBJ_DIR)/main.o \
           $(OBJ_DIR)/MCUModule.o \
           $(OBJ_DIR)/ReceiveFrames.o

# Battery object files
BATTERY_OBJS = $(OBJ_DIR)/BatteryModule.o \

# Engine object files
ENGINE_OBJS = $(OBJ_DIR)/EngineModule.o \

# Doors object files
DOORS_OBJS = $(OBJ_DIR)/DoorsModule.o \

# HVAC object files
HVAC_OBJS = $(OBJ_DIR)/HVACModule.o \

# Utils object files
UTILS_OBJS = $(OBJ_DIR)/MemoryManager.o \
             $(OBJ_DIR)/Logger.o \
             $(OBJ_DIR)/GenerateFrames.o \
             $(OBJ_DIR)/CreateInterface.o \
             $(OBJ_DIR)/NegativeResponse.o \
			 $(OBJ_DIR)/ECU_ReceiveFrames.o \
			 $(OBJ_DIR)/HandleFrames.o \
			 $(OBJ_DIR)/FileManager.o \
			 $(OBJ_DIR)/ECU.o

# UDS object files
UDS_OBJS = $(OBJ_DIR)/DiagnosticSessionControl.o \
           $(OBJ_DIR)/ReadDataByIdentifier.o \
           $(OBJ_DIR)/WriteDataByIdentifier.o \
           $(OBJ_DIR)/EcuReset.o \
           $(OBJ_DIR)/SecurityAccess.o \
		   $(OBJ_DIR)/TesterPresent.o \
           $(OBJ_DIR)/ReadDtcInformation.o \
           $(OBJ_DIR)/ClearDtc.o \
           $(OBJ_DIR)/RoutineControl.o \
		   $(OBJ_DIR)/AccessTimingParameter.o \
		   $(OBJ_DIR)/ReadMemoryByAddress.o

# OTA object files
OTA_OBJS = $(OBJ_DIR)/RequestUpdateStatus.o \
           $(OBJ_DIR)/RequestTransferExit.o \
           $(OBJ_DIR)/RequestDownload.o \
           $(OBJ_DIR)/TransferData.o
           
OBJS = $(MCU_OBJS) \
       $(BATTERY_OBJS) \
	   $(ENGINE_OBJS) \
	   $(DOORS_OBJS) \
	   $(HVAC_OBJS) \
       $(UTILS_OBJS) \
       $(UDS_OBJS) \
       $(OTA_OBJS)


# Target executables
FINAL = main_mcu

.PHONY: all clean

all: $(OBJ_DIR) $(FINAL)

$(FINAL): $(OBJS)
	$(CXX) $(CFLAGS) -o $(FINAL) $(OBJS) $(LDFLAGS)

$(OBJ_DIR):
	mkdir -p $(OBJ_DIR)

# Compile individual source files into object files
$(OBJ_DIR)/main.o: $(SRC_DIR)/main.cpp
	$(CXX) $(CFLAGS) -c $(SRC_DIR)/main.cpp -o $(OBJ_DIR)/main.o

$(OBJ_DIR)/MCUModule.o: $(SRC_DIR)/MCUModule.cpp
	$(CXX) $(CFLAGS) -c $(SRC_DIR)/MCUModule.cpp -o $(OBJ_DIR)/MCUModule.o

$(OBJ_DIR)/BatteryModule.o: $(BATTERY_DIR)/BatteryModule.cpp
	$(CXX) $(CFLAGS) -c $(BATTERY_DIR)/BatteryModule.cpp -o $(OBJ_DIR)/BatteryModule.o

$(OBJ_DIR)/EngineModule.o: $(ENGINE_DIR)/EngineModule.cpp
	$(CXX) $(CFLAGS) -c $(ENGINE_DIR)/EngineModule.cpp -o $(OBJ_DIR)/EngineModule.o

$(OBJ_DIR)/DoorsModule.o: $(DOORS_DIR)/DoorsModule.cpp
	$(CXX) $(CFLAGS) -c $(DOORS_DIR)/DoorsModule.cpp -o $(OBJ_DIR)/DoorsModule.o


$(OBJ_DIR)/HVACModule.o: $(HVAC_DIR)/HVACModule.cpp
	$(CXX) $(CFLAGS) -c $(HVAC_DIR)/HVACModule.cpp -o $(OBJ_DIR)/HVACModule.o

$(OBJ_DIR)/ECU.o: $(UTILS_DIR)/ECU.cpp
	$(CXX) $(CFLAGS) -c $(UTILS_DIR)/ECU.cpp -o $(OBJ_DIR)/ECU.o

$(OBJ_DIR)/GenerateFrames.o: $(UTILS_DIR)/GenerateFrames.cpp
	$(CXX) $(CFLAGS) -c $(UTILS_DIR)/GenerateFrames.cpp -o $(OBJ_DIR)/GenerateFrames.o

$(OBJ_DIR)/HandleFrames.o: $(UTILS_DIR)/HandleFrames.cpp
	$(CXX) $(CFLAGS) -c $(UTILS_DIR)/HandleFrames.cpp -o $(OBJ_DIR)/HandleFrames.o

$(OBJ_DIR)/CreateInterface.o: $(UTILS_DIR)/CreateInterface.cpp
	$(CXX) $(CFLAGS) -c $(UTILS_DIR)/CreateInterface.cpp -o $(OBJ_DIR)/CreateInterface.o

$(OBJ_DIR)/DiagnosticSessionControl.o: $(UDS_DIR)/diagnostic_session_control/src/DiagnosticSessionControl.cpp
	$(CXX) $(CFLAGS) -c $(UDS_DIR)/diagnostic_session_control/src/DiagnosticSessionControl.cpp -o $(OBJ_DIR)/DiagnosticSessionControl.o

$(OBJ_DIR)/ReceiveFrames.o: $(SRC_DIR)/ReceiveFrames.cpp
	$(CXX) $(CFLAGS) -c $(SRC_DIR)/ReceiveFrames.cpp -o $(OBJ_DIR)/ReceiveFrames.o

$(OBJ_DIR)/ReadDataByIdentifier.o: $(UDS_DIR)/read_data_by_identifier/src/ReadDataByIdentifier.cpp
	$(CXX) $(CFLAGS) -c $(UDS_DIR)/read_data_by_identifier/src/ReadDataByIdentifier.cpp -o $(OBJ_DIR)/ReadDataByIdentifier.o

$(OBJ_DIR)/WriteDataByIdentifier.o: $(UDS_DIR)/write_data_by_identifier/src/WriteDataByIdentifier.cpp
	$(CXX) $(CFLAGS) -c $(UDS_DIR)/write_data_by_identifier/src/WriteDataByIdentifier.cpp -o $(OBJ_DIR)/WriteDataByIdentifier.o

$(OBJ_DIR)/EcuReset.o: $(UDS_DIR)/ecu_reset/src/EcuReset.cpp
	$(CXX) $(CFLAGS) -c $(UDS_DIR)/ecu_reset/src/EcuReset.cpp -o $(OBJ_DIR)/EcuReset.o

$(OBJ_DIR)/SecurityAccess.o: $(UDS_DIR)/authentication/src/SecurityAccess.cpp
	$(CXX) $(CFLAGS) -c $(UDS_DIR)/authentication/src/SecurityAccess.cpp -o $(OBJ_DIR)/SecurityAccess.o

$(OBJ_DIR)/TesterPresent.o: $(UDS_DIR)/tester_present/src/TesterPresent.cpp
	$(CXX) $(CFLAGS) -c $(UDS_DIR)/tester_present/src/TesterPresent.cpp -o $(OBJ_DIR)/TesterPresent.o

$(OBJ_DIR)/ReadMemoryByAddress.o: $(UDS_DIR)/read_memory_by_address/src/ReadMemoryByAddress.cpp
	$(CXX) $(CFLAGS) -c $(UDS_DIR)/read_memory_by_address/src/ReadMemoryByAddress.cpp -o $(OBJ_DIR)/ReadMemoryByAddress.o

$(OBJ_DIR)/ECU_ReceiveFrames.o: $(UTILS_DIR)/ReceiveFrames.cpp
	$(CXX) $(CFLAGS) -c $(UTILS_DIR)/ReceiveFrames.cpp -o $(OBJ_DIR)/ECU_ReceiveFrames.o

$(OBJ_DIR)/ReadDtcInformation.o: $(UDS_DIR)/read_dtc_information/src/ReadDtcInformation.cpp
	$(CXX) $(CFLAGS) -c $(UDS_DIR)/read_dtc_information/src/ReadDtcInformation.cpp -o $(OBJ_DIR)/ReadDtcInformation.o

$(OBJ_DIR)/RequestUpdateStatus.o: $(OTA_DIR)/request_update_status/src/RequestUpdateStatus.cpp
	$(CXX) $(CFLAGS) -c $(OTA_DIR)/request_update_status/src/RequestUpdateStatus.cpp -o $(OBJ_DIR)/RequestUpdateStatus.o

$(OBJ_DIR)/RequestTransferExit.o: $(OTA_DIR)/request_transfer_exit/src/RequestTransferExit.cpp
	$(CXX) $(CFLAGS) -c $(OTA_DIR)/request_transfer_exit/src/RequestTransferExit.cpp -o $(OBJ_DIR)/RequestTransferExit.o

$(OBJ_DIR)/RequestDownload.o: $(OTA_DIR)/request_download/src/RequestDownload.cpp
	$(CXX) $(CFLAGS) -c $(OTA_DIR)/request_download/src/RequestDownload.cpp -o $(OBJ_DIR)/RequestDownload.o

$(OBJ_DIR)/MemoryManager.o: $(UTILS_DIR)/MemoryManager.cpp
	$(CXX) $(CFLAGS) -c $(UTILS_DIR)/MemoryManager.cpp -o $(OBJ_DIR)/MemoryManager.o

$(OBJ_DIR)/FileManager.o: $(UTILS_DIR)/FileManager.cpp
	$(CXX) $(CFLAGS) -c $(UTILS_DIR)/FileManager.cpp -o $(OBJ_DIR)/FileManager.o

$(OBJ_DIR)/TransferData.o: $(OTA_DIR)/transfer_data/src/TransferData.cpp
	$(CXX) $(CFLAGS) -c $(OTA_DIR)/transfer_data/src/TransferData.cpp -o $(OBJ_DIR)/TransferData.o

$(OBJ_DIR)/Logger.o: $(UTILS_DIR)/Logger.cpp
	$(CXX) $(CFLAGS) -c $(UTILS_DIR)/Logger.cpp -o $(OBJ_DIR)/Logger.o

$(OBJ_DIR)/ClearDtc.o: $(UDS_DIR)/clear_dtc/src/ClearDtc.cpp
	$(CXX) $(CFLAGS) -c $(UDS_DIR)/clear_dtc/src/ClearDtc.cpp -o $(OBJ_DIR)/ClearDtc.o

$(OBJ_DIR)/RoutineControl.o: $(UDS_DIR)/routine_control/src/RoutineControl.cpp
	$(CXX) $(CFLAGS) -c $(UDS_DIR)/routine_control/src/RoutineControl.cpp -o $(OBJ_DIR)/RoutineControl.o

$(OBJ_DIR)/AccessTimingParameter.o: $(UDS_DIR)/access_timing_parameters/src/AccessTimingParameter.cpp
	$(CXX) $(CFLAGS) -c $(UDS_DIR)/access_timing_parameters/src/AccessTimingParameter.cpp -o $(OBJ_DIR)/AccessTimingParameter.o
  
$(OBJ_DIR)/NegativeResponse.o: $(UTILS_DIR)/NegativeResponse.cpp
	$(CXX) $(CFLAGS) -c $(UTILS_DIR)/NegativeResponse.cpp -o $(OBJ_DIR)/NegativeResponse.o

#----------------------------------------------------UNIT TESTS-----------------------------------------------------

MCU_OBJS_TEST = $(OBJ_DIR)/MCUModule_test.o \
                $(OBJ_DIR)/ReceiveFrames_test.o

ECU_OBJS_TEST = $(OBJ_DIR)/BatteryModule_test.o \
                $(OBJ_DIR)/ECU_ReceiveFrames_test.o \
                $(OBJ_DIR)/EngineModule_test.o \
                $(OBJ_DIR)/DoorsModule_test.o \
                $(OBJ_DIR)/HVACModule_test.o \
                $(OBJ_DIR)/ECU_test.o

UTILS_OBJS_TEST = $(OBJ_DIR)/MemoryManager_test.o \
                  $(OBJ_DIR)/Logger_test.o \
                  $(OBJ_DIR)/CreateInterface_test.o \
                  $(OBJ_DIR)/GenerateFrames_test.o \
                  $(OBJ_DIR)/NegativeResponse_test.o \
                  $(OBJ_DIR)/HandleFrames_test.o \
                  $(OBJ_DIR)/FileManager_test.o

OBJS_GENERATE_TEST = $(OBJ_DIR)/Logger_test.o \
                  	 $(OBJ_DIR)/GenerateFrames_test.o

OBJS_MEMORY_TEST =   $(OBJ_DIR)/Logger_test.o \
                  	 $(OBJ_DIR)/MemoryManager_test.o

OBJS_CREATEINTERFACE_TEST = $(OBJ_DIR)/Logger_test.o \
                      		$(OBJ_DIR)/CreateInterface_test.o
                      		
OBJS_LOGGER_TEST = $(OBJ_DIR)/Logger_test.o \

UDS_OBJS_TEST = $(OBJ_DIR)/DiagnosticSessionControl_test.o \
                $(OBJ_DIR)/ReadDataByIdentifier_test.o \
                $(OBJ_DIR)/WriteDataByIdentifier_test.o \
                $(OBJ_DIR)/EcuReset_test.o \
                $(OBJ_DIR)/SecurityAccess_test.o \
                $(OBJ_DIR)/TesterPresent_test.o \
                $(OBJ_DIR)/ReadDtcInformation_test.o \
                $(OBJ_DIR)/ClearDtc_test.o \
                $(OBJ_DIR)/RoutineControl_test.o \
                $(OBJ_DIR)/AccessTimingParameter_test.o \
                $(OBJ_DIR)/ReadMemoryByAddress_test.o 

OBJS_DIAGNOSTICSESSIONCONTROL_TEST = $(OBJ_DIR)/Logger_test.o \
                                     $(OBJ_DIR)/GenerateFrames_test.o \
                                     $(OBJ_DIR)/DiagnosticSessionControl_test.o \
									 $(OBJ_DIR)/AccessTimingParameter_test.o \
									 $(OBJ_DIR)/NegativeResponse_test.o

OBJS_SECURITYACCESS_TEST = $(OBJ_DIR)/Logger_test.o \
						   $(OBJ_DIR)/GenerateFrames_test.o \
			   			   $(OBJ_DIR)/SecurityAccess_test.o \
			   			   $(OBJ_DIR)/NegativeResponse_test.o

OBJS_TESTERPRESENT_TEST = $(OBJ_DIR)/Logger_test.o \
						  $(OBJ_DIR)/GenerateFrames_test.o \
			   			  $(OBJ_DIR)/TesterPresent_test.o \
			   			  $(OBJ_DIR)/NegativeResponse_test.o \
			   			  $(OBJ_DIR)/DiagnosticSessionControl_test.o \
			   			  $(OBJ_DIR)/AccessTimingParameter_test.o \
			   			  $(OBJ_DIR)/MCUModule_test.o \
			   			  $(OBJ_DIR)/ReceiveFrames_test.o \
			   			  $(OBJ_DIR)/CreateInterface_test.o


OBJS_READDTC_TEST = $(OBJ_DIR)/Logger_test.o \
			   		$(OBJ_DIR)/GenerateFrames_test.o \
			   		$(OBJ_DIR)/ReadDtcInformation_test.o
						
OBJS_CLEARDTC_TEST = $(OBJ_DIR)/Logger_test.o \
			   		 $(OBJ_DIR)/GenerateFrames_test.o \
			   		 $(OBJ_DIR)/ClearDtc_test.o

OBJS_ROUTINECONTROL_TEST = $(OBJ_DIR)/MemoryManager_test.o \
						   $(OBJ_DIR)/Logger_test.o \
			   			   $(OBJ_DIR)/GenerateFrames_test.o \
			   			   $(OBJ_DIR)/RoutineControl_test.o \
			   			   $(OBJ_DIR)/NegativeResponse_test.o \
			   			   $(OBJ_DIR)/SecurityAccess_test.o \
			   			   $(OBJ_DIR)/RoutineControl_test.o \
			   			   $(OBJ_DIR)/NegativeResponse_test.o \
			   			   $(OBJ_DIR)/SecurityAccess_test.o
					
OBJS_ACCESSTIMINGPARAMETER_TEST = $(OBJ_DIR)/Logger_test.o \
                                  $(OBJ_DIR)/GenerateFrames_test.o \
                                  $(OBJ_DIR)/AccessTimingParameter_test.o

OTA_OBJS_TEST = $(OBJ_DIR)/RequestUpdateStatus_test.o \
				$(OBJ_DIR)/RequestTransferExit_test.o \
				$(OBJ_DIR)/RequestDownload_test.o \
				$(OBJ_DIR)/TransferData_test.o
				
OBJS_REQUESTUPDATESTATUS_TEST = $(OBJ_DIR)/Logger_test.o \
			   			  		$(OBJ_DIR)/GenerateFrames_test.o \
			   			  		$(OBJ_DIR)/RequestUpdateStatus_test.o

OBJS_REQUESTTRANSFEREXIT_TEST = $(OBJ_DIR)/Logger_test.o \
                                $(OBJ_DIR)/GenerateFrames_test.o \
                                $(OBJ_DIR)/RequestTransferExit_test.o
						
OBJS_REQUESTDOWNLOAD_TEST = $(OBJ_DIR)/Logger_test.o \
			   			  $(OBJ_DIR)/GenerateFrames_test.o \
			   			  $(OBJ_DIR)/RequestDownload_test.o

OBJS_TRANSFERDATA_TEST = $(OBJ_DIR)/Logger_test.o \
			   			  $(OBJ_DIR)/GenerateFrames_test.o \
			   			  $(OBJ_DIR)/TransferData_test.o

OBJS_NEGATIVERESPONSE_TEST = $(OBJ_DIR)/Logger_test.o \
                             $(OBJ_DIR)/GenerateFrames_test.o \
                             $(OBJ_DIR)/NegativeResponse_test.o \

OBJS_TEST = $(MCU_OBJS_TEST) \
            $(ECU_OBJS_TEST) \
            $(UTILS_OBJS_TEST) \
            $(UDS_OBJS_TEST) \
            $(OTA_OBJS_TEST)

    
$(OBJ_DIR)/MCUModule_test.o: $(SRC_DIR)/MCUModule.cpp
	$(CXX) $(CFLAGS) $(CFLAGSTST) -c $(SRC_DIR)/MCUModule.cpp -o $(OBJ_DIR)/MCUModule_test.o $(CFLAGSTST2) $(LDFLAGS)

$(OBJ_DIR)/BatteryModule_test.o: $(BATTERY_DIR)/BatteryModule.cpp
	$(CXX) $(CFLAGS) $(CFLAGSTST) -c $(BATTERY_DIR)/BatteryModule.cpp -o $(OBJ_DIR)/BatteryModule_test.o $(CFLAGSTST2) $(LDFLAGS)

$(OBJ_DIR)/EngineModule_test.o: $(ENGINE_DIR)/EngineModule.cpp
	$(CXX) $(CFLAGS) $(CFLAGSTST) -c $(ENGINE_DIR)/EngineModule.cpp -o $(OBJ_DIR)/EngineModule_test.o $(CFLAGSTST2) $(LDFLAGS)

$(OBJ_DIR)/DoorsModule_test.o: $(DOORS_DIR)/DoorsModule.cpp
	$(CXX) $(CFLAGS) $(CFLAGSTST) -c $(DOORS_DIR)/DoorsModule.cpp -o $(OBJ_DIR)/DoorsModule_test.o $(CFLAGSTST2) $(LDFLAGS)

$(OBJ_DIR)/HVACModule_test.o: $(HVAC_DIR)/HVACModule.cpp
	$(CXX) $(CFLAGS) $(CFLAGSTST) -c $(HVAC_DIR)/HVACModule.cpp -o $(OBJ_DIR)/HVACModule_test.o $(CFLAGSTST2) $(LDFLAGS)

$(OBJ_DIR)/ECU_test.o: $(UTILS_DIR)/ECU.cpp
	$(CXX) $(CFLAGS) $(CFLAGSTST) -c $(UTILS_DIR)/ECU.cpp -o $(OBJ_DIR)/ECU_test.o $(CFLAGSTST2) $(LDFLAGS)

$(OBJ_DIR)/GenerateFrames_test.o: $(UTILS_DIR)/GenerateFrames.cpp
	$(CXX) $(CFLAGS) $(CFLAGSTST) -c $(UTILS_DIR)/GenerateFrames.cpp -o $(OBJ_DIR)/GenerateFrames_test.o $(CFLAGSTST2) $(LDFLAGS)

$(OBJ_DIR)/HandleFrames_test.o: $(UTILS_DIR)/HandleFrames.cpp
	$(CXX) $(CFLAGS) $(CFLAGSTST) -c $(UTILS_DIR)/HandleFrames.cpp -o $(OBJ_DIR)/HandleFrames_test.o $(CFLAGSTST2) $(LDFLAGS)

$(OBJ_DIR)/CreateInterface_test.o: $(UTILS_DIR)/CreateInterface.cpp
	$(CXX) $(CFLAGS) $(CFLAGSTST) -c $(UTILS_DIR)/CreateInterface.cpp -o $(OBJ_DIR)/CreateInterface_test.o $(CFLAGSTST2) $(LDFLAGS)

$(OBJ_DIR)/DiagnosticSessionControl_test.o: $(UDS_DIR)/diagnostic_session_control/src/DiagnosticSessionControl.cpp
	$(CXX) $(CFLAGS) $(CFLAGSTST) -c $(UDS_DIR)/diagnostic_session_control/src/DiagnosticSessionControl.cpp -o $(OBJ_DIR)/DiagnosticSessionControl_test.o $(CFLAGSTST2) $(LDFLAGS)

$(OBJ_DIR)/ReceiveFrames_test.o: $(SRC_DIR)/ReceiveFrames.cpp
	$(CXX) $(CFLAGS) $(CFLAGSTST) -c $(SRC_DIR)/ReceiveFrames.cpp -o $(OBJ_DIR)/ReceiveFrames_test.o $(CFLAGSTST2) $(LDFLAGS)

$(OBJ_DIR)/ReadDataByIdentifier_test.o: $(UDS_DIR)/read_data_by_identifier/src/ReadDataByIdentifier.cpp
	$(CXX) $(CFLAGS) $(CFLAGSTST) -c $(UDS_DIR)/read_data_by_identifier/src/ReadDataByIdentifier.cpp -o $(OBJ_DIR)/ReadDataByIdentifier_test.o $(CFLAGSTST2) $(LDFLAGS)

$(OBJ_DIR)/WriteDataByIdentifier_test.o: $(UDS_DIR)/write_data_by_identifier/src/WriteDataByIdentifier.cpp
	$(CXX) $(CFLAGS) $(CFLAGSTST) -c $(UDS_DIR)/write_data_by_identifier/src/WriteDataByIdentifier.cpp -o $(OBJ_DIR)/WriteDataByIdentifier_test.o $(CFLAGSTST2) $(LDFLAGS)

$(OBJ_DIR)/EcuReset_test.o: $(UDS_DIR)/ecu_reset/src/EcuReset.cpp
	$(CXX) $(CFLAGS) $(CFLAGSTST) -c $(UDS_DIR)/ecu_reset/src/EcuReset.cpp -o $(OBJ_DIR)/EcuReset_test.o $(CFLAGSTST2) $(LDFLAGS)

$(OBJ_DIR)/SecurityAccess_test.o: $(UDS_DIR)/authentication/src/SecurityAccess.cpp
	$(CXX) $(CFLAGS) $(CFLAGSTST) -c $(UDS_DIR)/authentication/src/SecurityAccess.cpp -o $(OBJ_DIR)/SecurityAccess_test.o $(CFLAGSTST2) $(LDFLAGS)

$(OBJ_DIR)/TesterPresent_test.o: $(UDS_DIR)/tester_present/src/TesterPresent.cpp
	$(CXX) $(CFLAGS) $(CFLAGSTST) -c $(UDS_DIR)/tester_present/src/TesterPresent.cpp -o $(OBJ_DIR)/TesterPresent_test.o $(CFLAGSTST2) $(LDFLAGS)

$(OBJ_DIR)/ECU_ReceiveFrames_test.o: $(UTILS_DIR)/ReceiveFrames.cpp
	$(CXX) $(CFLAGS) $(CFLAGSTST) -c $(UTILS_DIR)/ReceiveFrames.cpp -o $(OBJ_DIR)/ECU_ReceiveFrames_test.o $(CFLAGSTST2) $(LDFLAGS)

$(OBJ_DIR)/FileManager_test.o: $(UTILS_DIR)/FileManager.cpp
	$(CXX) $(CFLAGS) $(CFLAGSTST) -c $(UTILS_DIR)/FileManager.cpp -o $(OBJ_DIR)/FileManager_test.o $(CFLAGSTST2) $(LDFLAGS)

$(OBJ_DIR)/ReadDtcInformation_test.o: $(UDS_DIR)/read_dtc_information/src/ReadDtcInformation.cpp
	$(CXX) $(CFLAGS) $(CFLAGSTST) -c $(UDS_DIR)/read_dtc_information/src/ReadDtcInformation.cpp -o $(OBJ_DIR)/ReadDtcInformation_test.o $(CFLAGSTST2) $(LDFLAGS)

$(OBJ_DIR)/RequestUpdateStatus_test.o: $(OTA_DIR)/request_update_status/src/RequestUpdateStatus.cpp
	$(CXX) $(CFLAGS) $(CFLAGSTST) -c $(OTA_DIR)/request_update_status/src/RequestUpdateStatus.cpp -o $(OBJ_DIR)/RequestUpdateStatus_test.o $(CFLAGSTST2) $(LDFLAGS)

$(OBJ_DIR)/RequestTransferExit_test.o: $(OTA_DIR)/request_transfer_exit/src/RequestTransferExit.cpp
	$(CXX) $(CFLAGS) $(CFLAGSTST) -c $(OTA_DIR)/request_transfer_exit/src/RequestTransferExit.cpp -o $(OBJ_DIR)/RequestTransferExit_test.o $(CFLAGSTST2) $(LDFLAGS)

$(OBJ_DIR)/RequestDownload_test.o: $(OTA_DIR)/request_download/src/RequestDownload.cpp
	$(CXX) $(CFLAGS) $(CFLAGSTST) -c $(OTA_DIR)/request_download/src/RequestDownload.cpp -o $(OBJ_DIR)/RequestDownload_test.o $(CFLAGSTST2) $(LDFLAGS)

$(OBJ_DIR)/MemoryManager_test.o: $(UTILS_DIR)/MemoryManager.cpp
	$(CXX) $(CFLAGS) $(CFLAGSTST) -c $(UTILS_DIR)/MemoryManager.cpp -o $(OBJ_DIR)/MemoryManager_test.o $(CFLAGSTST2) $(LDFLAGS)

$(OBJ_DIR)/TransferData_test.o: $(OTA_DIR)/transfer_data/src/TransferData.cpp
	$(CXX) $(CFLAGS) $(CFLAGSTST) -c $(OTA_DIR)/transfer_data/src/TransferData.cpp -o $(OBJ_DIR)/TransferData_test.o $(CFLAGSTST2) $(LDFLAGS)

$(OBJ_DIR)/Logger_test.o: $(UTILS_DIR)/Logger.cpp
	$(CXX) $(CFLAGS) $(CFLAGSTST) -c $(UTILS_DIR)/Logger.cpp -o $(OBJ_DIR)/Logger_test.o $(CFLAGSTST2) $(LDFLAGS)

$(OBJ_DIR)/ClearDtc_test.o: $(UDS_DIR)/clear_dtc/src/ClearDtc.cpp
	$(CXX) $(CFLAGS) $(CFLAGSTST) -c $(UDS_DIR)/clear_dtc/src/ClearDtc.cpp -o $(OBJ_DIR)/ClearDtc_test.o $(CFLAGSTST2) $(LDFLAGS)
	
$(OBJ_DIR)/RoutineControl_test.o: $(UDS_DIR)/routine_control/src/RoutineControl.cpp
	$(CXX) $(CFLAGS) $(CFLAGSTST) -c $(UDS_DIR)/routine_control/src/RoutineControl.cpp -o $(OBJ_DIR)/RoutineControl_test.o $(CFLAGSTST2) $(LDFLAGS)

$(OBJ_DIR)/ReadMemoryByAddress_test.o: $(UDS_DIR)/read_memory_by_address/src/ReadMemoryByAddress.cpp
	$(CXX) $(CFLAGS) $(CFLAGSTST) -c $(UDS_DIR)/read_memory_by_address/src/ReadMemoryByAddress.cpp -o $(OBJ_DIR)/ReadMemoryByAddress_test.o $(CFLAGSTST2) $(LDFLAGS)

$(OBJ_DIR)/AccessTimingParameter_test.o: $(UDS_DIR)/access_timing_parameters/src/AccessTimingParameter.cpp
	$(CXX) $(CFLAGS) $(CFLAGSTST) -c $(UDS_DIR)/access_timing_parameters/src/AccessTimingParameter.cpp -o $(OBJ_DIR)/AccessTimingParameter_test.o $(CFLAGSTST2) $(LDFLAGS)

$(OBJ_DIR)/NegativeResponse_test.o: $(UTILS_DIR)/NegativeResponse.cpp
	$(CXX) $(CFLAGS) $(CFLAGSTST) -c $(UTILS_DIR)/NegativeResponse.cpp -o $(OBJ_DIR)/NegativeResponse_test.o $(CFLAGSTST2) $(LDFLAGS)

	
# Compile all unit tests

allTests: mcuModuleTest handleFramesTest receiveFramesTest generateFramesTest loggerTest memoryManagerTest createInterfaceTest diagnosticSessionControlTest writeDataByIdentifierTest readDataByIdentifierTest requestTransferExitTest readDtcTest clearDtcTest requestUpdateStatusTest securityAccessTest testerPresentTest routineControlTest transferDataTest requestDownloadTest ecuResetTest negativeResponseTest accessTimingParameterTest


# HandleFrames Unit tests
handleFramesTest: $(OBJ_DIR) $(UTILS_TEST)/handleFramesTest.out

$(UTILS_TEST)/handleFramesTest.out: $(OBJ_DIR) $(OBJS_TEST) $(UTILS_TEST)/HandleFrames_test.o
	$(CXX) $(CFLAGSTST) -o $(UTILS_TEST)/handleFramesTest.out $(UTILS_TEST)/HandleFrames_test.o $(OBJS_TEST) $(CFLAGSTST2) $(LDFLAGS)

$(UTILS_TEST)/HandleFrames_test.o: $(UTILS_TEST)/HandleFramesTest.cpp
	$(CXX) $(CFLAGS) $(CFLAGSTST) -c $(UTILS_TEST)/HandleFramesTest.cpp -o $(UTILS_TEST)/HandleFrames_test.o $(CFLAGSTST2) $(LDFLAGS)


# ReceiveFrames Unit tests
receiveFramesTest: $(OBJ_DIR) $(SRC_TEST)/receiveFramesTest.out

$(SRC_TEST)/receiveFramesTest.out: $(OBJ_DIR) $(OBJS_TEST) $(SRC_TEST)/ReceiveFrames_test.o
	$(CXX) $(CFLAGSTST) -o $(SRC_TEST)/receiveFramesTest.out $(SRC_TEST)/ReceiveFrames_test.o $(OBJS_TEST) $(CFLAGSTST2) $(LDFLAGS)

$(SRC_TEST)/ReceiveFrames_test.o: $(SRC_TEST)/ReceiveFramesTest.cpp
	$(CXX) $(CFLAGS) $(CFLAGSTST) -c $(SRC_TEST)/ReceiveFramesTest.cpp -o $(SRC_TEST)/ReceiveFrames_test.o $(CFLAGSTST2) $(LDFLAGS)



# MCUModule Unit tests
mcuModuleTest: $(OBJ_DIR) $(SRC_TEST)/mcuModuleTest.out

$(SRC_TEST)/mcuModuleTest.out: $(OBJ_DIR) $(OBJS_TEST) $(SRC_TEST)/MCUModule_test.o
	$(CXX) $(CFLAGSTST) -o $(SRC_TEST)/mcuModuleTest.out $(SRC_TEST)/MCUModule_test.o $(OBJS_TEST) $(CFLAGSTST2) $(LDFLAGS)

$(SRC_TEST)/MCUModule_test.o: $(SRC_TEST)/MCUModuleTest.cpp
	$(CXX) $(CFLAGS) $(CFLAGSTST) -c $(SRC_TEST)/MCUModuleTest.cpp -o $(SRC_TEST)/MCUModule_test.o $(CFLAGSTST2) $(LDFLAGS)



# GenerateFrames Unit tests
generateFramesTest: $(OBJ_DIR) $(UTILS_TEST)/generateFramesTest.out

$(UTILS_TEST)/generateFramesTest.out: $(OBJ_DIR) $(OBJS_GENERATE_TEST) $(UTILS_TEST)/GenerateFrames_test.o
	$(CXX) $(CFLAGSTST) -o $(UTILS_TEST)/generateFramesTest.out $(UTILS_TEST)/GenerateFrames_test.o $(OBJS_GENERATE_TEST) $(CFLAGSTST2) $(LDFLAGS)

$(UTILS_TEST)/GenerateFrames_test.o: $(UTILS_TEST)/GenerateFramesTest.cpp
	$(CXX) $(CFLAGS) $(CFLAGSTST) -c $(UTILS_TEST)/GenerateFramesTest.cpp -o $(UTILS_TEST)/GenerateFrames_test.o $(CFLAGSTST2) $(LDFLAGS)


	
# Logger Unit tests
loggerTest: $(OBJ_DIR) $(UTILS_TEST)/loggerTest.out

$(UTILS_TEST)/loggerTest.out: $(OBJ_DIR) $(OBJS_LOGGER_TEST) $(UTILS_TEST)/Logger_test.o
	$(CXX) $(CFLAGSTST) -o $(UTILS_TEST)/loggerTest.out $(UTILS_TEST)/Logger_test.o $(OBJS_LOGGER_TEST) $(CFLAGSTST2) $(LDFLAGS)

$(UTILS_TEST)/Logger_test.o: $(UTILS_TEST)/Logger_test.cpp
	$(CXX) $(CFLAGS) $(CFLAGSTST) -c $(UTILS_TEST)/Logger_test.cpp -o $(UTILS_TEST)/Logger_test.o $(CFLAGSTST2) $(LDFLAGS)



# MemoryManager Unit tests
memoryManagerTest: $(OBJ_DIR) $(UTILS_TEST)/memoryManagerTest.out

$(UTILS_TEST)/memoryManagerTest.out: $(OBJ_DIR) $(OBJS_MEMORY_TEST) $(UTILS_TEST)/MemoryManager_test.o
	$(CXX) $(CFLAGSTST) -o $(UTILS_TEST)/memoryManagerTest.out $(UTILS_TEST)/MemoryManager_test.o $(OBJS_MEMORY_TEST) $(CFLAGSTST2) $(LDFLAGS)

$(UTILS_TEST)/MemoryManager_test.o: $(UTILS_TEST)/MemoryManagerTest.cpp
	$(CXX) $(CFLAGS) $(CFLAGSTST) -c $(UTILS_TEST)/MemoryManagerTest.cpp -o $(UTILS_TEST)/MemoryManager_test.o $(CFLAGSTST2) $(LDFLAGS)



# CreateInterface Unit tests
createInterfaceTest: $(OBJ_DIR) $(UTILS_TEST)/createInterfaceTest.out

$(UTILS_TEST)/createInterfaceTest.out: $(OBJ_DIR) $(OBJS_CREATEINTERFACE_TEST) $(UTILS_TEST)/CreateInterface_test.o
	$(CXX) $(CFLAGSTST) -o $(UTILS_TEST)/createInterfaceTest.out $(UTILS_TEST)/CreateInterface_test.o $(OBJS_CREATEINTERFACE_TEST) $(CFLAGSTST2) $(LDFLAGS)

$(UTILS_TEST)/CreateInterface_test.o: $(UTILS_TEST)/CreateInterface_test.cpp
	$(CXX) $(CFLAGS) $(CFLAGSTST) -c $(UTILS_TEST)/CreateInterface_test.cpp -o $(UTILS_TEST)/CreateInterface_test.o $(CFLAGSTST2) $(LDFLAGS)



# DiagnosticSessionControl Unit tests
diagnosticSessionControlTest: $(OBJ_DIR) $(UDS_DIR)/diagnostic_session_control/utest/diagnosticSessionControlTest.out

$(UDS_DIR)/diagnostic_session_control/utest/diagnosticSessionControlTest.out: $(OBJ_DIR) $(OBJS_TEST) $(UDS_DIR)/diagnostic_session_control/utest/DiagnosticSessionControl_test.o
	$(CXX) $(CFLAGSTST) -o $(UDS_DIR)/diagnostic_session_control/utest/diagnosticSessionControlTest.out $(UDS_DIR)/diagnostic_session_control/utest/DiagnosticSessionControl_test.o $(OBJS_TEST) $(CFLAGSTST2) $(LDFLAGS)

$(UDS_DIR)/diagnostic_session_control/utest/DiagnosticSessionControl_test.o: $(UDS_DIR)/diagnostic_session_control/utest/DiagnosticSessionControlTest.cpp
	$(CXX) $(CFLAGS) $(CFLAGSTST) -c $(UDS_DIR)/diagnostic_session_control/utest/DiagnosticSessionControlTest.cpp -o $(UDS_DIR)/diagnostic_session_control/utest/DiagnosticSessionControl_test.o $(CFLAGSTST2) $(LDFLAGS)



# WriteDataByIdentifier Unit tests
writeDataByIdentifierTest: $(OBJ_DIR) $(UDS_DIR)/write_data_by_identifier/utest/writeDataByIdentifierTest.out

$(UDS_DIR)/write_data_by_identifier/utest/writeDataByIdentifierTest.out: $(OBJ_DIR) $(OBJS_TEST) $(UDS_DIR)/write_data_by_identifier/utest/WriteDataByIdentifier_test.o
	$(CXX) $(CFLAGSTST) -o $(UDS_DIR)/write_data_by_identifier/utest/writeDataByIdentifierTest.out $(UDS_DIR)/write_data_by_identifier/utest/WriteDataByIdentifier_test.o $(OBJS_TEST) $(CFLAGSTST2) $(LDFLAGS)

$(UDS_DIR)/write_data_by_identifier/utest/WriteDataByIdentifier_test.o: $(UDS_DIR)/write_data_by_identifier/utest/WriteDataByIdentifierTest.cpp
	$(CXX) $(CFLAGS) $(CFLAGSTST) -c $(UDS_DIR)/write_data_by_identifier/utest/WriteDataByIdentifierTest.cpp -o $(UDS_DIR)/write_data_by_identifier/utest/WriteDataByIdentifier_test.o $(CFLAGSTST2) $(LDFLAGS)



# ReadDataByIdentifier Unit tests
readDataByIdentifierTest: $(OBJ_DIR) $(UDS_DIR)/read_data_by_identifier/utest/readDataByIdentifierTest.out

$(UDS_DIR)/read_data_by_identifier/utest/readDataByIdentifierTest.out: $(OBJ_DIR) $(OBJS_TEST) $(UDS_DIR)/read_data_by_identifier/utest/ReadDataByIdentifier_test.o
	$(CXX) $(CFLAGSTST) -o $(UDS_DIR)/read_data_by_identifier/utest/readDataByIdentifierTest.out $(UDS_DIR)/read_data_by_identifier/utest/ReadDataByIdentifier_test.o $(OBJS_TEST) $(CFLAGSTST2) $(LDFLAGS)

$(UDS_DIR)/read_data_by_identifier/utest/ReadDataByIdentifier_test.o: $(UDS_DIR)/read_data_by_identifier/utest/ReadDataByIdentifierTest.cpp
	$(CXX) $(CFLAGS) $(CFLAGSTST) -c $(UDS_DIR)/read_data_by_identifier/utest/ReadDataByIdentifierTest.cpp -o $(UDS_DIR)/read_data_by_identifier/utest/ReadDataByIdentifier_test.o $(CFLAGSTST2) $(LDFLAGS)



# RequestTransferExit Unit tests
requestTransferExitTest: $(OBJ_DIR) $(OTA_DIR)/request_transfer_exit/utest/requestTransferExitTest.out

$(OTA_DIR)/request_transfer_exit/utest/requestTransferExitTest.out: $(OBJ_DIR) $(OBJS_REQUESTTRANSFEREXIT_TEST) $(OTA_DIR)/request_transfer_exit/utest/RequestTransferExit_test.o
	$(CXX) $(CFLAGSTST) -o $(OTA_DIR)/request_transfer_exit/utest/requestTransferExitTest.out $(OTA_DIR)/request_transfer_exit/utest/RequestTransferExit_test.o $(OBJS_REQUESTTRANSFEREXIT_TEST) $(CFLAGSTST2) $(LDFLAGS)

$(OTA_DIR)/request_transfer_exit/utest/RequestTransferExit_test.o: $(OTA_DIR)/request_transfer_exit/utest/RequestTransferExitTest.cpp
	$(CXX) $(CFLAGS) $(CFLAGSTST) -c $(OTA_DIR)/request_transfer_exit/utest/RequestTransferExitTest.cpp -o $(OTA_DIR)/request_transfer_exit/utest/RequestTransferExit_test.o $(CFLAGSTST2) $(LDFLAGS)



# ReadDTC Unit tests
readDtcTest: $(OBJ_DIR) $(UDS_DIR)/read_dtc_information/utest/readDtcTest.out

$(UDS_DIR)/read_dtc_information/utest/readDtcTest.out: $(OBJ_DIR) $(OBJS_TEST) $(UDS_DIR)/read_dtc_information/utest/ReadDtcInformation_test.o
	$(CXX) $(CFLAGSTST) -o $(UDS_DIR)/read_dtc_information/utest/readDtcTest.out $(UDS_DIR)/read_dtc_information/utest/ReadDtcInformation_test.o $(OBJS_TEST) $(CFLAGSTST2) $(LDFLAGS)

$(UDS_DIR)/read_dtc_information/utest/ReadDtcInformation_test.o: $(UDS_DIR)/read_dtc_information/utest/ReadDtcInformationTest.cpp
	$(CXX) $(CFLAGS) $(CFLAGSTST) -c $(UDS_DIR)/read_dtc_information/utest/ReadDtcInformationTest.cpp -o $(UDS_DIR)/read_dtc_information/utest/ReadDtcInformation_test.o $(CFLAGSTST2) $(LDFLAGS)



# ClearDTC Unit tests
clearDtcTest: $(OBJ_DIR) $(UDS_DIR)/clear_dtc/utest/clearDtcTest.out

$(UDS_DIR)/clear_dtc/utest/clearDtcTest.out: $(OBJ_DIR) $(OBJS_TEST) $(UDS_DIR)/clear_dtc/utest/ClearDtc_test.o
	$(CXX) $(CFLAGSTST) -o $(UDS_DIR)/clear_dtc/utest/clearDtcTest.out $(UDS_DIR)/clear_dtc/utest/ClearDtc_test.o $(OBJS_TEST) $(CFLAGSTST2) $(LDFLAGS)

$(UDS_DIR)/clear_dtc/utest/ClearDtc_test.o: $(UDS_DIR)/clear_dtc/utest/ClearDtcTest.cpp
	$(CXX) $(CFLAGS) $(CFLAGSTST) -c $(UDS_DIR)/clear_dtc/utest/ClearDtcTest.cpp -o $(UDS_DIR)/clear_dtc/utest/ClearDtc_test.o $(CFLAGSTST2) $(LDFLAGS)



# RequestUpdateStatus Unit tests
requestUpdateStatusTest: $(OBJ_DIR) $(OTA_DIR)/request_update_status/utest/requestUpdateStatusTest.out

$(OTA_DIR)/request_update_status/utest/requestUpdateStatusTest.out: $(OBJ_DIR) $(OBJS_REQUESTUPDATESTATUS_TEST) $(OTA_DIR)/request_update_status/utest/RequestUpdateStatus_test.o
	$(CXX) $(CFLAGSTST) -o $(OTA_DIR)/request_update_status/utest/requestUpdateStatusTest.out $(OTA_DIR)/request_update_status/utest/RequestUpdateStatus_test.o $(OBJS_REQUESTUPDATESTATUS_TEST) $(CFLAGSTST2) $(LDFLAGS)

$(OTA_DIR)/request_update_status/utest/RequestUpdateStatus_test.o: $(OTA_DIR)/request_update_status/utest/RequestUpdateStatusTest.cpp
	$(CXX) $(CFLAGS) $(CFLAGSTST) -c $(OTA_DIR)/request_update_status/utest/RequestUpdateStatusTest.cpp -o $(OTA_DIR)/request_update_status/utest/RequestUpdateStatus_test.o $(CFLAGSTST2) $(LDFLAGS)



# SecurityAccess Unit tests
securityAccessTest: $(OBJ_DIR) $(UDS_DIR)/authentication/utest/securityAccessTest.out

$(UDS_DIR)/authentication/utest/securityAccessTest.out: $(OBJ_DIR) $(OBJS_SECURITYACCESS_TEST) $(UDS_DIR)/authentication/utest/SecurityAccess_test.o
	$(CXX) $(CFLAGSTST) -o $(UDS_DIR)/authentication/utest/securityAccessTest.out $(UDS_DIR)/authentication/utest/SecurityAccess_test.o $(OBJS_SECURITYACCESS_TEST) $(CFLAGSTST2) $(LDFLAGS)

$(UDS_DIR)/authentication/utest/SecurityAccess_test.o: $(UDS_DIR)/authentication/utest/SecurityAccessTest.cpp
	$(CXX) $(CFLAGS) $(CFLAGSTST) -c $(UDS_DIR)/authentication/utest/SecurityAccessTest.cpp -o $(UDS_DIR)/authentication/utest/SecurityAccess_test.o $(CFLAGSTST2) $(LDFLAGS)



# TesterPresent Unit tests
testerPresentTest: $(OBJ_DIR) $(UDS_DIR)/tester_present/utest/testerPresentTest.out

$(UDS_DIR)/tester_present/utest/testerPresentTest.out: $(OBJ_DIR) $(OBJS_TEST) $(UDS_DIR)/tester_present/utest/TesterPresent_test.o
	$(CXX) $(CFLAGSTST) -o $(UDS_DIR)/tester_present/utest/testerPresentTest.out $(UDS_DIR)/tester_present/utest/TesterPresent_test.o $(OBJS_TEST) $(CFLAGSTST2) $(LDFLAGS)

$(UDS_DIR)/tester_present/utest/TesterPresent_test.o: $(UDS_DIR)/tester_present/utest/TesterPresentTest.cpp
	$(CXX) $(CFLAGS) $(CFLAGSTST) -c $(UDS_DIR)/tester_present/utest/TesterPresentTest.cpp -o $(UDS_DIR)/tester_present/utest/TesterPresent_test.o $(CFLAGSTST2) $(LDFLAGS)



# RoutineControl Unit tests
routineControlTest: $(OBJ_DIR) $(UDS_DIR)/routine_control/utest/routineControlTest.out

$(UDS_DIR)/routine_control/utest/routineControlTest.out: $(OBJ_DIR) $(OBJS_TEST) $(UDS_DIR)/routine_control/utest/RoutineControl_test.o
	$(CXX) $(CFLAGSTST) -o $(UDS_DIR)/routine_control/utest/routineControlTest.out $(UDS_DIR)/routine_control/utest/RoutineControl_test.o $(OBJS_TEST) $(CFLAGSTST2) $(LDFLAGS)

$(UDS_DIR)/routine_control/utest/RoutineControl_test.o: $(UDS_DIR)/routine_control/utest/RoutineControlTest.cpp
	$(CXX) $(CFLAGS) $(CFLAGSTST) -c $(UDS_DIR)/routine_control/utest/RoutineControlTest.cpp -o $(UDS_DIR)/routine_control/utest/RoutineControl_test.o $(CFLAGSTST2) $(LDFLAGS)



# EcuReset Unit tests
ecuResetTest: $(OBJ_DIR) $(UDS_DIR)/ecu_reset/utest/ecuResetTest.out

$(UDS_DIR)/ecu_reset/utest/ecuResetTest.out: $(OBJ_DIR) $(OBJS_TEST) $(UDS_DIR)/ecu_reset/utest/EcuReset_test.o
	$(CXX) $(CFLAGSTST) -o $(UDS_DIR)/ecu_reset/utest/ecuResetTest.out $(UDS_DIR)/ecu_reset/utest/EcuReset_test.o $(OBJS_TEST) $(CFLAGSTST2) $(LDFLAGS)

$(UDS_DIR)/ecu_reset/utest/EcuReset_test.o: $(UDS_DIR)/ecu_reset/utest/EcuResetTest.cpp
	$(CXX) $(CFLAGS) $(CFLAGSTST) -c $(UDS_DIR)/ecu_reset/utest/EcuResetTest.cpp -o $(UDS_DIR)/ecu_reset/utest/EcuReset_test.o $(CFLAGSTST2) $(LDFLAGS)



# RequestDownload Unit tests
requestDownloadTest: $(OBJ_DIR) $(OTA_DIR)/request_download/utest/requestDownloadTest.out

$(OTA_DIR)/request_download/utest/requestDownloadTest.out: $(OBJ_DIR) $(OBJS_REQUESTDOWNLOAD_TEST) $(OTA_DIR)/request_download/utest/RequestDownload_test.o
	$(CXX) $(CFLAGSTST) -o $(OTA_DIR)/request_download/utest/requestDownloadTest.out $(OTA_DIR)/request_download/utest/RequestDownload_test.o $(OBJS_REQUESTDOWNLOAD_TEST) $(CFLAGSTST2) $(LDFLAGS)

$(OTA_DIR)/request_download/utest/RequestDownload_test.o: $(OTA_DIR)/request_download/utest/RequestDownloadTest.cpp
	$(CXX) $(CFLAGS) $(CFLAGSTST) -c $(OTA_DIR)/request_download/utest/RequestDownloadTest.cpp -o $(OTA_DIR)/request_download/utest/RequestDownload_test.o $(CFLAGSTST2) $(LDFLAGS)



# TransferData Unit tests
transferDataTest: $(OBJ_DIR) $(OTA_DIR)/transfer_data/utest/transferDataTest.out

$(OTA_DIR)/transfer_data/utest/transferDataTest.out: $(OBJ_DIR) $(OBJS_TRANSFERDATA_TEST) $(OTA_DIR)/transfer_data/utest/TransferData_test.o
	$(CXX) $(CFLAGSTST) -o $(OTA_DIR)/transfer_data/utest/transferDataTest.out $(OTA_DIR)/transfer_data/utest/TransferData_test.o $(OBJS_TRANSFERDATA_TEST) $(CFLAGSTST2) $(LDFLAGS)

$(OTA_DIR)/transfer_data/utest/TransferData_test.o: $(OTA_DIR)/transfer_data/utest/TransferDataTest.cpp
	$(CXX) $(CFLAGS) $(CFLAGSTST) -c $(OTA_DIR)/transfer_data/utest/TransferDataTest.cpp -o $(OTA_DIR)/transfer_data/utest/TransferData_test.o $(CFLAGSTST2) $(LDFLAGS)



# AccessTimingParameter Unit tests
accessTimingParameterTest: $(OBJ_DIR) $(UDS_DIR)/access_timing_parameters/utest/accessTimingParameterTest.out

$(UDS_DIR)/access_timing_parameters/utest/accessTimingParameterTest.out: $(UDS_DIR) $(OBJS_TEST) $(UDS_DIR)/access_timing_parameters/utest/AccessTimingParameter_test.o
	$(CXX) $(CFLAGSTST) -o $(UDS_DIR)/access_timing_parameters/utest/accessTimingParameterTest.out $(UDS_DIR)/access_timing_parameters/utest/AccessTimingParameter_test.o $(OBJS_TEST) $(CFLAGSTST2) $(LDFLAGS)

$(UDS_DIR)/access_timing_parameters/utest/AccessTimingParameter_test.o: $(UDS_DIR)/access_timing_parameters/utest/AccessTimingParameterTest.cpp
	$(CXX) $(CFLAGS) $(CFLAGSTST) -c $(UDS_DIR)/access_timing_parameters/utest/AccessTimingParameterTest.cpp -o $(UDS_DIR)/access_timing_parameters/utest/AccessTimingParameter_test.o $(CFLAGSTST2) $(LDFLAGS)



# NegativeResponse Unit tests
negativeResponseTest: $(OBJ_DIR) $(UTILS_TEST)/negativeResponseTest.out

$(UTILS_TEST)/negativeResponseTest.out: $(OBJ_DIR) $(OBJS_NEGATIVERESPONSE_TEST) $(UTILS_TEST)/NegativeResponse_test.o
	$(CXX) $(CFLAGSTST) -o $(UTILS_TEST)/negativeResponseTest.out $(UTILS_TEST)/NegativeResponse_test.o $(OBJS_NEGATIVERESPONSE_TEST) $(CFLAGSTST2) $(LDFLAGS)

$(UTILS_TEST)/NegativeResponse_test.o: $(UTILS_TEST)/NegativeResponseTest.cpp
	$(CXX) $(CFLAGS) $(CFLAGSTST) -c $(UTILS_TEST)/NegativeResponseTest.cpp -o $(UTILS_TEST)/NegativeResponse_test.o $(CFLAGSTST2) $(LDFLAGS)



#----------------------------------------------------Coverage--------------------------------------------------------

# routineControl Coverage - solo job
routineControlCoverage: routineControl_coverage

	lcov --capture --directory . --output-file coverage.info
	lcov --remove coverage.info '/usr/' --output-file coverage_filtered.info
	lcov --remove coverage.info '/test/' --output-file coverage.info
	genhtml coverage_filtered.info --output-directory coverage/out_RoutineControlTest
	xdg-open coverage/out_RoutineControlTest/index.html
# This job runs for allCoverage
routineControl_coverage: routineControlTest
	./$(UDS_DIR)/routine_control/utest/routineControlTest.out
	mkdir -p coverage/out_RoutineControl
	gcov -o obj/RoutineControl_test.gcno obj/RoutineControl_test.gcda $(UDS_DIR)/routine_control/src/RoutineControl.cpp
	mv RoutineControl.cpp.gcov coverage/out_RoutineControl



# securityAccess Coverage - solo job
securityAccessCoverage: securityAccess_coverage
	lcov --capture --directory . --output-file coverage.info
	lcov --remove coverage.info '/usr/' --output-file coverage_filtered.info
	lcov --remove coverage.info '/test/' --output-file coverage.info
	genhtml coverage_filtered.info --output-directory coverage/out_SecurityAccessTest
	xdg-open coverage/out_SecurityAccessTest/index.html
# This job runs for allCoverage
securityAccess_coverage: securityAccessTest
	./$(UDS_DIR)/authentication/utest/securityAccessTest.out
	mkdir -p coverage/out_SecurityAccess
	gcov -o obj/SecurityAccess_test.gcno obj/SecurityAccess_test.gcda $(UDS_DIR)/authentication/src/SecurityAccess.cpp
	mv SecurityAccess.cpp.gcov coverage/out_SecurityAccess



# RequestUpdateStatus Coverage - solo job
requestUpdateStatusCoverage: requestUpdateStatus_coverage
	lcov --capture --directory . --output-file coverage.info
	lcov --remove coverage.info '/usr/' --output-file coverage_filtered.info
	lcov --remove coverage.info '/test/' --output-file coverage.info
	genhtml coverage_filtered.info --output-directory coverage/out_RequestUpdateStatusTest
	xdg-open coverage/out_RequestUpdateStatusTest/index.html

# This job runs for allCoverage
requestUpdateStatus_coverage: requestUpdateStatusTest
	./$(OTA_DIR)/request_update_status/utest/requestUpdateStatusTest.out
	mkdir -p coverage/out_RequestUpdateStatus
	gcov -o obj/RequestUpdateStatus_test.gcno obj/RequestUpdateStatus_test.gcda $(OTA_DIR)/request_update_status/src/RequestUpdateStatus.cpp
	mv RequestUpdateStatus.cpp.gcov coverage/out_RequestUpdateStatus



# ReadDTC Coverage - solo job
readDtcCoverage: readDtc_coverage
	lcov --capture --directory . --output-file coverage.info
	lcov --remove coverage.info '/usr/' --output-file coverage_filtered.info
	lcov --remove coverage.info '/test/' --output-file coverage.info
	genhtml coverage_filtered.info --output-directory coverage/out_ReadDtcTest
	xdg-open coverage/out_ReadDtcTest/index.html
# This job runs for allCoverage
readDtc_coverage: readDtcTest
	./$(UDS_DIR)/read_dtc_information/utest/readDtcTest.out
	mkdir -p coverage/out_ReadDtc
	gcov -o obj/ReadDtcInformation_test.gcno obj/ReadDtcInformation_test.gcda $(UDS_DIR)/read_dtc_information/src/ReadDtcInformation.cpp
	mv ReadDtcInformation.cpp.gcov coverage/out_ReadDtc



# ClearDTC Coverage - solo job
clearDtcCoverage: clearDtc_coverage
	lcov --capture --directory . --output-file coverage.info
	lcov --remove coverage.info '/usr/' --output-file coverage_filtered.info
	lcov --remove coverage.info '/test/' --output-file coverage.info
	genhtml coverage_filtered.info --output-directory coverage/out_ClearDtcTest
	xdg-open coverage/out_ClearDtcTest/index.html

# This job runs for allCoverage
clearDtc_coverage: clearDtcTest
	./$(UDS_DIR)/clear_dtc/utest/clearDtcTest.out
	mkdir -p coverage/out_ClearDtc
	gcov -o obj/ClearDtc_test.gcno obj/ClearDtc_test.gcda $(UDS_DIR)/clear_dtc/src/ClearDtc.cpp
	mv ClearDtc.cpp.gcov coverage/out_ClearDtc



# RequestTransferExit Coverage - solo job
requestTransferExitCoverage: requestTransferExit_coverage
	lcov --capture --directory . --output-file coverage.info
	lcov --remove coverage.info '/usr/' --output-file coverage_filtered.info
	lcov --remove coverage.info '/test/' --output-file coverage.info
	genhtml coverage_filtered.info --output-directory coverage/out_RequestTransferExitTest
	xdg-open coverage/out_RequestTransferExitTest/index.html
# This job runs for allCoverage
requestTransferExit_coverage: requestTransferExitTest
	./$(OTA_DIR)/request_transfer_exit/utest/requestTransferExitTest.out
	mkdir -p coverage/out_RequestTransferExit
	gcov -o obj/RequestTransferExit_test.gcno obj/RequestTransferExit_test.gcda $(OTA_DIR)/request_transfer_exit/src/RequestTransferExit.cpp
	mv RequestTransferExit.cpp.gcov coverage/out_RequestTransferExit



# ReadDataByIdentifier Coverage - solo job
readDataByIdentifierCoverage: readDataByIdentifier_coverage
	lcov --capture --directory . --output-file coverage.info
	lcov --remove coverage.info '/usr/' --output-file coverage_filtered.info
	lcov --remove coverage.info '/test/' --output-file coverage.info
	genhtml coverage_filtered.info --output-directory coverage/out_ReadDataByIdentifierTest
	xdg-open coverage/out_ReadDataByIdentifierTest/index.html
# This job runs for allCoverage
readDataByIdentifier_coverage: readDataByIdentifierTest
	./$(UDS_DIR)/read_data_by_identifier/utest/readDataByIdentifierTest.out
	mkdir -p coverage/out_ReadDataByIdentifier
	gcov -o obj/ReadDataByIdentifier_test.gcno obj/ReadDataByIdentifier_test.gcda $(UDS_DIR)/read_data_by_identifier/src/ReadDataByIdentifier.cpp
	mv ReadDataByIdentifier.cpp.gcov coverage/out_ReadDataByIdentifier



# WriteDataByIdentifier Coverage - solo job
writeDataByIdentifierCoverage: writeDataByIdentifier_coverage
	lcov --capture --directory . --output-file coverage.info
	lcov --remove coverage.info '/usr/' --output-file coverage_filtered.info
	lcov --remove coverage.info '/test/' --output-file coverage.info
	genhtml coverage_filtered.info --output-directory coverage/out_WriteDataByIdentifierTest
	xdg-open coverage/out_WriteDataByIdentifierTest/index.html
# This job runs for allCoverage
writeDataByIdentifier_coverage: writeDataByIdentifierTest
	./$(UDS_DIR)/write_data_by_identifier/utest/writeDataByIdentifierTest.out
	mkdir -p coverage/out_WriteDataByIdentifier
	gcov -o obj/WriteDataByIdentifier_test.gcno obj/WriteDataByIdentifier_test.gcda $(UDS_DIR)/write_data_by_identifier/src/WriteDataByIdentifier.cpp
	mv WriteDataByIdentifier.cpp.gcov coverage/out_WriteDataByIdentifier



# DiagnosticSessionControl Coverage - solo job
diagnosticSessionControlCoverage: diagnosticSessionControl_coverage
	lcov --capture --directory . --output-file coverage.info
	lcov --remove coverage.info '/usr/' --output-file coverage_filtered.info
	lcov --remove coverage.info '/test/' --output-file coverage.info
	genhtml coverage_filtered.info --output-directory coverage/out_DiagnosticSessionControlTest
	xdg-open coverage/out_DiagnosticSessionControlTest/index.html
# This job runs for allCoverage
diagnosticSessionControl_coverage: diagnosticSessionControlTest
	./$(UDS_DIR)/diagnostic_session_control/utest/diagnosticSessionControlTest.out
	mkdir -p coverage/out_DiagnosticSessionControl
	gcov -o obj/DiagnosticSessionControl_test.gcno obj/DiagnosticSessionControl_test.gcda $(UDS_DIR)/diagnostic_session_control/src/DiagnosticSessionControl.cpp
	mv DiagnosticSessionControl.cpp.gcov coverage/out_DiagnosticSessionControl



# CreateInterface Coverage - solo job
createInterfaceCoverage: createInterface_coverage
	lcov --capture --directory . --output-file coverage.info
	lcov --remove coverage.info '/usr/' --output-file coverage_filtered.info
	lcov --remove coverage.info '/test/' --output-file coverage.info
	genhtml coverage_filtered.info --output-directory coverage/out_CreateInterfaceTest
	xdg-open coverage/out_CreateInterfaceTest/index.html
# This job runs for allCoverage
createInterface_coverage: createInterfaceTest
	./$(UTILS_TEST)/createInterfaceTest.out
	mkdir -p coverage/out_CreateInterface
	gcov -o obj/CreateInterface_test.gcno obj/CreateInterface_test.gcda $(UTILS_DIR)/CreateInterface.cpp
	mv CreateInterface.cpp.gcov coverage/out_CreateInterface



# HandleFrames Coverage - solo job
handleFramesCoverage: handleFrames_coverage
	lcov --capture --directory . --output-file coverage.info
	lcov --remove coverage.info '/usr/' --output-file coverage_filtered.info
	lcov --remove coverage.info '/test/' --output-file coverage.info
	genhtml coverage_filtered.info --output-directory coverage/out_HandleFramesTest
	xdg-open coverage/out_HandleFramesTest/index.html
# This job runs for allCoverage
handleFrames_coverage: handleFramesTest
	./$(UTILS_TEST)/handleFramesTest.out
	mkdir -p coverage/out_HandleFrames
	gcov -o obj/HandleFrames_test.gcno obj/HandleFrames_test.gcda $(UTILS_TEST)/HandleFrames.cpp
	mv HandleFrames.cpp.gcov coverage/out_HandleFrames



# ReceiveFrames Coverage - solo job
receiveFramesCoverage: receiveFrames_coverage
	lcov --capture --directory . --output-file coverage.info
	lcov --remove coverage.info '/usr/' --output-file coverage_filtered.info
	lcov --remove coverage.info '/test/' --output-file coverage.info
	genhtml coverage_filtered.info --output-directory coverage/out_ReceiveFramesTest
	xdg-open coverage/out_ReceiveFramesTest/index.html
# This job runs for allCoverage
receiveFrames_coverage: receiveFramesTest
	./$(SRC_TEST)/receiveFramesTest.out
	mkdir -p coverage/out_ReceiveFrames
	gcov -o obj/ReceiveFrames_test.gcno obj/ReceiveFrames_test.gcda $(SRC_TEST)/ReceiveFrames.cpp
	mv ReceiveFrames.cpp.gcov coverage/out_ReceiveFrames



# MCUModule Coverage - solo job
mcuModuleCoverage: mcuModule_coverage
	lcov --capture --directory . --output-file coverage.info
	lcov --remove coverage.info '/usr/' --output-file coverage_filtered.info
	lcov --remove coverage.info '/test/' --output-file coverage.info
	genhtml coverage_filtered.info --output-directory coverage/out_MCUModuleTest
	xdg-open coverage/out_MCUModuleTest/index.html
# This job runs for allCoverage
mcuModule_coverage: mcuModuleTest
	./$(SRC_TEST)/mcuModuleTest.out
	mkdir -p coverage/out_MCUModuleTest
	gcov -o obj/MCUModule_test.gcno obj/MCUModule_test.gcda $(SRC_TEST)/MCUModule.cpp
	mv MCUModule.cpp.gcov coverage/out_MCUModule



# GenerateFrames Coverage - solo job
generateFramesCoverage: generateFrames_coverage
	lcov --capture --directory . --output-file coverage.info
	lcov --remove coverage.info '/usr/' --output-file coverage_filtered.info
	lcov --remove coverage.info '/test/' --output-file coverage.info
	genhtml coverage_filtered.info --output-directory coverage/out_GenerateFramesTest
	xdg-open coverage/out_GenerateFramesTest/index.html
# This job runs for allCoverage
generateFrames_coverage: generateFramesTest
	./$(UTILS_TEST)/generateFramesTest.out
	mkdir -p coverage/out_GenerateFrames
	gcov -o obj/GenerateFrames_test.gcno obj/GenerateFrames_test.gcda $(UTILS_DIR)/GenerateFrames.cpp
	mv GenerateFrames.cpp.gcov coverage/out_GenerateFrames



# Logger Coverage - solo job
loggerCoverage: logger_coverage
	lcov --capture --directory . --output-file coverage.info
	lcov --remove coverage.info '/usr/' --output-file coverage_filtered.info
	lcov --remove coverage.info '/test/' --output-file coverage.info
	genhtml coverage_filtered.info --output-directory coverage/out_LoggerTest
	xdg-open coverage/out_LoggerTest/index.html
# This job runs for allCoverage
logger_coverage: loggerTest
	./$(UTILS_TEST)/loggerTest.out
	mkdir -p coverage/out_Logger
	gcov -o obj/Logger_test.gcno obj/Logger_test.gcda $(UTILS_DIR)/Logger.cpp
	mv Logger.cpp.gcov coverage/out_Logger



# MemoryManager Coverage - solo job
memoryManagerCoverage: memoryManager_coverage
	lcov --capture --directory . --output-file coverage.info
	lcov --remove coverage.info '/usr/' --output-file coverage_filtered.info
	lcov --remove coverage.info '/test/' --output-file coverage.info
	genhtml coverage_filtered.info --output-directory coverage/out_MemoryManagerTest
	xdg-open coverage/out_MemoryManagerTest/index.html
# This job runs for allCoverage
memoryManager_coverage: memoryManagerTest
	./$(UTILS_TEST)/memoryManagerTest.out
	mkdir -p coverage/out_MemoryManager
	gcov -o obj/MemoryManager_test.gcno obj/MemoryManager_test.gcda $(UTILS_DIR)/MemoryManager.cpp
	mv MemoryManager.cpp.gcov coverage/out_MemoryManager



# EcuReset Coverage - solo job
ecuResetCoverage: ecuReset_coverage
	lcov --capture --directory . --output-file coverage.info
	lcov --remove coverage.info '/usr/' --output-file coverage_filtered.info
	lcov --remove coverage.info '/test/' --output-file coverage.info
	genhtml coverage_filtered.info --output-directory coverage/out_EcuResetTest
	xdg-open coverage/out_EcuResetTest/index.html
# This job runs for allCoverage
ecuReset_coverage: ecuResetTest
	./$(UDS_DIR)/ecu_reset/utest/ecuResetTest.out
	mkdir -p coverage/out_EcuReset
	gcov -o obj/EcuReset_test.gcno obj/EcuReset_test.gcda $(UDS_DIR)/ecu_reset/src/EcuReset.cpp
	mv EcuReset.cpp.gcov coverage/out_EcuReset



# RequestDownload Coverage - solo job
requestDownloadCoverage: requestDownload_coverage
	lcov --capture --directory . --output-file coverage.info
	lcov --remove coverage.info '/usr/' --output-file coverage_filtered.info
	lcov --remove coverage.info '/test/' --output-file coverage.info
	genhtml coverage_filtered.info --output-directory coverage/out_RequestDownloadTest
	xdg-open coverage/out_RequestDownloadTest/index.html
# This job runs for allCoverage
requestDownload_coverage: requestDownloadTest
	./$(OTA_DIR)/request_download/utest/requestDownloadTest.out
	mkdir -p coverage/out_RequestDownload
	gcov -o obj/RequestDownload_test.gcno obj/RequestDownload_test.gcda $(OTA_DIR)/request_download/src/RequestDownload.cpp
	mv RequestDownload.cpp.gcov coverage/out_RequestDownload



# TesterPresent Coverage - solo job
testerPresentCoverage: testerPresent_coverage
	lcov --capture --directory . --output-file coverage.info
	lcov --remove coverage.info '/usr/' --output-file coverage_filtered.info
	lcov --remove coverage.info '/test/' --output-file coverage.info
	genhtml coverage_filtered.info --output-directory coverage/out_TesterPresentTest
	xdg-open coverage/out_TesterPresentTest/index.html
# This job runs for allCoverage
testerPresent_coverage: testerPresentTest
	./$(UDS_DIR)/tester_present/utest/testerPresentTest.out
	mkdir -p coverage/out_TesterPresent
	gcov -o obj/TesterPresent_test.gcno obj/TesterPresent_test.gcda $(UDS_DIR)/tester_present/src/TesterPresent.cpp
	mv TesterPresent.cpp.gcov coverage/out_TesterPresent



# TransferData Coverage - solo job
transferDataCoverage: transferData_coverage
	lcov --capture --directory . --output-file coverage.info
	lcov --remove coverage.info '/usr/' --output-file coverage_filtered.info
	lcov --remove coverage.info '/test/' --output-file coverage.info
	genhtml coverage_filtered.info --output-directory coverage/out_TransferDataTest
	xdg-open coverage/out_TransferDataTest/index.html
# This job runs for allCoverage
transferData_coverage: transferDataTest
	./$(OTA_DIR)/transfer_data/utest/transferDataTest.out
	mkdir -p coverage/out_TransferData
	gcov -o obj/TransferData_test.gcno obj/TransferData_test.gcda $(OTA_DIR)/transfer_data/src/TransferData.cpp
	mv TransferData.cpp.gcov coverage/out_TransferData



# AccessTimingParameter Coverage - solo job
accessTimingParameterCoverage: accessTimingParameter_coverage
	lcov --capture --directory . --output-file coverage.info
	lcov --remove coverage.info '/usr/' --output-file coverage_filtered.info
	lcov --remove coverage.info '/test/' --output-file coverage.info
	genhtml coverage_filtered.info --output-directory coverage/out_AccessTimingParameterTest
	xdg-open coverage/out_AccessTimingParameterTest/index.html
# This job runs for allCoverage
accessTimingParameter_coverage: accessTimingParameterTest
	./$(UDS_DIR)/access_timing_parameters/utest/accessTimingParameterTest.out
	mkdir -p coverage/out_AccessTimingParameterTest
	gcov -o obj/AccessTimingParameter_test.gcno obj/AccessTimingParameter_test.gcda $(UDS_DIR)/access_timing_parameters/src/AccessTimingParameter.cpp
	mv AccessTimingParameter.cpp.gcov coverage/out_AccessTimingParameter



# NegativeResponse Coverage - solo job
negativeResponseCoverage: negativeResponse_coverage
	lcov --capture --directory . --output-file coverage.info
	lcov --remove coverage.info '/usr/' --output-file coverage_filtered.info
	lcov --remove coverage.info '/test/' --output-file coverage.info
	genhtml coverage_filtered.info --output-directory coverage/out_NegativeResponseTest
	xdg-open coverage/out_NegativeResponseTest/index.html
# This job runs for allCoverage
negativeResponse_coverage: negativeResponseTest
	./$(UTILS_TEST)/negativeResponseTest.out
	mkdir -p coverage/out_NegativeResponse
	gcov -o obj/NegativeResponse_test.gcno obj/NegativeResponse_test.gcda $(UTILS_DIR)/NegativeResponse.cpp
	mv NegativeResponse.cpp.gcov coverage/out_NegativeResponse



allCoverage: securityAccess_coverage routineControl_coverage transferData_coverage requestDownload_coverage ecuReset_coverage memoryManager_coverage logger_coverage generateFrames_coverage mcuModule_coverage receiveFrames_coverage handleFrames_coverage createInterface_coverage diagnosticSessionControl_coverage writeDataByIdentifier_coverage readDataByIdentifier_coverage testerPresent_coverage requestTransferExit_coverage clearDtc_coverage readDtc_coverage requestUpdateStatus_coverage negativeResponse_coverage accessTimingParameter_coverage


allCoveragePostProcess:
	lcov --capture --directory . --output-file coverage.info
	lcov --remove coverage.info '/usr/*' --output-file coverage_filtered.info
	lcov --remove coverage.info '*/test/*' --output-file coverage.info
	genhtml coverage_filtered.info --output-directory coverage/out_All
	xdg-open coverage/out_All/index.html
	find . -name '*.gcno' -delete
	find . -name '*.gcda' -delete
	find . -name '*.gcov' -delete
	find . -name '*.info' -delete

allCoverage: allCoveragePostProcess

#----------------------------------------------------Clean up--------------------------------------------------------

BASE_DIR_1 := ../uds
BASE_DIR_2 := ../ota
BASE_DIR_UTILS := ../utils

SUB_DIRS_UDS := \
    access_timing_parameters \
    authentication \
    clear_dtc \
    diagnostic_session_control \
    ecu_reset \
    read_data_by_identifier \
    read_dtc_information \
    read_memory_by_adress \
    routine_control \
    tester_present \
    write_data_by_identifier \
	access_timing_parameters
FULL_SUB_DIRS_UDS := $(addprefix $(BASE_DIR_1)/, $(SUB_DIRS_UDS))

SUB_DIRS_OTA := \
    request_download \
    request_transfer_exit \
    request_update_status \
    transfer_data
FULL_SUB_DIRS_OTA := $(addprefix $(BASE_DIR_2)/, $(SUB_DIRS_OTA))

SUB_DIRS_UTILS := \
    src \
    test
FULL_SUB_DIRS_UTILS := $(addprefix $(BASE_DIR_UTILS)/, $(SUB_DIRS_UTILS))

FULL_SUB_DIRS := $(FULL_SUB_DIRS_UDS) $(FULL_SUB_DIRS_OTA)

UTEST_DIRS := $(addsuffix /utest, $(FULL_SUB_DIRS)) $(FULL_SUB_DIRS_UTILS)

FILE_EXT_out := .out
FILE_EXT_o := .o

.PHONY: clean
clean:
	@echo "Cleaning up..."
	rm -f $(FINAL)
	rm -rf $(OBJ_DIR)
	@for dir in $(UTEST_DIRS); do \
		if [ -d $$dir ]; then \
			find $$dir -name "*$(FILE_EXT_out)" -type f -exec rm -f {} +; \
			echo "Removed *$(FILE_EXT_out) files from $$dir"; \
			find $$dir -name "*$(FILE_EXT_o)" -type f -exec rm -f {} +; \
			echo "Removed *$(FILE_EXT_o) files from $$dir"; \
			find $$dir -name '*.gcno' -delete; \
			find $$dir -name '*.gcda' -delete; \
			find $$dir -name '*.gcov' -delete; \
			find $$dir -name '*.info' -delete; \
		fi \
	done
	find . -name '*.gcno' -delete
	find . -name '*.gcda' -delete
	find . -name '*.gcov' -delete
	find . -name '*.info' -delete
	find ./test -name '*.out' -delete
	find ./test -name '*.o' -delete
	if [ -d "./coverage" ]; then find ./coverage -name '*' -delete; fi