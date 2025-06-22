#include "User.hpp"
#include "Logger.hpp"
#include <iostream>
#include <string>

User::User(const std::string& host)
	:	_nickname("*"),
		_username(""),
		_host(host),
		_registered(false),
		_password(false),
		_partial_message("")
{
	Logger::log(INFO, "User Constructor called.");
}

User::~User()
{
	Logger::log(INFO, "User Desstructor called.");
}

void User::setNickname(const std::string& nickname)
{
	_nickname = nickname;
}

const std::string&	User::getNickname() const
{
	return _nickname;
}

void User::setUsername(const std::string& username)
{
	_username = username;
}

void	User::setRegisteredStatus(const bool status)
{
	_registered = status;
}

bool	User::isRegistered() const
{
	return _registered;
}

void User::buffer(const std::string& message)
{
	_partial_message.append(message);
}

const std::string User::get_partial_message()
{
	std::string temp = _partial_message;
	_partial_message.clear();
	return temp;
}

bool User::getPasswordStatus() const
{
	return _password;
}

void User::setPasswordStatus(const bool state)
{
	_password = state;
}
