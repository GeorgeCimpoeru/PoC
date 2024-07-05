#Compiler flags
CFLAGS = -g -Wall -pthread -I./include -I./src -I../../utils/include -I../../utils/src -I../../uds/include -I../../uds/src
CFLAGSTST = -std=c++17 -I/include -DUNIT_TESTING_MODE
LDFLAGS = -lspdlog
CFLAGSTST2 = -lgtest -lgtest_main -pthread
#Sources files
SRCS = src/main.cpp src/BatteryModule.cpp ../../utils/src/GenerateFrames.cpp src/HandleFrames.cpp ../../utils/src/CreateInterface.cpp ../../uds/diagnostic_session_control/src/DiagnosticSessionControl.cpp src/ReceiveFrames.cpp ../../uds/read_data_by_identifier/src/ReadDataByIdentifier.cpp ../../mcu/src/HandleFrames.cpp ../../mcu/src/ReceiveFrames.cpp ../../mcu/src/MCUModule.cpp ../../uds/read_dtc_information/src/ReadDtcInformation.cpp
SRCSTEST = src/HandleFrames.cpp src/BatteryModule.cpp src/ReceiveFrames.cpp ../../utils/src/GenerateFrames.cpp ../../utils/src/CreateInterface.cpp ../../uds/diagnostic_session_control/src/DiagnosticSessionControl.cpp ../../utils/src/Logger.cpp ../../uds/read_data_by_identifier/src/ReadDataByIdentifier.cpp ../../mcu/src/HandleFrames.cpp ../../mcu/src/ReceiveFrames.cpp ../../mcu/src/MCUModule.cpp ../../uds/read_dtc_information/src/ReadDtcInformation.cpp
#Object files
OBJS = $(patsubst ../uds/src/%.cpp ../utils/src/%.cpp src/%.cpp, obj/%.o, $(SRCS))
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
$(OBJ_DIR)/main.o: src/main.cpp include/BatteryModule.h include/ReceiveFrames.h include/HandleFrames.h include/GenerateFrames.h ../../utils/include/CreateInterface.h ../../uds/diagnostic_session_control/include/DiagnosticSessionControl.h ../../utils/include/Logger.h include/BatteryModuleLogger.h ../../uds/read_data_by_identifier/include/ReadDataByIdentifier.h ../../mcu/include/HandleFrames.h ../../mcu/include/ReceiveFrames.h ../../mcu/include/MCUModule.h ../../uds/read_dtc_information/include/ReadDtcInformation.h
	$(CXX) $(CFLAGS) -c src/main.cpp -o $(OBJ_DIR)/main.o
$(OBJ_DIR)/BatteryModule.o: src/BatteryModule.cpp
	$(CXX) $(CFLAGS) -c src/BatteryModule.cpp -o $(OBJ_DIR)/BatteryModule.o
$(OBJ_DIR)/ReceiveFrames.o: src/ReceiveFrames.cpp
	$(CXX) $(CFLAGS) -c src/ReceiveFrames.cpp -o $(OBJ_DIR)/ReceiveFrames.o
$(OBJ_DIR)/HandleFrames.o: src/HandleFrames.cpp
	$(CXX) $(CFLAGS) -c src/HandleFrames.cpp -o $(OBJ_DIR)/HandleFrames.o
$(OBJ_DIR)/GenerateFrames.o: ../../utils/src/src/GenerateFrames.cpp
	$(CXX) $(CFLAGS) -c ../../utils/src/src/GenerateFrames.cpp -o $(OBJ_DIR)/GenerateFrames.o
$(OBJ_DIR)/CreateInterface.o: ../../utils/src/CreateInterface.cpp
	$(CXX) $(CFLAGS) -c ../../utils/src/CreateInterface.cpp -o $(OBJ_DIR)/CreateInterface.o
$(OBJ_DIR)/DiagnosticSessionControl.o: ../../uds/diagnostic_session_control/src/DiagnosticSessionControl.cpp
	$(CXX) $(CFLAGS) -c ../../uds/diagnostic_session_control/src/DiagnosticSessionControl.cpp -o $(OBJ_DIR)/DiagnosticSessionControl.o	
$(OBJ_DIR)/Logger.o: ../../utils/src/Logger.cpp
	$(CXX) $(CFLAGS) -c ../../utils/src/Logger.cpp -o $(OBJ_DIR)/Logger.o
