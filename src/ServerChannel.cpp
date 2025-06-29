#include "Server.hpp"
#include "Logger.hpp"
#include "signal.hpp"
#include <iostream>
#include <netdb.h>
#include <cstring>
#include <errno.h>
#include <sys/socket.h>
#include <sys/epoll.h>
#include <unistd.h>
#include <string>
#include <arpa/inet.h>

/*	
	Server Method Specific to the JOIN command 
*/													

bool	Server::doesChannelExist(const std::string& channelId)
{
	for (chanIt it = _channels.begin(); it != _channels.end(); it++)
	{
		if (it->first == channelId)
			return (true);
	}
	return (false);
}

const std::string&	Server::getChannelName(const std::string& channelId)
{
	return (_channels[channelId]->getName());
}

void	Server::addChannel(const std::string& channelName)
{
	std::string channelId = Parser::toLower(channelName);
	_channels[channelId] = new Channel(channelName);
}

void	Server::removeChannel(const std::string& channelId)
{
	delete _channels[channelId];
	_channels.erase(channelId);
	if (!_channels[channelId])
		std::cout << "The channel was deleted!!!" << std::endl;
}

void	Server::addUserToChannel(const std::string& channelId)
{
	User*	user = _users[_client_fd];

	_channels[channelId]->addMember(_client_fd, user);
	user->addChannel(channelId);
}

bool	Server::isChannelFull(const std::string& channelId)
{
	return (_channels[channelId]->isFull());	
}

bool	Server::isChannelEmpty(const std::string& channelId)
{
	return (_channels[channelId]->isEmpty());	
}

bool	Server::doesChannelHaveLimit(const std::string& channelId)
{
	return (_channels[channelId]->hasUserLimit());	
}

void	Server::promoteChannelMember(const std::string& channelId)
{
	_channels[channelId]->promoteMember(_client_fd);
}

void	Server::promoteChannelMember(const std::string& channelId, const std::string& nickname)
{
	int targetFd = getUserFd(nickname);
	_channels[channelId]->promoteMember(targetFd);
}

void	Server::demoteChannelOperator(const std::string& channelId, const std::string& nickname)
{
	int targetFd = getUserFd(nickname);
	_channels[channelId]->demoteOperator(targetFd);
}

void	Server::promoteChannelInvitee(const std::string& channelId)
{
	_channels[channelId]->promoteInvitee(_client_fd);
}

bool	Server::isChannelPasswordProtected(const std::string& channelId)
{
	return (_channels[channelId]->needsChannelKey());	
}

bool	Server::isChannelPasswordValid(const std::string& channelId, const std::string& key)
{
	return (_channels[channelId]->isKeyValid(key));
}

bool	Server::isChannelInviteOnly(const std::string& channelId)
{
	return (_channels[channelId]->isInviteOnly());
}

bool	Server::isUserInvited(const std::string& channelId)
{
	return (_channels[channelId]->isInvitee(_client_fd));
}

bool	Server::isChannelTopicSet(const std::string& channelId)
{
	return (_channels[channelId]->isTopicSet());
}

const std::string&	Server::getChannelTopic(const std::string& channelId)
{
	return (_channels[channelId]->getTopic());
}

const std::string	Server::getChannelUsersList(const std::string& channelId)
{
	return (_channels[channelId]->getUsersList(_client_fd));
}

bool	Server::isChannelUser(const std::string& channelId)
{
	return (_channels[channelId]->isMember(_client_fd) || _channels[channelId]->isOperator(_client_fd));
}

bool	Server::isTargetInChannel(const std::string& channelId, const std::string& nick)
{
	int		fd = Server::getUserFd(nick);
	return (_channels[channelId]->isMember(fd) || _channels[channelId]->isOperator(fd));
}

void	Server::sendMessageToChannel(const std::string& channelId, const std::string& message)
{
	std::vector<int> users = _channels[channelId]->getUsers();
	for (std::vector<int>::const_iterator it = users.begin(); it != users.end(); it++)
	{
		if (*it != _client_fd)
			sendToUser(message, *it);
	}
}

void	Server::removeUserFromChannel(const std::string& channelId, const std::string& nick)
{
	int		fd = Server::getUserFd(nick);

	_channels[channelId]->removeUser(fd);
}


bool	Server::isChannelTopicProtected(const std::string& channelId)
{
	return _channels[channelId]->isTopicRestricted();
}

bool	Server::isUserChannelOperator(const std::string& channelId)
{
	if (_channels.find(channelId) == _channels.end())
		return (false);
	return (_channels[channelId]->isOperator(_client_fd));
}

void	Server::setChannelInviteOnly(const std::string& channelId, bool status)
{
	_channels[channelId]->setInviteOnly(status);
}

void	Server::setChannelTopicRestricted(const std::string& channelId, bool status)
{
	_channels[channelId]->setTopicRestricted(status);
}

void	Server::addInviteeToChannel(const std::string& channelId, const std::string& nickname)
{
	int		userFd = getUserFd(nickname);
	User* user = _users[getUserFd(nickname)];
	_channels[channelId]->addInvitee(userFd, user);
}

void	Server::setNewTopic(const std::string& channelId, const std::string& topic)
{
	Logger::log(DEBUG, "is setNewTopic() being called?");
	_channels[channelId]->setTopic(topic);
}

void	Server::setChannelHasLimit(const std::string& channelId, bool status)
{
	_channels[channelId]->setHasLimit(status);
}

void	Server::setChannelLimit(const std::string& channelId, int limit)
{
	_channels[channelId]->setUserLimit(limit);
}

int	Server::getChannelLimit(const std::string& channelId)
{
	return _channels[channelId]->getUserLimit();
}

bool	Server::isTargetChannelMember(const std::string& channelId, std::string& userNickname)
{
	return (_channels[channelId]->isMember(getUserFd(userNickname)));
}

bool	Server::isTargetChannelOperator(const std::string& channelId, std::string& userNickname)
{
	return (_channels[channelId]->isOperator(getUserFd(userNickname)));
}

void	Server::setChannelPassword(const std::string& channelName, const std::string&  key)
{
	_channels[channelName]->setKey(key);
}

const std::string&	Server::getChannelPassword(const std::string& channelName)
{
	return (_channels[channelName]->getKey());
}

void	Server::clearChannelPassword(const std::string& channelName)
{
	_channels[channelName]->clearKey();
}
