#Compiler flags
CFLAGS = -g -Wall -pthread -I./include -I./src
#Sources files
SRCS = src/main.cpp src/BatteryModule.cpp src/GenerateFrames.cpp src/HandleFrames.cpp src/InterfaceConfig.cpp src/ReceiveFrames.cpp
#Object files
OBJS = $(patsubst src/%.cpp, obj/%.o, $(SRCS))
#Directories
OBJ_DIR = obj
#TARGET
FINAL = main

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
	rm -f $(FINAL) $(OBJS)
	rm -rf $(OBJ_DIR)
