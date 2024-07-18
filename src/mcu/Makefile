#Compiler flags
CFLAGS = -std=c++17 -g -Wall -pthread -I./include -I./src -I../utils/include -I../utils/src -I../uds/diagnostic_session_control/include -I../uds/diagnostic_session_control/src
CFLAGSTST = -std=c++17 -I/include
CFLAGSTST2 = -lgtest -lgtest_main -lpthread
LDFLAGS = -lspdlog
CFLAGSTST += -DUNIT_TESTING_MODE
#Sources files
SRCS = src/main.cpp src/MCUModule.cpp ../ecu_simulation/BatteryModule/src/BatteryModule.cpp ../utils/src/GenerateFrames.cpp src/HandleFrames.cpp ../utils/src/CreateInterface.cpp ../uds/diagnostic_session_control/src/DiagnosticSessionControl.cpp src/ReceiveFrames.cpp ../uds/read_data_by_identifier/src/ReadDataByIdentifier.cpp ../uds/write_data_by_identifier/src/WriteDataByIdentifier.cpp ../uds/ecu_reset/src/EcuReset.cpp ../uds/authentication/src/SecurityAccess.cpp  ../ecu_simulation/BatteryModule/src/HandleFrames.cpp  ../ecu_simulation/BatteryModule/src/ReceiveFrames.cpp ../uds/read_dtc_information/src/ReadDtcInformation.cpp ../ota/request_update_status/src/RequestUpdateStatus.cpp ../ota/request_transfer_exit/src/RequestTransferExit.cpp ../ota/request_download/src/RequestDownload.cpp ../utils/src/MemoryManager.cpp
SRCSTEST = src/HandleFrames.cpp src/MCUModule.cpp src/ReceiveFrames.cpp ../utils/src/GenerateFrames.cpp ../utils/src/CreateInterface.cpp ../uds/diagnostic_session_control/src/DiagnosticSessionControl.cpp ../utils/src/Logger.cpp  ../uds/read_data_by_identifier/src/ReadDataByIdentifier.cpp ../uds/write_data_by_identifier/src/WriteDataByIdentifier.cpp ../uds/ecu_reset/src/EcuReset.cpp ../uds/authentication/src/SecurityAccess.cpp ../ecu_simulation/BatteryModule/src/HandleFrames.cpp ../ecu_simulation/BatteryModule/src/ReceiveFrames.cpp ../ecu_simulation/BatteryModule/src/BatteryModule.cpp ../uds/read_dtc_information/src/ReadDtcInformation.cpp ../ota/request_update_status/src/RequestUpdateStatus.cpp ../ota/request_transfer_exit/src/RequestTransferExit.cpp ../ota/request_download/src/RequestDownload.cpp ../utils/src/MemoryManager.cpp
#Object files
OBJS = $(patsubst ../utils/src/%.cpp ../uds/diagnostic_session_control/src/%.cpp src/%.cpp, obj/%.o, $(SRCS))
OBJS:= $(OBJS) obj/Logger.o
#Directories
OBJ_DIR = obj
#TARGET
FINAL = main
FINAL_TEST = test_binary
.PHONY: all clean

all: $(OBJ_DIR) $(FINAL) 
$(FINAL): $(OBJS)
	$(CXX) $(CFLAGS) -o $(FINAL) $(OBJS) ${LDFLAGS}
$(OBJ_DIR):
	mkdir -p $(OBJ_DIR)
