#include "Logger.hpp"
#include <iostream>
#include <string>
#include <fstream>
#include <sstream>

Logger Logger::_instance;

Logger::Logger()
{
	Logger::opener("log.txt");
}

Logger::~Logger()
{
	Logger::closer();
}

std::string Logger::_levelToString(LogLevel level)
{
	switch (level)
	{
		case INFO:
			return "INFO";
		case WARN:
			return "WARN";
		case ERROR:
			return "ERROR";
		case DEBUG:
			return "DEBUG";
		default:
			return "UNKNOWN";
	}
}

void	Logger::opener(const std::string& logFile)
{
	_instance._fileStream.open(logFile.c_str());
}

void	Logger::closer()
{
	_instance._fileStream.close();
}

void	Logger::log(LogLevel level, const std::string& message)
{
	std::ostream& stream = _instance._fileStream ;
	stream << "[" << _levelToString(level) << "] " << message << std::endl;

}

void	Logger::log(LogLevel level, const std::string& message, int var)
{
	std::ostream& stream = _instance._fileStream ;
	stream << "[" << _levelToString(level) << "] " << message << " (" << var << ")" << std::endl;

}

void	Logger::log(LogLevel level, const std::string& message, std::string var)
{
	std::ostream& stream = _instance._fileStream ;
	stream << "[" << _levelToString(level) << "] " << message << " (" << var << ")" << std::endl;

}
