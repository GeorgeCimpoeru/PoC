#Compiler flags
CFLAGS = -g -Wall -pthread -I./include -I./src -I../utils/include -I../utils/src
CFLAGSTST = -std=c++17 -I/include
CFLAGSTST2 = -lgtest -lgtest_main -lpthread
LDFLAGS = -lspdlog
CFLAGSTST += -DUNIT_TESTING_MODE
#Sources files
SRCS = src/main.cpp src/MCUModule.cpp ../ecu_simulation/BatteryModule/src/BatteryModule.cpp ../utils/src/GenerateFrames.cpp src/HandleFrames.cpp ../utils/src/CreateInterface.cpp src/ReceiveFrames.cpp ../uds/read_data_by_identifier/src/ReadDataByIdentifier.cpp  ../uds/authentication/src/SecurityAccess.cpp ../ecu_simulation/BatteryModule/src/HandleFrames.cpp  ../ecu_simulation/BatteryModule/src/ReceiveFrames.cpp 
SRCSTEST = src/HandleFrames.cpp src/MCUModule.cpp src/ReceiveFrames.cpp ../utils/src/GenerateFrames.cpp ../utils/src/CreateInterface.cpp ../utils/src/Logger.cpp  ../uds/read_data_by_identifier/src/ReadDataByIdentifier.cpp ../uds/authentication/src/SecurityAccess.cpp ../ecu_simulation/BatteryModule/src/HandleFrames.cpp ../ecu_simulation/BatteryModule/src/ReceiveFrames.cpp ../ecu_simulation/BatteryModule/src/BatteryModule.cpp
#Object files
OBJS = $(patsubst ../utils/src/%.cpp src/%.cpp, obj/%.o, $(SRCS))
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
$(OBJ_DIR)/main.o: src/main.cpp include/MCUModule.h include/ReceiveFrames.h include/HandleFrames.h ../utils/include/GenerateFrames.h ../utils/include/Logger.h ../utils/include/CreateInterface.h include/MCULogger.h ../uds/read_data_by_identifier/include/ReadDataByIdentifier.h ../uds/authentication/include/SecurityAccess.h ../ecu_simulation/BatteryModule/include/BatteryModule.h   ../ecu_simulation/BatteryModule/include/HandleFrames.h  ../ecu_simulation/BatteryModule/include/ReceiveFrames.h
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
$(OBJ_DIR)/Logger.o: ../utils/src/Logger.cpp
	$(CXX) $(CFLAGS) -c ../utils/src/Logger.cpp -o $(OBJ_DIR)/Logger.o

clean:
	rm -f $(FINAL)  ${FINAL_TEST} ../utils/test/GenerateFramesTest test/HandleFrames_test test/ReceiveFramesTest ../utils/test/CreateInterface_test
	rm -rf $(OBJ_DIR)
#Delete unnecessary file from gcov compiler
	find . -name '*.gcno' -delete
	find . -name '*.gcda' -delete
	find . -name '*.gcov' -delete
	find . -name '*.info' -delete

# Compile all unit tests
allTests: handleTest receiveTest generateTest createInterfaceTest
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
GenerateFrames_test: ../utils/test/GenerateFramesTest.cpp ../utils/src/GenerateFrames.cpp
	$(CXX) $(CFLAGS) $(CFLAGSTST) -o ../utils/test/GenerateFramesTest ../utils/test/GenerateFramesTest.cpp $(SRCSTEST) $(CFLAGSTST2) $(LDFLAGS)

# CreateInterface Unit tests
createInterfaceTest: CreateInterface_test
CreateInterface_test: ../utils/test/CreateInterface_test.cpp ../utils/src/CreateInterface.cpp
	$(CXX) $(CFLAGS) $(CFLAGSTST) -o ../utils/test/CreateInterface_test ../utils/test/CreateInterface_test.cpp $(SRCSTEST) $(CFLAGSTST2) $(LDFLAGS)

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

# CreateInterface Coverage
createInterfaceCoverage: CreateInterfaceCoverage
CreateInterfaceCoverage: ../utils/test/CreateInterface_test.cpp ../utils/src/CreateInterface.cpp
	  $(CXX) $(CFLAGS) $(CFLAGSTST) -fprofile-arcs -ftest-coverage -o ../utils/test/CreateInterface_test ../utils/test/CreateInterface_test.cpp $(SRCSTEST) $(CFLAGSTST2) $(LDFLAGS)
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

allCoverage: HandleFrames_coverage ReceiveFrames_coverage GenerateFrames_coverage CreateInterface_coverage

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
	$(CXX) $(CFLAGS) $(CFLAGSTST) -fprofile-arcs -ftest-coverage -o ../utils/test/CreateInterface_test ../utils/test/CreateInterface_test.cpp $(SRCSTEST) $(CFLAGSTST2) $(LDFLAGS)
	./../utils/test/CreateInterface_test
	mkdir -p coverage/out_CreateInterface
	gcov -o . ../utils/src/CreateInterface.cpp
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