$(OBJ_DIR)/main.o: src/main.cpp include/MCUModule.h include/ReceiveFrames.h include/HandleFrames.h ../utils/include/GenerateFrames.h ../utils/include/Logger.h ../utils/include/CreateInterface.h ../uds/diagnostic_session_control/include/DiagnosticSessionControl.h include/MCULogger.h ../uds/read_data_by_identifier/include/ReadDataByIdentifier.h ../uds/write_data_by_identifier/include/WriteDataByIdentifier.h ../uds/ecu_reset/include/EcuReset.h ../uds/authentication/include/SecurityAccess.h ../ecu_simulation/BatteryModule/include/BatteryModule.h   ../ecu_simulation/BatteryModule/include/HandleFrames.h  ../ecu_simulation/BatteryModule/include/ReceiveFrames.h ../uds/read_dtc_information/include/ReadDtcInformation.h ../ota/request_update_status/include/RequestUpdateStatus.h ../ota/request_transfer_exit/include/RequestTransferExit.h ../ota/request_download/include/RequestDownload.h ../utils/include/MemoryManager.h
	$(CXX) $(CFLAGS) -c src/main.cpp -o $(OBJ_DIR)/main.o
$(OBJ_DIR)/MCUModule.o: src/MCUModule.cpp
	$(CXX) $(CFLAGS) -c src/MCUModule.cpp -o $(OBJ_DIR)/MCUModule.o
$(OBJ_DIR)/ReceiveFrames.o: src/ReceiveFrames.cpp
	$(CXX) $(CFLAGS) -c src/ReceiveFrames.cpp -o $(OBJ_DIR)/ReceiveFrames.o
$(OBJ_DIR)/HandleFrames.o: src/HandleFrames.cpp
	$(CXX) $(CFLAGS) -c src/HandleFrames.cpp -o $(OBJ_DIR)/HandleFrames.o
$(OBJ_DIR)/GenerateFrames.o: ../utils/src/GenerateFrames.cpp
	$(CXX) $(CFLAGS) -c ../utils/src/GenerateFrames.cpp -o $(OBJ_DIR)/GenerateFrames.o
$(OBJ_DIR)/CreateInterface.o: ../utils/src/CreateInterface.cpp
	$(CXX) $(CFLAGS) -c ../utils/src/CreateInterface.cpp -o $(OBJ_DIR)/CreateInterface.o
$(OBJ_DIR)/DiagnosticSessionControl.o: ../uds/diagnostic_session_control/src/DiagnosticSessionControl.cpp
	$(CXX) $(CFLAGS) -c ../uds/diagnostic_session_control/src/DiagnosticSessionControl.cpp -o $(OBJ_DIR)/DiagnosticSessionControl.o
$(OBJ_DIR)/Logger.o: ../utils/src/Logger.cpp
	$(CXX) $(CFLAGS) -c ../utils/src/Logger.cpp -o $(OBJ_DIR)/Logger.o
$(OBJ_DIR)/ReadDtcInformation.o: ../uds/read_dtc_information/src/ReadDtcInformation.cpp
	$(CXX) $(CFLAGS) -c ../uds/read_dtc_information/src/ReadDtcInformation.cpp -o $(OBJ_DIR)/ReadDtcInformation.o
$(OBJ_DIR)/RequestTransferExit.o: ../ota/request_transfer_exit/src/RequestTransferExit.cpp
	$(CXX) $(CFLAGS) -c ../ota/request_transfer_exit/src/RequestTransferExit.cpp -o $(OBJ_DIR)/RequestTransferExit.o
$(OBJ_DIR)/RequestDownload.o: ../ota/request_download/src/RequestDownload.cpp
	$(CXX) $(CFLAGS) -c ../ota/request_download/src/RequestDownload.cpp -o $(OBJ_DIR)/RequestDownload.o
$(OBJ_DIR)/RequestUpdateStatus.o: ../ota/request_update_status/src/RequestUpdateStatus.cpp
	$(CXX) $(CFLAGS) -c ../ota/request_update_status/src/RequestUpdateStatus.cpp -o $(OBJ_DIR)/RequestUpdateStatus.o
$(OBJ_DIR)/MemoryManager.o: ../utils/src/MemoryManager.cpp
	$(CXX) $(CFLAGS) -c ../utils/src/MemoryManager.cpp -o $(OBJ_DIR)/MemoryManager.o

