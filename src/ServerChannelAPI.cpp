#include "Server.hpp"
#include "User.hpp"
#include "Channel.hpp"
#include <vector>
#include <string>


/*
	Getters
*/

const std::string&	Server::getChannelName(const std::string& channelId) const
{
	return (_channels.at(channelId)->getName());
}

const std::string	Server::getChannelUsersList(const std::string& channelId) const
{
	return (_channels.at(channelId)->getMembersNicknamesList(_clientFd));
}

const std::string&	Server::getChannelKey(const std::string& channelId) const
{
	return (_channels.at(channelId)->getKey());
}

size_t	Server::getChannelLimit(const std::string& channelId) const
{
	return _channels.at(channelId)->getUserLimit();
}

const std::string&	Server::getChannelTopic(const std::string& channelId) const
{
	return (_channels.at(channelId)->getTopic());
}

bool	Server::isChannelInviteMode(const std::string& channelId) const
{
	return (_channels.at(channelId)->isInviteMode());
}

bool	Server::isChannelLimitMode(const std::string& channelId) const
{
	return (_channels.at(channelId)->isLimitMode());	
}

bool	Server::isChannelKeyMode(const std::string& channelId) const
{
	return (_channels.at(channelId)->isKeyMode());	
}

bool	Server::isChannelTopicMode(const std::string& channelId) const
{
	return _channels.at(channelId)->isTopicMode();
}

bool	Server::isChannelKeyValid(const std::string& channelId, const std::string& key) const
{
	return (_channels.at(channelId)->isKeyValid(key));
}

bool	Server::isChannelFull(const std::string& channelId) const
{
	return (_channels.at(channelId)->isMembersFull());	
}

bool	Server::isChannelTopicSet(const std::string& channelId) const
{
	return (_channels.at(channelId)->isTopicSet());
}

/*
	Setters
*/

void	Server::setChannelKey(const std::string& channelName, const std::string&  key)
{
	_channels.at(channelName)->setKey(key);
}

void	Server::setChannelLimit(const std::string& channelId, size_t limit)
{
	_channels.at(channelId)->setUserLimit(limit);
}

void	Server::setChannelTopic(const std::string& channelId, const std::string& topic)
{
	// Logger::log(DEBUG, "is setChannelTopic() being called?");
	_channels.at(channelId)->setTopic(topic);
}

void	Server::setChannelInviteMode(const std::string& channelId, bool status)
{
	_channels.at(channelId)->setInviteMode(status);
}

void	Server::setChannelTopicMode(const std::string& channelId, bool status)
{
	_channels.at(channelId)->setTopicMode(status);
}

void	Server::clearChannelKey(const std::string& channelId)
{
	_channels.at(channelId)->clearKey();
}

/*
	Membership
*/
// Users
bool	Server::isChannelUser(const std::string& channelId, std::string nickname) const
{
	return (isChannelMember(channelId, nickname) || isChannelOperator(channelId, nickname));
}

void	Server::removeUserFromChannel(const std::string& channelId, std::string nickname)
{
	int userFd = nickname.empty() ? _clientFd : getUserFd(nickname);
	
	removeUserFromChannel(channelId, userFd);
}


// Members
void	Server::addMemberToChannel(const std::string& channelId)
{
	_channels.at(channelId)->addMember(_clientFd, _users.at(_clientFd));
}

bool	Server::isChannelMember(const std::string& channelId, std::string nickname) const
{
	int userFd = nickname.empty() ? _clientFd : getUserFd(nickname);
	return (_channels.at(channelId)->isUserMember(userFd));
}

// Operators

void	Server::promoteChannelMember(const std::string& channelId, std::string nickname)
{
	int userFd = nickname.empty() ? _clientFd : getUserFd(nickname);

	_channels.at(channelId)->promoteMember(userFd);
}

bool	Server::isChannelOperator(const std::string& channelId, std::string nickname) const
{
	int userFd = nickname.empty() ? _clientFd : getUserFd(nickname);

	if (_channels.find(channelId) == _channels.end())
		return (false);
	return (_channels.at(channelId)->isUserOperator(userFd));
}

void	Server::demoteChannelOperator(const std::string& channelId, const std::string& nickname)
{
	int targetFd = getUserFd(nickname);
	_channels.at(channelId)->demoteOperator(targetFd);
}

// Invitees

void	Server::addInviteeToChannel(const std::string& channelId, const std::string& nickname)
{
	int		userFd = getUserFd(nickname);
	User* user = _users.at(userFd);
	_channels.at(channelId)->addInvitee(userFd, user);
}

bool	Server::isChannelInvitee(const std::string& channelId) const
{
	return (_channels.at(channelId)->isChannelInvitee(_clientFd));
}

void	Server::promoteChannelInvitee(const std::string& channelId)
{
	_channels.at(channelId)->promoteInvitee(_clientFd);
}
