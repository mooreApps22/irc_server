#pragma once
#include <fstream>

enum LogLevel
{
	INFO,
	WARN,
	ERROR,
	DEBUG
};

class Logger
{
	public:
		static void opener(const std::string& logFile);
		static void closer();
		static void	log(LogLevel level, const std::string& message);
		static void	log(LogLevel level, const std::string& message, int var);
		static void	log(LogLevel level, const std::string& message, std::string var);
	private:
		Logger();
		~Logger();
		std::ofstream			_fileStream;
		static Logger			_instance;
		static std::string		_levelToString(LogLevel level);
};
