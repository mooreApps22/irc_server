#include "Server.hpp"
#include "User.hpp"
#include <string>

/*
	Setters
*/

void	Server::setUserNickname(const std::string& nickname)
{
	_users.at(_clientFd)->setNickname(nickname);
}

void	Server::setUserUsername(const std::string& username)
{
	_users.at(_clientFd)->setUsername(username);
}

void 	Server::setUserPasswordGivenStatus()
{
	_users.at(_clientFd)->setPasswordGivenStatus();
}

void 	Server::setUserNicknameGivenStatus()
{
	_users.at(_clientFd)->setNicknameGivenStatus();
}

void	Server::setUserRegisteredStatus()
{
	_users.at(_clientFd)->setRegisteredStatus();
}

// void	Server::setUserInvisibleMode(bool status)
// {
// 	_users.at(_clientFd)->setInvisibleMode(status);
// }

/*
	Getters
*/

const std::string&	Server::getUserNickname() const
{
	return (_users.at(_clientFd)->getNickname());
}

const std::string&	Server::getUserHost() const
{
	return (_users.at(_clientFd)->getHost());
}

const std::string	Server::getUserIdentifier() const
{
	return (_users.at(_clientFd)->getIdentifier());
}

bool 	Server::hasUserGivenPassword(void) const
{
	return (_users.at(_clientFd)->hasGivenPassword());
}

bool 	Server::hasUserGivenNickname(void) const
{
	return (_users.at(_clientFd)->hasGivenNickname());
}

bool 	Server::isUserRegistered() const
{
	return (_users.at(_clientFd)->isRegistered());
}

// bool	Server::isUserInvisibleMode() const
// {
// 	return (_users.at(_clientFd)->isInvisibleMode());
// }