clean:
	rm -f $(FINAL)  ${FINAL_TEST} ../utils/test/GenerateFramesTest test/HandleFrames_test test/ReceiveFramesTest ../utils/test/CreateInterface_test ../uds/write_data_by_identifier/utest/WriteDataByIdentifierTest ../uds/read_data_by_identifier/utest/ReadDataByIdentifierTest
	rm -rf $(OBJ_DIR)
#Delete unnecessary file from gcov compiler
	find . -name '*.gcno' -delete
	find . -name '*.gcda' -delete
	find . -name '*.gcov' -delete
	find . -name '*.info' -delete

# Compile all unit tests
allTests: handleTest receiveTest generateTest createInterfaceTest RDBITest wdbiTest RUSTest

# HandleFrames Unit tests
handleTest: HandleFrames_test
HandleFrames_test: test/HandleFrames_test.cpp src/HandleFrames.cpp
	$(CXX) $(CFLAGS) $(CFLAGSTST) -o test/HandleFrames_test test/HandleFrames_test.cpp $(SRCSTEST) $(CFLAGSTST2) $(LDFLAGS)

	../uds/write_data_by_identifier/utest/WriteDataByIdentifierTest.cpp\

# WriteDataByIdentifier Unit tests
wdbiTest: WDBIFrames_test
WDBIFrames_test: ../uds/write_data_by_identifier/utest/WriteDataByIdentifierTest.cpp ../uds/write_data_by_identifier/src/WriteDataByIdentifier.cpp
	$(CXX) $(CFLAGS) $(CFLAGSTST) -o ../uds/write_data_by_identifier/utest/WriteDataByIdentifierTest ../uds/write_data_by_identifier/utest/WriteDataByIdentifierTest.cpp $(SRCSTEST) $(CFLAGSTST2) $(LDFLAGS)

# ReceiveFrames Unit tests
receiveTest: ReceiveFrames_test
ReceiveFrames_test: test/ReceiveFramesTest.cpp src/ReceiveFrames.cpp
	$(CXX) $(CFLAGS) $(CFLAGSTST) -o test/ReceiveFramesTest test/ReceiveFramesTest.cpp $(SRCSTEST) $(CFLAGSTST2) $(LDFLAGS)

# GenerateFrames Unit tests
generateTest: GenerateFrames_test
GenerateFrames_test: ../utils/test/GenerateFramesTest.cpp ../utils/src/GenerateFrames.cpp
	$(CXX) $(CFLAGS) $(CFLAGSTST) -o ../utils/test/GenerateFramesTest ../utils/test/GenerateFramesTest.cpp $(SRCSTEST) $(CFLAGSTST2) $(LDFLAGS)

# MemoryManager Unit tests
memoryTest: MemoryManager_test
MemoryManager_test: ../utils/test/MemoryManagerTest.cpp ../utils/src/MemoryManager.cpp
	$(CXX) $(CFLAGS) $(CFLAGSTST) -o ../utils/test/MemoryManager_test ../utils/test/MemoryManagerTest.cpp ../utils/src/MemoryManager.cpp ../utils/src/Logger.cpp $(CFLAGSTST2) $(LDFLAGS)


# CreateInterface Unit tests
createInterfaceTest: CreateInterface_test
CreateInterface_test: ../utils/test/CreateInterface_test.cpp ../utils/src/CreateInterface.cpp
	$(CXX) $(CFLAGS) $(CFLAGSTST) -o ../utils/test/CreateInterface_test ../utils/test/CreateInterface_test.cpp ../utils/src/CreateInterface.cpp ../utils/src/Logger.cpp $(CFLAGSTST2) $(LDFLAGS)

