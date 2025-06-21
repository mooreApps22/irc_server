#include "Server.hpp"
#include "Channel.hpp"
#include "IrcErrorCodes.hpp"
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

User*	Server::getUser(const int fd)
{
	std::map<const int, User*>::iterator it = _users.find(fd);
	
	if (it != _users.end())
	{
		Logger::log(ERROR, "Got User in getUser().");
		return (it->second);
	}
	return (NULL);
}

Channel*	Server::getChannel(const std::string& channel_name)
{
	std::map<std::string, Channel*>::iterator it = _channels.find(channel_name);

	if (it != _channels.end())
	{
		Logger::log(ERROR, "Got channel in getChannel().");
		return (it->second);
	}
	return (NULL);
}

void	Server::addUserToChannel(User* user, const std::string& channel_name)
{
	if (user && !channel_name.empty())	
	{
		Channel* channel = getChannel(channel_name);

		if (channel)
		{
			channel->addMember(user, user->getNickname());
			Logger::log(INFO, "User " + user->getNickname());
		}
		else
			Logger::log(INFO, "Channel " + channel_name + " does no exist.");
	}
	else
		Logger::log(ERROR, "Invalid user or channel name.");
}

int	Server::getFdFromNickname(const std::string& nickname)
{
	std::map<int, User*>::iterator it;

	for (it = _users.begin(); it != _users.end(); ++it)
	{
		if (it->second->getNickname() == nickname)
		{
			Logger::log(ERROR, "Got FD in getFdFromNickname");
			return (it->first);
		}
	}
	return (-1);
}
