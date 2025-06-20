#pragma once

# include "Channel.hpp" 
# include <string>
# include <map>


typedef std::map<std::string, Channel*>::iterator	chanIt;
typedef std::map<int, User*>::iterator				usrsIt;

class IServerAPI
{
public:
	virtual ~IServerAPI() {}
	

	// Server API
	virtual void	send_reply(const std::string& message) = 0;
	virtual bool	isUserRegistered() = 0;
	virtual bool	isPasswordValid(const std::string& password) = 0;
	virtual void	setUserPassword(bool state) = 0;
	virtual bool	getUserPassword(void) = 0;
	virtual void	disconnectUser(void) = 0;
	virtual usrsIt	getUsersBegin(void) = 0;
	virtual usrsIt	getUsersEnd(void) = 0;
	virtual void	setUserNick(const std::string& nickneame) = 0;

/*
	virtual void addChannel(const std::string& name) = 0;
	virtual void removeChannel(const std::string& name) = 0;

	virtual Channel*	getChannel(const std::string& name) = 0;
	virtual chanIt		getChannelsBegin(void) = 0;
	virtual chanIt		getChannelsEnd(void) = 0;
	virtual User*		getUser(const int fd) = 0;

*/
};