# ReadDataByIdentifier Unit tests
RDBITest: ReadDataByIdentifier_test
ReadDataByIdentifier_test: ../uds/read_data_by_identifier/utest/ReadDataByIdentifierTest.cpp ../uds/read_data_by_identifier/src/ReadDataByIdentifier.cpp
	$(CXX) $(CFLAGS) $(CFLAGSTST) -o ../uds/read_data_by_identifier/utest/ReadDataByIdentifierTest ../uds/read_data_by_identifier/utest/ReadDataByIdentifierTest.cpp $(SRCSTEST) $(CFLAGSTST2) $(LDFLAGS)

# RequestTransferExit Unit tests
RTESTest: RequestTransferExit_test
RequestTransferExit_test: ../ota/request_transfer_exit/utest/RequestTransferExitTest.cpp ../ota/request_transfer_exit/src/RequestTransferExit.cpp
	$(CXX) $(CFLAGS) $(CFLAGSTST) -o ../ota/request_transfer_exit/utest/RequestTransferExitTest ../ota/request_transfer_exit/utest/RequestTransferExitTest.cpp $(SRCSTEST) $(CFLAGSTST2) $(LDFLAGS)

# ReadDTC Unit tests
readDtcTest: readDtc_test
readDtc_test: ../uds/read_dtc_information/utest/ReadDtcInformationTest.cpp ../uds/read_dtc_information/src/ReadDtcInformation.cpp
	$(CXX) $(CFLAGS) $(CFLAGSTST) -o ../uds/read_dtc_information/utest/ReadDtcInformationTest ../uds/read_dtc_information/utest/ReadDtcInformationTest.cpp ../uds/read_dtc_information/src/ReadDtcInformation.cpp ../utils/src/GenerateFrames.cpp ../utils/src/CreateInterface.cpp ../utils/src/Logger.cpp $(CFLAGSTST2) $(LDFLAGS)

# RequestUpdateStatus Unit tests
RUSTest: RequestUpdateStatus_test
RequestUpdateStatus_test: ../ota/request_update_status/utest/RequestUpdateStatusTest.cpp ../ota/request_update_status/src/RequestUpdateStatus.cpp
	$(CXX) $(CFLAGS) $(CFLAGSTST) -o ../ota/request_update_status/utest/RequestUpdateStatusTest ../ota/request_update_status/utest/RequestUpdateStatusTest.cpp $(SRCSTEST) $(CFLAGSTST2) $(LDFLAGS)

# HandleFrames Coverage
handleCoverage: HandleFramesCoverage
HandleFramesCoverage: test/HandleFrames_test.cpp src/HandleFrames.cpp
	$(CXX) $(CFLAGS) $(CFLAGSTST) -fprofile-arcs -ftest-coverage -o test/HandleFrames_test test/HandleFrames_test.cpp $(SRCSTEST) $(CFLAGSTST2) $(LDFLAGS)
	./test/HandleFrames_test
	mkdir -p coverage/out_HandleFrames
	gcov -o . src/HandleFrames.cpp
	mv HandleFrames.cpp.gcov coverage/out_HandleFrames
	lcov --capture --directory . --output-file coverage.info
	lcov --remove coverage.info '/usr/' --output-file coverage_filtered.info
	lcov --remove coverage.info '/test/' --output-file coverage.info
	genhtml coverage_filtered.info --output-directory coverage/out_HandleFrames
	xdg-open coverage/out_HandleFrames/index.html

# ReceiveFrames Coverage
receiveCoverage: ReceiveFramesCoverage
ReceiveFramesCoverage: test/ReceiveFramesTest.cpp src/ReceiveFrames.cpp
	  $(CXX) $(CFLAGS) $(CFLAGSTST) -fprofile-arcs -ftest-coverage -o test/ReceiveFramesTest test/ReceiveFramesTest.cpp $(SRCSTEST) $(CFLAGSTST2) $(LDFLAGS)
	./test/ReceiveFramesTest
	mkdir -p coverage/out_ReceiveFrames
	gcov -o . src/ReceiveFrames.cpp
	mv ReceiveFrames.cpp.gcov coverage/out_ReceiveFrames
	lcov --capture --directory . --output-file coverage.info
	lcov --remove coverage.info '/usr/*' --output-file coverage_filtered.info
	lcov --remove coverage.info '*/test/*' --output-file coverage.info
	genhtml coverage_filtered.info --output-directory coverage/out_ReceiveFrames
	xdg-open coverage/out_ReceiveFrames/index.html
	find . -name '*.gcno' -delete
	find . -name '*.gcda' -delete
	find . -name '*.gcov' -delete

