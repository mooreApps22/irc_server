#pragma once

# include "Channel.hpp" 
# include <string>
# include <map>

class IServerAPI
{
public:
	virtual ~IServerAPI() {}
	typedef std::map<std::string, Channel*>::iterator	chanIt;
	typedef std::map<int, Channel*>::iterator			userIt;

	// Server
	virtual void send_reply(const std::string& message) = 0;

/*
	virtual void addChannel(const std::string& name) = 0;
	virtual void removeChannel(const std::string& name) = 0;

	virtual Channel*	getChannel(const std::string& name) = 0;
	virtual chanIt		getChannels(void) = 0;
	virtual User*		getUser(const int fd) = 0;
	virtual userIt		getUsers(void) = 0;

*/
};
