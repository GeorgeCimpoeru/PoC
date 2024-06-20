/**
 * @file Logger.cpp
 * @author Iancu Daniel
 * @brief 
 * @version 0.1
 * @date 2024-06-05
 * 
 * @copyright Copyright (c) 2024
 * 
 */

#include "../include/Logger.h"

/* first initialisation of the consoleLogger shall be to nullptr. */
std::shared_ptr<spdlog::logger> Logger::_consoleLogger = nullptr;

Logger::Logger()
{
    spdlog::flush_on(spdlog::level::trace);
}

Logger::Logger(std::string loggerName, std::string filePath)
{
    spdlog::flush_on(spdlog::level::trace);

    _fileLogger = spdlog::basic_logger_mt(loggerName, filePath);

    /**
     *  For future use, not needed now
     * 
     * spdlog::init_thread_pool(8192, 1); // queue with 8k items and 1 backing thread.
     * _fileLogger = spdlog::create_async<spdlog::sinks::basic_file_sink_mt>(loggerName, filePath);
     */
    _loggers.emplace_back(loggerName);
}

std::shared_ptr<spdlog::logger> Logger::getConsoleLogger()
{
    if(_consoleLogger == nullptr)
    {
        /* Create multithreaded console sink with colors. */
        auto _consoleSink = std::make_shared<spdlog::sinks::stdout_color_sink_mt>();
        _consoleLogger = std::make_shared<spdlog::logger>("console", _consoleSink);
        spdlog::register_logger(_consoleLogger);
    }
    
    return _consoleLogger;
}

std::shared_ptr<spdlog::logger> Logger::getFileLogger() const
{
    return _fileLogger;
}

std::shared_ptr<spdlog::logger> Logger::addFileLogger(std::string loggerName, std::string filePath)
{
    _loggers.emplace_back(loggerName);
    return spdlog::basic_logger_mt(loggerName, filePath);
}

void Logger::removeLogger(std::string loggerName)
{
    spdlog::drop(loggerName);
    _loggers.erase(std::remove(_loggers.begin(), _loggers.end(), loggerName), _loggers.end());
}

void Logger::removeAllLoggers()
{
    for(auto& logger : _loggers)
    {
        removeLogger(logger);
    }
}

const std::vector<std::string>& Logger::getLoggers() const
{
    return _loggers;
}

Logger::~Logger()
{
    removeAllLoggers();
}