# generateFrames Coverage
generateCoverage: GenerateFramesCoverage
GenerateFramesCoverage: ../utils/test/GenerateFramesTest.cpp ../utils/src/GenerateFrames.cpp
	  $(CXX) $(CFLAGS) $(CFLAGSTST) -fprofile-arcs -ftest-coverage -o ../utils/test/GenerateFramesTest ../utils/test/GenerateFramesTest.cpp $(SRCSTEST) $(CFLAGSTST2) $(LDFLAGS)
	./../utils/test/GenerateFramesTest
	mkdir -p coverage/out_GenerateFrames
	gcov -o . ../utils/src/GenerateFrames.cpp
	mv GenerateFrames.cpp.gcov coverage/out_GenerateFrames
	lcov --capture --directory . --output-file coverage.info
	lcov --remove coverage.info '/usr/*' --output-file coverage_filtered.info
	lcov --remove coverage.info '*/test/*' --output-file coverage.info
	genhtml coverage_filtered.info --output-directory coverage/out_GenerateFrames
	xdg-open coverage/out_GenerateFrames/index.html
	find . -name '*.gcno' -delete
	find . -name '*.gcda' -delete
	find . -name '*.gcov' -delete

# MemoryManager Coverage
memoryCoverage: MemoryManagerCoverage
MemoryManagerCoverage: ../utils/test/MemoryManagerTest.cpp ../utils/src/MemoryManager.cpp
	$(CXX) $(CFLAGS) $(CFLAGSTST)  -fprofile-arcs -ftest-coverage -o ../utils/test/MemoryManager_test ../utils/test/MemoryManagerTest.cpp ../utils/src/MemoryManager.cpp ../utils/src/Logger.cpp $(CFLAGSTST2) $(LDFLAGS)
	sudo ./../utils/test/MemoryManager_test
	mkdir -p coverage/out_MemoryManager
	gcov -o . ../utils/src/MemoryManager.cpp
	mv MemoryManager.cpp.gcov coverage/out_MemoryManager
	lcov --capture --directory . --output-file coverage.info
	lcov --remove coverage.info '/usr/*' --output-file coverage_filtered.info
	lcov --remove coverage.info '*/test/*' --output-file coverage.info
	genhtml coverage_filtered.info --output-directory coverage/out_MemoryManager
	xdg-open coverage/out_MemoryManager/index.html
	find . -name '*.gcno' -delete
	find . -name '*.gcda' -delete
	find . -name '*.gcov' -delete

# CreateInterface Coverage
createInterfaceCoverage: CreateInterfaceCoverage
CreateInterfaceCoverage: ../utils/test/CreateInterface_test.cpp ../utils/src/CreateInterface.cpp
	  $(CXX) $(CFLAGS) $(CFLAGSTST) -fprofile-arcs -ftest-coverage -o ../utils/test/CreateInterface_test ../utils/test/CreateInterface_test.cpp ../utils/src/CreateInterface.cpp ../utils/src/Logger.cpp $(CFLAGSTST2) $(LDFLAGS)
	./../utils/test/CreateInterface_test
	mkdir -p coverage/out_CreateInterface
	gcov -o . ../utils/src/CreateInterface.cpp
	mv CreateInterface.cpp.gcov coverage/out_CreateInterface
	lcov --capture --directory . --output-file coverage.info
	lcov --remove coverage.info '/usr/*' --output-file coverage_filtered.info
	lcov --remove coverage.info '*/test/*' --output-file coverage.info
	genhtml coverage_filtered.info --output-directory coverage/out_CreateInterface
	xdg-open coverage/out_CreateInterface/index.html
	find . -name '*.gcno' -delete
	find . -name '*.gcda' -delete
	find . -name '*.gcov' -delete

