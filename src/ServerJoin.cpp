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

int	Server::getUserFd(const std::string& nick)
{
	std::map<int, User*>::iterator	it;

	for (it = _users.begin(); it != _users.end(); ++it)
	{
		if (it->second->getNickname() == nick)
		{
			Logger::log(INFO, "Got FD from User.");
			return (it->first);
		}
	}
	Logger::log(ERROR, "Failed to get FD from User.");
	return (-1);
}


Channel*	Server::getChannel(const std::string& channelName)
{
	std::map<std::string, Channel*>::iterator it = _channels.find(channelName);

	if (it != _channels.end())
	{
		Logger::log(INFO, "The JOIN command got the " + channelName);
		return (it->second);
	}
	Logger::log(ERROR, "The JOIN command failed to get the " + channelName);
	return (NULL);
}

///void	Server::addUserToChannel(std::string& channelName)
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
		//std::map<int, User*>					_operatorsFd;
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
