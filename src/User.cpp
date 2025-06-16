#include "User.hpp"
#include <iostream>

User::User()
	:	_nickname(""),
		_registered(false)
{
	std::cout << "User Constructor called." << std::endl;
}

User::~User() { }

bool User::is_registered()
{
	return _registered;
}