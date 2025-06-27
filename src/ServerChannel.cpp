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

bool	Server::doesChannelExist(const std::string& channelName)
{
	for (chanIt it = _channels.begin(); it != _channels.end(); it++)
	{
		if (it->first == channelName)		
			return (true);
	}
	return (false);
}

void	Server::addChannel(const std::string& channelName)
{
	_channels[channelName] = new Channel(channelName);
}

void	Server::addUserToChannel(const std::string& channelName)
{
	User*	user = _users[_client_fd];

	_channels[channelName]->addMember(_client_fd, user);
	user->addChannel(channelName);
}

bool	Server::isChannelFull(const std::string& channelName)
{
	return (_channels[channelName]->isFull());	
}

bool	Server::doesChannelHaveLimit(const std::string& channelName)
{
	return (_channels[channelName]->hasUserLimit());	
}

void	Server::setUserAsOperator(const std::string& channelName)
{
	User*	user = _users[_client_fd];
	_channels[channelName]->removeMember(_client_fd);
	_channels[channelName]->addOperator(_client_fd, user);
}

bool	Server::isChannelPasswordProtected(const std::string& channelName)
{
	return (_channels[channelName]->needsChannelKey());	
}

bool	Server::isChannelPasswordValid(const std::string& channelName, const std::string& key)
{
	return (_channels[channelName]->isKeyValid(key));
}

bool	Server::isChannelInviteOnly(const std::string& channelName)
{
	return (_channels[channelName]->isInviteOnly());
}

bool	Server::isUserInvited(const std::string& channelName)
{
	return (_channels[channelName]->isInvitee(_client_fd));
}

bool	Server::isChannelTopicSet(const std::string& channelName)
{
	return (_channels[channelName]->isTopicSet());
}

const std::string&	Server::getChannelTopic(const std::string& channelName)
{
	return (_channels[channelName]->getTopic());
}

const std::string	Server::getChannelUsersList(const std::string& channelName)
{
	return (_channels[channelName]->getUsersList(_client_fd));
}

bool	Server::isChannelUser(const std::string& channelName)
{
	return (_channels[channelName]->isMember(_client_fd) || _channels[channelName]->isOperator(_client_fd));
}

void	Server::sendMessageToChannel(const std::string& channelName, const std::string& message)
{
	for (std::map<int, User*>::const_iterator	it = _channels[channelName]->getMembers().begin(); it != _channels[channelName]->getMembers().end(); it++)
	{
		if (it->first != _client_fd)
			sendToUser(message, it->first);
	}
	for (std::map<int, User*>::const_iterator	it = _channels[channelName]->getOperators().begin(); it != _channels[channelName]->getOperators().end(); it++)
	{
		if (it->first != _client_fd)
			sendToUser(message, it->first);
	}
}

void	Server::removeUserFromChannel(const std::string& channelName, const std::string& nick)
{
	int		fd = Server::getUserFd(nick);

	if (_channels[channelName]->isMember(fd))
		_channels[channelName]->removeMember(fd);
	else if (_channels[channelName]->isOperator(fd))
		_channels[channelName]->removeOperator(fd);
}

bool	Server::isUserInChannel(const std::string& channelName, const std::string& nick)
{
	int		fd = Server::getUserFd(nick);

	return (_channels[channelName]->isMember(fd) || _channels[channelName]->isOperator(fd));
}

bool	Server::isUserChannelOperator(const std::string& channelName, const std::string& nick)
{
	int fd = getUserFd(nick);	

	if (_channels.find(channelName) == _channels.end())
		return (false);
	return (_channels[channelName]->isOperator(fd));
}