# ReadDataByIdentifier Coverage
RDBICoverage: ReadDataByIdentifierCoverage
ReadDataByIdentifierCoverage: ../uds/read_data_by_identifier/utest/ReadDataByIdentifierTest.cpp ../uds/read_data_by_identifier/src/ReadDataByIdentifier.cpp
	  $(CXX) $(CFLAGS) $(CFLAGSTST) -fprofile-arcs -ftest-coverage -o ../uds/read_data_by_identifier/utest/ReadDataByIdentifierTest ../uds/read_data_by_identifier/utest/ReadDataByIdentifierTest.cpp $(SRCSTEST) $(CFLAGSTST2) $(LDFLAGS)
	./../uds/read_data_by_identifier/utest/ReadDataByIdentifierTest
	mkdir -p coverage/out_ReadDataByIdentifier
	gcov -o . ../uds/read_data_by_identifier/src/ReadDataByIdentifier.cpp
	mv ReadDataByIdentifier.cpp.gcov coverage/out_ReadDataByIdentifier
	lcov --capture --directory . --output-file coverage.info
	lcov --remove coverage.info '/usr/*' --output-file coverage_filtered.info
	lcov --remove coverage.info '*/test/*' --output-file coverage.info
	genhtml coverage_filtered.info --output-directory coverage/out_ReadDataByIdentifier
	xdg-open coverage/out_ReadDataByIdentifier/index.html
	find . -name '*.gcno' -delete
	find . -name '*.gcda' -delete
	find . -name '*.gcov' -delete
  
# WriteDayaByIdentifier Coverage
wdbiCoverage: WDBICoverage
WDBICoverage: ../uds/write_data_by_identifier/utest/WriteDataByIdentifierTest.cpp ../uds/write_data_by_identifier/src/WriteDataByIdentifier.cpp
	$(CXX) $(CFLAGS) $(CFLAGSTST) -fprofile-arcs -ftest-coverage -o ../uds/write_data_by_identifier/utest/WriteDataByIdentifierTest ../uds/write_data_by_identifier/utest/WriteDataByIdentifierTest.cpp $(SRCSTEST) $(CFLAGSTST2) $(LDFLAGS)
	../uds/write_data_by_identifier/utest/WriteDataByIdentifierTest
	mkdir -p coverage/out_WriteDataByIdentifierTest
	gcov -o . ../uds/write_data_by_identifier/src/WriteDataByIdentifier.cpp
	mv WriteDataByIdentifier.cpp.gcov coverage/out_WriteDataByIdentifierTest
	lcov --capture --directory . --output-file coverage.info
	lcov --remove coverage.info '/usr/' --output-file coverage_filtered.info
	lcov --remove coverage.info '/test/' --output-file coverage.info
	genhtml coverage_filtered.info --output-directory coverage/out_WriteDataByIdentifierTest
	xdg-open coverage/out_WriteDataByIdentifierTest/index.html

RUSCoverage: RequestUpdateStatusCoverage
RequestUpdateStatusCoverage: ../ota/request_update_status/utest/RequestUpdateStatusTest.cpp ../ota/request_update_status/src/RequestUpdateStatus.cpp
	  $(CXX) $(CFLAGS) $(CFLAGSTST) -fprofile-arcs -ftest-coverage -o ../ota/request_update_status/utest/RequestUpdateStatusTest ../ota/request_update_status/utest/RequestUpdateStatusTest.cpp $(SRCSTEST) $(CFLAGSTST2) $(LDFLAGS)
	../ota/request_update_status/utest/RequestUpdateStatusTest
	mkdir -p coverage/out_RequestUpdateStatus
	gcov -o . ../ota/request_update_status/src/RequestUpdateStatus.cpp
	mv RequestUpdateStatus.cpp.gcov coverage/out_RequestUpdateStatus
	lcov --capture --directory . --output-file coverage.info
	lcov --remove coverage.info '/usr/*' --output-file coverage_filtered.info
	lcov --remove coverage.info '*/test/*' --output-file coverage.info
	genhtml coverage_filtered.info --output-directory coverage/out_RequestUpdateStatus
	xdg-open coverage/out_RequestUpdateStatus/index.html
