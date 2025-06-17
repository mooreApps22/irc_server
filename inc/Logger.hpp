#pragma once
# include <iostream>
# include <string>
#include <fstream>
#include <sstream>

enum LogLevel
{
	INFO,
	WARN,
	ERROR
};

class Logger
{
    public:
		static void opener(const std::string& logFile);
		static void closer();
		static void	log(LogLevel level, const std::string& message);
	private:
        Logger();
        ~Logger();
		std::ofstream			_fileStream;
		static Logger			_instance;
		static std::string		_levelToString(LogLevel level);
};
