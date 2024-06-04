
#include "../include/Logger.hpp"

std::shared_ptr<spdlog::logger> Logger::_consoleLogger = nullptr;

Logger::Logger()
{

}

Logger::Logger(std::string loggerName, std::string filePath)
{
    _fileLogger = spdlog::basic_logger_mt(loggerName, filePath);
    _loggers.emplace_back(loggerName);
}

std::shared_ptr<spdlog::logger> Logger::getConsoleLogger()
{
    if(_consoleLogger == nullptr)
    {
        auto _consoleSink = std::make_shared<spdlog::sinks::stdout_color_sink_mt>();
        _consoleLogger = std::make_shared<spdlog::logger>("console", _consoleSink);
        spdlog::register_logger(_consoleLogger);
    }
    
    return _consoleLogger;
}

const std::vector<std::string>& Logger::getLoggers() const
{
    return _loggers;
}
