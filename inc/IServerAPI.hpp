#pragma once
#include "User.hpp"
# include <string>


class IServerAPI
{
public:
	virtual ~IServerAPI() {}
	
	/*
		Server API
	*/
	virtual void	sendReply(const std::string& message) const = 0;
	virtual void	sendToAll(const std::string& message) const = 0;
	virtual void	sendToTarget(const std::string& message, std::string& nickname) const = 0;
	virtual	void	sendMessageToChannel(const std::string& message, const std::string& channelId) const = 0;
	virtual	void	sendMessageToChannelsWhereUser(const std::string& message) const = 0;

	virtual bool	isPasswordValid(const std::string& password) const = 0;
	virtual bool	doesNicknameExist(const std::string& nickname) const = 0;
	virtual bool	doesChannelExist(const std::string& channelName) const = 0;
	
	virtual void	addChannel(const std::string& channelName) = 0;
	virtual	void	disconnectClientFromServer(int clientFd = -1) = 0;
	virtual void	disconnectAllClientsFromServer() = 0;


	/*
		User API
	*/
	// Setters
	virtual void	setUserNickname(const std::string& nickname) = 0;
	virtual void	setUserUsername(const std::string& username) = 0;
	virtual void	setUserPasswordGivenStatus() = 0;
	virtual void	setUserNicknameGivenStatus() = 0;
	virtual void	setUserRegisteredStatus() = 0;
	// virtual void	setUserInvisibleMode(bool state) = 0;

	// Getters
	virtual const std::string&		getUserNickname() const = 0;
	virtual const std::string&		getUserHost() const = 0;
	virtual const std::string		getUserIdentifier() const = 0;
	virtual bool					hasUserGivenPassword(void) const = 0;
	virtual bool					hasUserGivenNickname(void) const = 0;
	virtual bool					isUserRegistered() const = 0;
	// virtual bool					isUserInvisibleMode() const = 0;


	/*
		Channel API
	*/
	// Getters
	virtual const std::string&	getChannelName(const std::string& channelId) const = 0;
	virtual const std::string	getChannelUsersList(const std::string& channelId) const = 0;
	virtual const std::string&	getChannelKey(const std::string& channelId) const = 0;
	virtual size_t				getChannelLimit(const std::string& channelId) const = 0;
	virtual const std::string&	getChannelTopic(const std::string& channelId) const = 0;
	
	virtual bool				isChannelInviteMode(const std::string& channelId) const = 0;
	virtual bool				isChannelKeyMode(const std::string& channelId) const = 0;
	virtual bool				isChannelLimitMode(const std::string& channelId) const = 0;
	virtual bool				isChannelTopicMode(const std::string& channelId) const = 0;

	virtual bool				isChannelKeyValid(const std::string& channelId, const std::string& key) const = 0;
	virtual bool				isChannelFull(const std::string& channelId) const = 0;
	virtual bool				isChannelTopicSet(const std::string& channelId) const = 0;	

	// Setters
	virtual void				setChannelKey(const std::string& channelId, const std::string&  key) = 0; //
	virtual void 				setChannelLimit(const std::string& channelId, size_t limit) = 0;
	virtual void				setChannelTopic(const std::string& channelId, const std::string& topic) = 0;
	virtual void 				setChannelInviteMode(const std::string& channelId, bool status) = 0;
	virtual void 				setChannelTopicMode(const std::string& channelId, bool status) = 0;
	virtual void				clearChannelKey(const std::string& channelId) = 0;


	// Membership
	// Users
	virtual bool				isChannelUser(const std::string& channelId, std::string nickname = "") const = 0;
	virtual void				removeUserFromChannel(const std::string& channelId, std::string nickname = "") = 0;

	// Members
	virtual void				addMemberToChannel(const std::string& channelId) = 0;
	virtual bool				isChannelMember(const std::string& channelId, std::string nickname = "") const = 0;

	// Operators
	virtual void				promoteChannelMember(const std::string& channelId, std::string nickname = "") = 0;
	virtual bool				isChannelOperator(const std::string& channelId, std::string nickname = "") const = 0;
	virtual void				demoteChannelOperator(const std::string& channelId, const std::string& nickname) = 0;


	// Invitees
	virtual void				addInviteeToChannel(const std::string& channelId, const std::string& nickname) = 0;
	virtual bool				isChannelInvitee(const std::string& channelId) const = 0;
	virtual void				promoteChannelInvitee(const std::string& channelId) = 0;
};