#  RequestTransferExit Coverage
RTESCoverage: RequestTransferExitCoverage
RequestTransferExitCoverage: ../ota/request_transfer_exit/utest/RequestTransferExitTest.cpp ../ota/request_transfer_exit/src/RequestTransferExit.cpp
	  $(CXX) $(CFLAGS) $(CFLAGSTST) -fprofile-arcs -ftest-coverage -o ../ota/request_transfer_exit/utest/RequestTransferExitTest ../ota/request_transfer_exit/utest/RequestTransferExitTest.cpp $(SRCSTEST) $(CFLAGSTST2) $(LDFLAGS)
	../ota/request_transfer_exit/utest/RequestTransferExitTest
	mkdir -p coverage/out_RequestTransferExit
	gcov -o . ../ota/request_transfer_exit/src/RequestTransferExitTest.cpp
	mv RequestTransferExitTest.cpp.gcov coverage/out_RequestTransferExitTest
	lcov --capture --directory . --output-file coverage.info
	lcov --remove coverage.info '/usr/*' --output-file coverage_filtered.info
	lcov --remove coverage.info '*/test/*' --output-file coverage.info
	genhtml coverage_filtered.info --output-directory coverage/out_RequestTransferExit
	xdg-open coverage/out_RequestTransferExit/index.html
	find . -name '*.gcno' -delete
	find . -name '*.gcda' -delete
	find . -name '*.gcov' -delete

allCoverage: HandleFrames_coverage ReceiveFrames_coverage GenerateFrames_coverage CreateInterface_coverage ReadDataByIdentifier_coverage WDBI_coverage RUSCoverage

HandleFrames_coverage:
	$(CXX) $(CFLAGS) $(CFLAGSTST) -fprofile-arcs -ftest-coverage -o test/HandleFrames_test test/HandleFrames_test.cpp $(SRCSTEST) $(CFLAGSTST2) $(LDFLAGS)
	./test/HandleFrames_test
	mkdir -p coverage/out_HandleFrames
	gcov -o . src/HandleFrames.cpp
	mv HandleFrames.cpp.gcov coverage/out_HandleFrames

ReceiveFrames_coverage:
	$(CXX) $(CFLAGS) $(CFLAGSTST) -fprofile-arcs -ftest-coverage -o test/ReceiveFramesTest test/ReceiveFramesTest.cpp $(SRCSTEST) $(CFLAGSTST2) $(LDFLAGS)
	./test/ReceiveFramesTest
	mkdir -p coverage/out_ReceiveFrames
	gcov -o . src/ReceiveFrames.cpp
	mv ReceiveFrames.cpp.gcov coverage/out_ReceiveFrames

GenerateFrames_coverage:
	$(CXX) $(CFLAGS) $(CFLAGSTST) -fprofile-arcs -ftest-coverage -o test/GenerateFrames_Test test/GenerateFrames_Test.cpp $(SRCSTEST) $(CFLAGSTST2) $(LDFLAGS)
	./test/GenerateFrames_Test
	mkdir -p coverage/out_GenerateFrames
	gcov -o . ../utils/src/GenerateFrames.cpp
	mv GenerateFrames.cpp.gcov coverage/out_GenerateFrames

