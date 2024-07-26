/**
 * @file LoggerTest.cpp
 * @author Iancu Daniel
 * @brief Unit tests for the Logger class.
 * @version 0.1
 * @date 2024-06-18
 * 
 * @copyright Copyright (c) 2024
 * 
 */

#include "gtest/gtest.h"
#include "src/utils/include/Logger.h"

class LoggerTest : public ::testing::Test
{
    
};

/**
 * @brief ACTION: Create a logger object
 * EXPECTED: Default logger shall be set and vector of loggers shall contain the default file logger.
 * 
 */
TEST_F(LoggerTest, LoggerConstructorTest)
{
    std::string loggerName = "logger";
    Logger loggerObject(loggerName, "logs/logs.log");

    EXPECT_NE(loggerObject.getFileLogger(), nullptr);

    auto loggers = loggerObject.getLoggers();

    EXPECT_EQ(loggers.size(), 1) 
        << "Logger object should contain only one private logger by default, but contains " 
        << loggers.size() << std::endl;

    EXPECT_EQ(loggers[0], loggerName);
}

/**
 * @brief ACTION: Request the default file logger.
 * EXPECTED: The default file logger is returned.
 * 
 */
TEST_F(LoggerTest, GetFileLoggerTest)
{
    std::string loggerName = "FileLogger";
    Logger fileLogger(loggerName, "logs/fileLogger.log");

    EXPECT_EQ(fileLogger.getFileLogger()->name(),loggerName);
}

/**
 * @brief ACTION: Check if only one console logger is used.
 * EXPECTED: Same console logger returned on multiple calls.
 * 
 */
TEST_F(LoggerTest, ConsoleLoggerTest)
{
    Logger fileLogger("FileLogger", "logs/fileLogger.log");

    EXPECT_EQ(Logger::getConsoleLogger(), fileLogger.getConsoleLogger());
    
    auto otherConsoleLogger = Logger::getConsoleLogger();
    EXPECT_EQ(otherConsoleLogger, Logger::getConsoleLogger());
}

/**
 * @brief ACTION: add a new file logger
 * EXPECTED: vector of loggers updated with new logger.
 * 
 */
TEST_F(LoggerTest, AddFileLogger)
{
    Logger logger("logger", "logs/logs.log");

    std::string newLoggerName = "newLogger";
    logger.addFileLogger(newLoggerName, "logs/logs.log");

    EXPECT_EQ(logger.getLoggers().back(), newLoggerName);
}

/**
 * @brief ACTION: Add multiple file loggers.
 * EXPECTED: Check if vector of loggers contains the added file loggers.
 * 
 */
TEST_F(LoggerTest, GetAllLoggersTest)
{
    uint8_t numberOfLoggers = 2;

    std::string loggerName1 = "logger1";
    Logger logger(loggerName1, "logs/logs.log");

    std::string loggerName2 = "logger2";
    logger.addFileLogger(loggerName2, "logs/logs.log");

    EXPECT_EQ(logger.getLoggers().size(), numberOfLoggers);

    EXPECT_NE(std::find(logger.getLoggers().begin(), logger.getLoggers().end(), loggerName1), logger.getLoggers().end());
    EXPECT_NE(std::find(logger.getLoggers().begin(), logger.getLoggers().end(), loggerName2), logger.getLoggers().end());
}

/**
 * @brief ACTION: Remove a logger.
 * EXPECTED: The size of the vector of loggers is decreased by 1 and the logger is removed from the vector.
 * 
 */
TEST_F(LoggerTest, RemoveLoggerTest)
{
    std::string loggerName = "logger";
    Logger logger(loggerName, "logs/logs.log");

    auto loggersSize = logger.getLoggers().size();
    logger.removeLogger(loggerName);

    EXPECT_EQ(logger.getLoggers().size(), loggersSize - 1);
    EXPECT_EQ(std::find(logger.getLoggers().begin(), logger.getLoggers().end(), loggerName), logger.getLoggers().end());
}

/**
 * @brief ACTION: Remove all loggers.
 * EXPECTED: Vector of loggers becomes empty.
 * 
 */
TEST_F(LoggerTest, RemoveAllLoggersTest)
{
    Logger logger("logger", "logs/logs.log");
    logger.removeAllLoggers();

    EXPECT_EQ(logger.getLoggers().empty(), true);
}

int main(int argc, char **argv)
{
    testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}