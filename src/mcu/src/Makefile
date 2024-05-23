CXX = g++
CXXFLAGS = -std=c++17 -Wall -Iinclude -pthread
TARGET = mcu
SRC = main.cpp HandleFrames.cpp InterfaceModule.cpp MCUModule.cpp ReceiveFrames.cpp

all: $(TARGET)

$(TARGET): $(SRC)
	$(CXX) $(CXXFLAGS) -o $(TARGET) $(SRC)

clean:
	rm -f $(TARGET)