CreateInterface_coverage:
	$(CXX) $(CFLAGS) $(CFLAGSTST) -fprofile-arcs -ftest-coverage -o ../utils/test/CreateInterface_test ../utils/test/CreateInterface_test.cpp ../utils/src/CreateInterface.cpp ../utils/src/Logger.cpp $(CFLAGSTST2) $(LDFLAGS)
	./../utils/test/CreateInterface_test
	mkdir -p coverage/out_CreateInterface
	gcov -o . ../utils/src/CreateInterface.cpp
	mv CreateInterface.cpp.gcov coverage/out_CreateInterface

ReadDataByIdentifier_coverage: ../uds/read_data_by_identifier/utest/ReadDataByIdentifierTest.cpp ../uds/read_data_by_identifier/src/ReadDataByIdentifier.cpp
	  $(CXX) $(CFLAGS) $(CFLAGSTST) -fprofile-arcs -ftest-coverage -o ../uds/read_data_by_identifier/utest/ReadDataByIdentifierTest ../uds/read_data_by_identifier/utest/ReadDataByIdentifierTest.cpp $(SRCSTEST) $(CFLAGSTST2) $(LDFLAGS)
	./../uds/read_data_by_identifier/utest/ReadDataByIdentifierTest
	mkdir -p coverage/out_ReadDataByIdentifier
	gcov -o . ../uds/read_data_by_identifier/src/ReadDataByIdentifier.cpp
	mv ReadDataByIdentifier.cpp.gcov coverage/out_ReadDataByIdentifier
  
WDBI_coverage:
	$(CXX) $(CFLAGS) $(CFLAGSTST) -fprofile-arcs -ftest-coverage -o ../uds/write_data_by_identifier/utest/WriteDataByIdentifierTest ../uds/write_data_by_identifier/utest/WriteDataByIdentifierTest.cpp $(SRCSTEST) $(CFLAGSTST2) $(LDFLAGS)
	../uds/write_data_by_identifier/utest/WriteDataByIdentifierTest
	mkdir -p coverage/out_	mkdir -p coverage/out_WriteDataByIdentifier
	gcov -o . ../uds/write_data_by_identifier/src/WriteDataByIdentifier.cpp
	mv WriteDataByIdentifier.cpp.gcov coverage/out_WriteDataByIdentifier

RequestUpdateStatus_coverage: ../ota/request_update_status/src/RequestUpdateStatusTest.cpp ../ota/request_update_status/src/RequestUpdateStatus.cpp
	  $(CXX) $(CFLAGS) $(CFLAGSTST) -fprofile-arcs -ftest-coverage -o ../ota/request_update_status/src/RequestUpdateStatusTest ../ota/request_update_status/src/RequestUpdateStatusTest.cpp $(SRCSTEST) $(CFLAGSTST2) $(LDFLAGS)
	../ota/request_update_status/src/RequestUpdateStatus
	mkdir -p coverage/out_RequestUpdateStatus
	gcov -o . ../ota/request_update_status/src/RequestUpdateStatus.cpp
	mv RequestUpdateStatus.cpp.gcov coverage/out_RequestUpdateStatus
  
RequestTransferExit_coverage: ../ota/request_transfer_exit/utest/RequestTransferExitTest.cpp ../ota/request_transfer_exit/src/RequestTransferExitTest.cpp
	  $(CXX) $(CFLAGS) $(CFLAGSTST) -fprofile-arcs -ftest-coverage -o ../ota/request_transfer_exit/utest/RequestTransferExitTest ../ota/request_transfer_exit/utest/RequestTransferExitTest.cpp $(SRCSTEST) $(CFLAGSTST2) $(LDFLAGS)
	../../ota/request_transfer_exit/src/RequestTransferExitTest
	mkdir -p coverage/out_RequestTransferExit
	gcov -o . ../ota/request_transfer_exit/utest/RequestTransferExitTest.cpp
	mv RequestTransferExitTest.cpp.gcov coverage/out_RequestTransferExitTest

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