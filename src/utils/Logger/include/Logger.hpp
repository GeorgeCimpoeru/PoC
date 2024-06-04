#pragma once

#define SPDLOG_COMPILED_LIB
#define SPDLOG_ACTIVE_LEVEL SPDLOG_LEVEL_DEBUG

#include <spdlog/spdlog.h>
#include <spdlog/sinks/basic_file_sink.h>
#include <spdlog/sinks/stdout_color_sinks.h>

class Logger
{
    std::shared_ptr<spdlog::logger> _fileLogger;
    static std::shared_ptr<spdlog::logger> _consoleLogger;

    std::vector<std::string> _loggers{"console"};
public:
    Logger();
    Logger(std::string loggerName, std::string filePath);
    
    static std::shared_ptr<spdlog::logger> getConsoleLogger();
    const std::vector<std::string>& getLoggers() const;
};

#define LOG_TRACE(logger, ...) SPDLOG_LOGGER_TRACE(logger, __VA_ARGS__)
#define LOG_DEBUG(logger, ...) SPDLOG_LOGGER_DEBUG(logger, __VA_ARGS__)
#define LOG_INFO(logger, ...) SPDLOG_LOGGER_INFO(logger, __VA_ARGS__)
#define LOG_WARN(logger, ...) SPDLOG_LOGGER_WARN(logger, __VA_ARGS__)
#define LOG_ERROR(logger, ...) SPDLOG_LOGGER_ERROR(logger, __VA_ARGS__)
#define LOG_CRITICAL(logger, ...) SPDLOG_LOGGER_CRITICAL(logger, __VA_ARGS__)