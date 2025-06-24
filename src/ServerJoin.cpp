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


