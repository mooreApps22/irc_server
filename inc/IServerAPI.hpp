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
	// Server actions related
	virtual void	send_reply(const std::string& message) = 0;
	virtual bool	isPasswordValid(const std::string& password) = 0;
	virtual void	disconnectUser(void) = 0;


	// User actions related
	virtual void				setUserNick(const std::string& nickname) = 0;
	virtual const std::string&	getUserNick() = 0;
	virtual void				setUserUsername(const std::string& username) = 0;
	virtual const std::string	getUserIdentifier() = 0;
	virtual void				setUserRegisteredStatus(bool status) = 0;
	virtual bool				isUserRegistered() = 0;
	virtual void				setUserPasswordState(bool state) = 0;
	virtual bool				getUserPasswordState(void) = 0;
	virtual usrsIt				getUsersBegin(void) = 0;
	virtual usrsIt				getUsersEnd(void) = 0;
	

/*
	// Channel actions relatedc
	virtual void addChannel(const std::string& name) = 0;
	virtual void removeChannel(const std::string& name) = 0;

	virtual Channel*	getChannel(const std::string& name) = 0;
	virtual chanIt		getChannelsBegin(void) = 0;
	virtual chanIt		getChannelsEnd(void) = 0;
	virtual User*		getUser(const int fd) = 0;

*/
};