$(OBJ_DIR)/ReadDtcInformation.o: ../../uds/read_dtc_information/src/ReadDtcInformation.cpp
	$(CXX) $(CFLAGS) -c ../../uds/read_dtc_information/src/ReadDtcInformation.cpp -o $(OBJ_DIR)/ReadDtcInformation.o

clean:
	rm -f $(FINAL)  ${FINAL_TEST} test/BatteryModuleTest ../../utils/test/GenerateFramesTest test/ReceiveFramesTest ../../utils/test/CreateInterface_test
	rm -rf $(OBJ_DIR)
#Delete unnecessary file from gcov compiler
	find . -name '*.gcno' -delete
	find . -name '*.gcda' -delete
	find . -name '*.gcov' -delete
	find . -name '*.info' -delete

# Compile all unit tests
allTests: handleTest receiveTest generateTest createInterfaceTest batteryModuleTest
# BatteryModule Unit tests
batteryModuleTest: BatteryModuleTest
BatteryModuleTest: test/BatteryModuleTest.cpp src/BatteryModule.cpp
	$(CXX) $(CFLAGS) $(CFLAGSTST) -o test/BatteryModuleTest test/BatteryModuleTest.cpp $(SRCSTEST) $(CFLAGSTST2) $(LDFLAGS)

# HandleFrames Unit tests
handleTest: HandleFrames_test
HandleFrames_test: test/HandleFrames_test.cpp src/HandleFrames.cpp
	$(CXX) $(CFLAGS) $(CFLAGSTST) -o test/HandleFrames_test test/HandleFrames_test.cpp $(SRCSTEST) $(CFLAGSTST2) $(LDFLAGS)

# ReceiveFrames Unit tests
receiveTest: ReceiveFrames_test
ReceiveFrames_test: test/ReceiveFramesTest.cpp src/ReceiveFrames.cpp
	$(CXX) $(CFLAGS) $(CFLAGSTST) -o test/ReceiveFramesTest test/ReceiveFramesTest.cpp $(SRCSTEST) $(CFLAGSTST2) $(LDFLAGS)

# GenerateFrames Unit tests
generateTest: GenerateFrames_test
GenerateFrames_test: ../../utils/test/GenerateFramesTest.cpp ../../utils/src/GenerateFrames.cpp
	$(CXX) $(CFLAGS) $(CFLAGSTST) -o ../../utils/test/GenerateFramesTest ../../utils/test/GenerateFramesTest.cpp $(SRCSTEST) $(CFLAGSTST2) $(LDFLAGS)

# CreateInterface Unit tests
createInterfaceTest: CreateInterface_test
CreateInterface_test: ../../utils/test/CreateInterface_test.cpp ../../utils/src/CreateInterface.cpp
	$(CXX) $(CFLAGS) $(CFLAGSTST) -o ../../utils/test/CreateInterface_test ../../utils/test/CreateInterface_test.cpp $(SRCSTEST) $(CFLAGSTST2) $(LDFLAGS)

readDtcTest: readDtc_test
readDtc_test: ../../uds/read_dtc_information/utest/ReadDtcInformationTest.cpp ../../uds/read_dtc_information/src/ReadDtcInformation.cpp
	$(CXX) $(CFLAGS) $(CFLAGSTST) -o ../../uds/read_dtc_information/utest/ReadDtcInformationTest ../../uds/read_dtc_information/utest/ReadDtcInformationTest.cpp ../../uds/read_dtc_information/src/ReadDtcInformation.cpp ../../utils/src/GenerateFrames.cpp ../../utils/src/CreateInterface.cpp ../../utils/src/Logger.cpp $(CFLAGSTST2) $(LDFLAGS)

