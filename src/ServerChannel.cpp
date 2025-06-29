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

void	Server::promoteChannelMember(const std::string& channelName)
{
	_channels[channelName]->promoteMember(_client_fd);
}

void	Server::promoteChannelInvitee(const std::string& channelName)
{
	_channels[channelName]->promoteInvitee(_client_fd);
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

bool	Server::isTargetInChannel(const std::string& channelName, const std::string& nick)
{
	int		fd = Server::getUserFd(nick);
	return (_channels[channelName]->isMember(fd) || _channels[channelName]->isOperator(fd));
}

void	Server::sendMessageToChannel(const std::string& channelName, const std::string& message)
{
	std::vector<int> users = _channels[channelName]->getUsers();
	for (std::vector<int>::const_iterator it = users.begin(); it != users.end(); it++)
	{
		if (*it != _client_fd)
			sendToUser(message, *it);
	}
}

void	Server::removeUserFromChannel(const std::string& channelName, const std::string& nick)
{
	int		fd = Server::getUserFd(nick);

	_channels[channelName]->removeUser(fd);
}


//
bool	Server::isChannelTopicProtected(const std::string& channelName)
{
	return _channels[channelName]->isTopicRestricted();
}

bool	Server::isUserChannelOperator(const std::string& channelName)
{
	if (_channels.find(channelName) == _channels.end())
		return (false);
	return (_channels[channelName]->isOperator(_client_fd));
}

void	Server::setChannelInviteOnly(const std::string& channelName, bool status)
{
	_channels[channelName]->setInviteOnly(status);
}

void	Server::setChannelTopicRestricted(const std::string& channelName, bool status)
{
	_channels[channelName]->setTopicRestricted(status);
}

void	Server::addInviteeToChannel(const std::string& channelName, const std::string& nickname)
{
	int		userFd = getUserFd(nickname);
	User* user = _users[getUserFd(nickname)];
	_channels[channelName]->addInvitee(userFd, user);
}

void	Server::setNewTopic(const std::string& channelName, const std::string& topic)
{
	Logger::log(DEBUG, "is setNewTopic() being called?");
	_channels[channelName]->setTopic(topic);
}
