#include "User.hpp"
#include "Logger.hpp"
#include <string>

User::User(const std::string& host)
	:	_nickname("*"),
		_username(""),
 		_host(host),
		_registrationState(CONNECTED),
		// _invisibleMode(false),
		_partialMessage("")
{
	// Logger::log(INFO, "User Constructor called.");
}

User::~User()
{
	// Logger::log(INFO, "User Desstructor called.");
}

/*
	Setters
*/

void User::setNickname(const std::string& nickname)
{
	_nickname = nickname;
}

void User::setUsername(const std::string& username)
{
	_username = username;
}

void User::setPasswordGivenStatus()
{
	_registrationState = PASS_GIVEN;
}

void User::setNicknameGivenStatus()
{
	_registrationState = NICK_GIVEN;
}

void	User::setRegisteredStatus()
{
	_registrationState = REGISTERED;
}

void	User::setInvisibleMode(const bool status)
{
	_invisibleMode = status;
}

/*
	Getters
*/

const std::string&	User::getNickname() const
{
	return (_nickname);
}

const std::string&	User::getHost() const
{
	return (_host);
}

const std::string	User::getIdentifier() const
{
	std::string identifier = _nickname;
	identifier += "!";
	identifier += _username;
	identifier += "@";
	identifier += _host;
	return (identifier);
}

bool User::hasGivenPassword() const
{
	return (_registrationState >= PASS_GIVEN);
}

bool User::hasGivenNickname() const
{
	return (_registrationState >= NICK_GIVEN);
}

bool	User::isRegistered() const
{
	return (_registrationState == REGISTERED);
}

// bool	User::isInvisibleMode() const
// {
// 	return (_invisibleMode);
// }

User::channelsIt	User::getChannelsBegin() const
{
	return (_channels.begin());
}

User::channelsIt	User::getChannelsEnd() const
{
	return (_channels.end());
}

/*
	Messages
*/

void User::buffer(const std::string& message)
{
	_partialMessage.append(message);
}

const std::string User::getPartialMessage()
{
	std::string temp = _partialMessage;
	_partialMessage.clear();
	return (temp);
}

/*
	Channels
*/

void	User::addChannel(const std::string& channelId)
{
	_channels.insert(channelId);
}

void	User::removeChannel(const std::string& channelId)
{
	_channels.erase(channelId);
}
