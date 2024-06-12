#Compiler flags
CFLAGS = -g -Wall -pthread -I./include -I./src -I../utils/include -I../utils/src
CFLAGSTST = -std=c++17 -I/include
CFLAGSTST2 = -lgtest -lgtest_main -pthread
LDFLAGS = -lspdlog
#Sources files
SRCS = src/main.cpp src/MCUModule.cpp src/GenerateFrames.cpp src/HandleFrames.cpp ../utils/src/CreateInterface.cpp src/ReceiveFrames.cpp
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
$(OBJ_DIR)/main.o: src/main.cpp include/MCUModule.h include/ReceiveFrames.h include/HandleFrames.h include/GenerateFrames.h ../utils/include/Logger.h ../utils/include/CreateInterface.h include/MCULogger.h
	$(CXX) $(CFLAGS) -c src/main.cpp -o $(OBJ_DIR)/main.o
$(OBJ_DIR)/MCUModule.o: src/MCUModule.cpp
	$(CXX) $(CFLAGS) -c src/MCUModule.cpp -o $(OBJ_DIR)/MCUModule.o
$(OBJ_DIR)/ReceiveFrames.o: src/ReceiveFrames.cpp
	$(CXX) $(CFLAGS) -c src/ReceiveFrames.cpp -o $(OBJ_DIR)/ReceiveFrames.o
$(OBJ_DIR)/HandleFrames.o: src/HandleFrames.cpp
	$(CXX) $(CFLAGS) -c src/HandleFrames.cpp -o $(OBJ_DIR)/HandleFrames.o
$(OBJ_DIR)/GenerateFrames.o: src/GenerateFrames.cpp
	$(CXX) $(CFLAGS) -c src/GenerateFrames.cpp -o $(OBJ_DIR)/GenerateFrames.o
$(OBJ_DIR)/CreateInterface.o: ../utils/src/CreateInterface.cpp
	$(CXX) $(CFLAGS) -c ../utils/src/CreateInterface.cpp -o $(OBJ_DIR)/CreateInterface.o
$(OBJ_DIR)/Logger.o: ../utils/src/Logger.cpp
	$(CXX) $(CFLAGS) -c ../utils/src/Logger.cpp -o $(OBJ_DIR)/Logger.o

clean:
	rm -f $(FINAL) $(OBJS) ${FINAL_TEST}
	rm -rf $(OBJ_DIR)
#Delete inecessary file from gcov compiler
	find . -name '*.gcno' -delete
	find . -name '*.gcda' -delete
	find . -name '*.gcov' -delete