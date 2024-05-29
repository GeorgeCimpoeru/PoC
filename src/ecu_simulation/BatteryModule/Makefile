#Compiler flags
CFLAGS = -g -Wall -pthread -I./include -I./src
CFLAGSTST = -std=c++17 -I/include -fprofile-arcs -ftest-coverage
CFLAGSTST2 = -lgtest -lgtest_main -pthread
#Sources files
SRCS = src/main.cpp src/BatteryModule.cpp src/GenerateFrames.cpp src/HandleFrames.cpp src/InterfaceConfig.cpp src/ReceiveFrames.cpp
#Object files
OBJS = $(patsubst src/%.cpp, obj/%.o, $(SRCS))
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
$(OBJ_DIR)/main.o: src/main.cpp include/BatteryModule.h include/ReceiveFrames.h include/HandleFrames.h include/GenerateFrames.h include/InterfaceConfig.h
	$(CXX) $(CFLAGS) -c src/main.cpp -o $(OBJ_DIR)/main.o
$(OBJ_DIR)/BatteryModule.o: src/BatteryModule.cpp
	$(CXX) $(CFLAGS) -c src/BatteryModule.cpp -o $(OBJ_DIR)/BatteryModule.o
$(OBJ_DIR)/ReceiveFrames.o: src/ReceiveFrames.cpp
	$(CXX) $(CFLAGS) -c src/ReceiveFrames.cpp -o $(OBJ_DIR)/ReceiveFrames.o
$(OBJ_DIR)/HandleFrames.o: src/HandleFrames.cpp
	$(CXX) $(CFLAGS) -c src/HandleFrames.cpp -o $(OBJ_DIR)/HandleFrames.o
$(OBJ_DIR)/GenerateFrames.o: src/GenerateFrames.cpp
	$(CXX) $(CFLAGS) -c src/GenerateFrames.cpp -o $(OBJ_DIR)/GenerateFrames.o
$(OBJ_DIR)/InterfaceConfig.o: src/InterfaceConfig.cpp
	$(CXX) $(CFLAGS) -c src/InterfaceConfig.cpp -o $(OBJ_DIR)/InterfaceConfig.o

clean:
	rm -f $(FINAL) $(OBJS) ${FINAL_TEST}
	rm -rf $(OBJ_DIR)
#Delete inecessary file from gcov compiler
	find . -name '*.gcno' -delete
	find . -name '*.gcda' -delete
	find . -name '*.gcov' -delete

testing: $(FINAL_TEST)

$(FINAL_TEST): test/GenerateFramesTest.cpp src/GenerateFrames.cpp
	$(CXX) ${CFLAGSTST} -o ${FINAL_TEST} test/GenerateFramesTest.cpp src/GenerateFrames.cpp ${CFLAGSTST2}
	./test_binary
	gcov -o . src/GenerateFrames.cpp
	mv GenerateFrames.cpp.gcov test/
#Delete inecessary file from gcov compiler(Work just for GenerateFrame!!!!)
	find . -name "*.gcov" ! -name "GenerateFrames.cpp.gcov" -delete
	find . -name "*.gcda" ! -name "GenerateFrames.cpp.gcov" -delete
	find . -name "*.gcno" ! -name "GenerateFrames.cpp.gcov" -delete
