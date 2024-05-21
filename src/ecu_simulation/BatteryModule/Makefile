#Compiler flags
CFLAGS = -g -Wall -pthread -I./include -I./src
#Sources files
SRCS = src/main.cpp src/BatteryModule.cpp src/GenerateFrames.cpp src/HandleFrames.cpp src/InterfaceConfig.cpp src/ReceiveFrames.cpp
#Object files
OBJS = $(SRCS:.cpp=.o)
#OBJS = $(SRCS:src/%.cpp=obj/%.o)
#TARGET
FINAL = main

.PHONY: all clean

all: $(OBJS)
	$(CXX) $(CFLAGS) -o $(FINAL) $(OBJS) ${LDFLAGS}
main.o: src/main.cpp include/BatteryModule.h include/ReceiveFrames.h include/HandleFrames.h include/GenerateFrames.h include/InterfaceConfig.h
	$(CXX) $(CFLAGS) -c src/main.cpp
BatteryModule.o: src/BatteryModule.cpp
	$(CXX) $(CFLAGS) -c BatteryModule.cpp
ReciveFrame.o: src/ReceiveFrames.cpp
	$(CXX) $(CFLAGS) -c ReceiveFrames.cpp
HandleFrame.o: src/HandleFrames.cpp
	$(CXX) $(CFLAGS) -c HandleFrames.cpp
GenerateFrames.o: src/GenerateFrames.cpp
	$(CXX) $(CFLAGS) -c GenerateFrames.cpp
InterfaceConfig.o: src/InterfaceConfig.cpp
	$(CXX) $(CFLAGS) -c InterfaceConfig.cpp

clean:
	rm -f $(FINAL) $(OBJS)
