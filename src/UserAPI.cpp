#include "Server.hpp"
#include <string>

/*
	Setters
*/

void 	Server::setUserPasswordState(bool state)
{
	_users.at(_clientFd)->setPasswordStatus(state);
}

/*
	Getters
*/

const std::string&	Server::getUserNickname() const
{
	return _users.at(_clientFd)->getNickname();
}

bool 	Server::isUserRegistered() const
{
	return _users.at(_clientFd)->isRegistered();
}