# BatteryModule Coverage
batteryModuleCoverage: BatteryModuleCoverage
BatteryModuleCoverage: test/BatteryModuleTest.cpp src/BatteryModule.cpp
	$(CXX) $(CFLAGS) $(CFLAGSTST) -fprofile-arcs -ftest-coverage -o test/BatteryModuleTest test/BatteryModuleTest.cpp $(SRCSTEST) $(CFLAGSTST2) $(LDFLAGS)
	./test/BatteryModuleTest
	mkdir -p coverage/out_BatteryModule
	gcov -o . src/BatteryModule.cpp
	mv BatteryModule.cpp.gcov coverage/out_BatteryModule
	lcov --capture --directory . --output-file coverage.info
	lcov --remove coverage.info '/usr/' --output-file coverage_filtered.info
	lcov --remove coverage.info '/test/' --output-file coverage.info
	genhtml coverage_filtered.info --output-directory coverage/out_BatteryModule
	xdg-open coverage/out_BatteryModule/index.html
	find . -name '.gcno' -delete
	find . -name '.gcda' -delete
	find . -name '.gcov' -delete

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
	find . -name '.gcno' -delete
	find . -name '.gcda' -delete
	find . -name '.gcov' -delete

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
GenerateFramesCoverage: ../../utils/test/GenerateFramesTest.cpp ../../utils/src/GenerateFrames.cpp
	  $(CXX) $(CFLAGS) $(CFLAGSTST) -fprofile-arcs -ftest-coverage -o ../../utils/test/GenerateFramesTest ../../utils/test/GenerateFramesTest.cpp $(SRCSTEST) $(CFLAGSTST2) $(LDFLAGS)
	./test/GenerateFramesTest
	mkdir -p coverage/out_GenerateFrames
	gcov -o . ../../utils/src/GenerateFrames.cpp
	mv GenerateFrames.cpp.gcov coverage/out_GenerateFrames
	lcov --capture --directory . --output-file coverage.info
	lcov --remove coverage.info '/usr/*' --output-file coverage_filtered.info
	lcov --remove coverage.info '*/test/*' --output-file coverage.info
	genhtml coverage_filtered.info --output-directory coverage/out_GenerateFrames
	xdg-open coverage/out_GenerateFrames/index.html
	find . -name '*.gcno' -delete
	find . -name '*.gcda' -delete
	find . -name '*.gcov' -delete

# CreateInterface Coverage
createInterfaceCoverage: CreateInterfaceCoverage
CreateInterfaceCoverage: ../../utils/test/CreateInterface_test.cpp ../../utils/src/CreateInterface.cpp
	  $(CXX) $(CFLAGS) $(CFLAGSTST) -fprofile-arcs -ftest-coverage -o ../../utils/test/CreateInterface_test ../../utils/test/CreateInterface_test.cpp $(SRCSTEST) $(CFLAGSTST2) $(LDFLAGS)
	./../../utils/test/CreateInterface_test
	mkdir -p coverage/out_CreateInterface
	gcov -o . ../../utils/src/CreateInterface.cpp
	mv CreateInterface.cpp.gcov coverage/out_CreateInterface
	lcov --capture --directory . --output-file coverage.info
	lcov --remove coverage.info '/usr/*' --output-file coverage_filtered.info
	lcov --remove coverage.info '*/test/*' --output-file coverage.info
	genhtml coverage_filtered.info --output-directory coverage/out_CreateInterface
	xdg-open coverage/out_CreateInterface/index.html
	find . -name '*.gcno' -delete
	find . -name '*.gcda' -delete
	find . -name '*.gcov' -delete

allCoverage: BatteryModule_coverage HandleFrames_coverage ReceiveFrames_coverage GenerateFrames_coverage CreateInterface_coverage

BatteryModule_coverage:
	$(CXX) $(CFLAGS) $(CFLAGSTST) -fprofile-arcs -ftest-coverage -o test/BatteryModuleTest test/BatteryModuleTest.cpp $(SRCSTEST) $(CFLAGSTST2) $(LDFLAGS)
	./test/BatteryModuleTest
	mkdir -p coverage/out_BatteryModule
	gcov -o . src/BatteryModule.cpp
	mv BatteryModule.cpp.gcov coverage/out_BatteryModule

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
	$(CXX) $(CFLAGS) $(CFLAGSTST) -fprofile-arcs -ftest-coverage -o ../../utils/test/GenerateFrames_Test ../../utils/test/GenerateFrames_Test.cpp $(SRCSTEST) $(CFLAGSTST2) $(LDFLAGS)
	./test/GenerateFrames_Test
	mkdir -p coverage/out_GenerateFrames
	gcov -o . ../../utils/src/GenerateFrames.cpp
	mv GenerateFrames.cpp.gcov coverage/out_GenerateFrames

CreateInterface_coverage:
	$(CXX) $(CFLAGS) $(CFLAGSTST) -fprofile-arcs -ftest-coverage -o ../utils/test/CreateInterface_test ../utils/test/CreateInterface_test.cpp $(SRCSTEST) $(CFLAGSTST2) $(LDFLAGS)
	./../utils/test/CreateInterface_test
	mkdir -p coverage/out_CreateInterface
	gcov -o . ../../utils/src/CreateInterface.cpp
	mv CreateInterface.cpp.gcov coverage/out_CreateInterface

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