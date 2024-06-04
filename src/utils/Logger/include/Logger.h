#pragma once

#define SPDLOG_COMPILED_LIB
#define SPDLOG_ACTIVE_LEVEL SPDLOG_LEVEL_DEBUG

#include <spdlog/spdlog.h>
#include <spdlog/sinks/basic_file_sink.h>
#include <spdlog/sinks/stdout_color_sinks.h>
#include <spdlog/async.h>

class Logger
{
private:
    /* File logger specific to a logger object. */
    std::shared_ptr<spdlog::logger> _fileLogger = nullptr;
    /* Global console logger, can be used without creating a Logger object. */
    static std::shared_ptr<spdlog::logger> _consoleLogger;
    /* Keep track of all loggers specific to a Logger object. */
    std::vector<std::string> _loggers;
public:
    /**
     * @brief Construct a new Logger object without adding a file logger to it. Only console logger by default.
     * 
     */
    Logger();
    /**
     * @brief Construct a new Logger object and add a file logger to it. The file logger is specific to this logger,
     *  but can be used in another module to log to it.
     *  
     * @param[i] loggerName 
     * @param[i] filePath 
     */
    Logger(std::string loggerName, std::string filePath);

    void setFileLogger(std::string& loggerName, std::string& filePath);

    /**
     * @brief Create only one static consoleLogger for the Logger class. 
     * This can be used by all modules without creating a Logger object.
     * 
     * @return std::shared_ptr<spdlog::logger> 
     */
    static std::shared_ptr<spdlog::logger> getConsoleLogger();

    std::shared_ptr<spdlog::logger> getFileLogger() const;
    /**
     * @brief Remove a specific logger knowing its name.
     * 
     * @param[i] loggerName 
     */
    void removeLogger(std::string loggerName);
    /**
     * @brief Remove all loggers specific to a Logger object.
     * Console logger is not removed.
     * 
     */
    void removeAllLoggers();
    /**
     * @brief Get the Loggers specific to this Logger (without global loggers like consoleLogger).
     * 
     * @return const std::vector<std::string>& 
     */
    const std::vector<std::string>& getLoggers() const;

    ~Logger();
};

/* LOGGING CAN BE TURNED OFF AT COMPILE TIME, BUT USAGE OF MACROS IS NEEDED IN ORDER TO WORK*/
#define LOG_TRACE(logger, ...) SPDLOG_LOGGER_TRACE(logger, __VA_ARGS__)
#define LOG_DEBUG(logger, ...) SPDLOG_LOGGER_DEBUG(logger, __VA_ARGS__)
#define LOG_INFO(logger, ...) SPDLOG_LOGGER_INFO(logger, __VA_ARGS__)
#define LOG_WARN(logger, ...) SPDLOG_LOGGER_WARN(logger, __VA_ARGS__)
#define LOG_ERROR(logger, ...) SPDLOG_LOGGER_ERROR(logger, __VA_ARGS__)
#define LOG_CRITICAL(logger, ...) SPDLOG_LOGGER_CRITICAL(logger, __VA_ARGS__)