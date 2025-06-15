#include "User.hpp"
#include <iostream>

User::User()
	: _nickname(""),
		_password(false)
{
	std::cout << "User Constructor called." << std::endl;
}