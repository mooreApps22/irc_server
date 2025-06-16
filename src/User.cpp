#include "User.hpp"
#include <iostream>

User::User()
	:	_nickname(""),
		_registered(false),
		_partial_message("")
{
	std::cout << "User Constructor called." << std::endl;
}

User::~User()
{
	std::cout << "User Desstructor called." << std::endl;
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