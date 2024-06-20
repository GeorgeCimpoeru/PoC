#Compiler flags
CFLAGS = -g -Wall -pthread - -I./src -I/include -I/src
CFLAGSTST = -std=c++17 -I/include/Logger.h
LDFLAGS = -lspdlog
CFLAGSTST2 = -lgtest -lgtest_main -pthread -fprofile-arcs -ftest-coverage -lspdlog
#Sources files
SRCS = src/Logger.cpp

#Object files
OBJS = $(patsubst src/%.cpp, obj/%.o, $(SRCS))
OBJS:= $(OBJS) obj/Logger.o
#Directories
OBJ_DIR = obj
#TARGET
FINAL = main
FINAL_TEST = test_binary

clean:
	rm -f $(FINAL) $(OBJS) ${FINAL_TEST}
	rm -rf $(OBJ_DIR)
#Delete inecessary file from gcov compiler
	find . -name '*.gcno' -delete
	find . -name '*.gcda' -delete
	find . -name '*.gcov' -delete

testing: $(FINAL_TEST)

$(FINAL_TEST): test/LoggerTest.cpp src/Logger.cpp
	$(CXX) ${CFLAGSTST} -o ${FINAL_TEST} test/LoggerTest.cpp src/Logger.cpp ${CFLAGSTST2}
# 	gcov -o . src/Logger.cpp
# 	mv Logger.cpp.gcov coverage/out_Logger
# 	lcov --capture --directory . --output-file coverage.info
# 	lcov --remove coverage.info '/usr/*' --output-file coverage_filtered.info
# 	genhtml coverage_filtered.info --output-directory coverage/out_GenerateFrame
# #Delete inecessary file from gcov compiler(Work just for GenerateFrame!!!!)
# 	find . -name "*.gcov" ! -name "Logger.cpp.gcov" -delete
# 	find . -name "*.gcda" ! -name "Logger.cpp.gcov" -delete
# 	find . -name "*.gcno" ! -name "Logger.cpp.gcov" -delete