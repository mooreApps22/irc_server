#include "User.hpp"
#include "Logger.hpp"
#include <iostream>
#include <string>

User::User()
	:	_nickname(""),
		_registered(false),
		_partial_message("")
{
	Logger::log(INFO, "User Constructor called.");
}

User::~User()
{
	Logger::log(INFO, "User Desstructor called.");
}

void User::buffer(std::string& message)
{
	_partial_message.append(message);
}

std::string User::get_partial_message()
{
	std::string temp = _partial_message;
	_partial_message.clear();
	return temp;
}

bool User::is_registered()
{
	return _registered;
}
