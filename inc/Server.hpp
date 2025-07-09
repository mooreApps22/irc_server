#pragma once
#include "IServerAPI.hpp"
#include "User.hpp"
#include "Channel.hpp"
#include "Parser.hpp"
#include "CommandHandler.hpp"
#include <map>
#include <string>


class Server: public IServerAPI
{
	typedef std::map<int, User*> 			serverClients;
	typedef serverClients::const_iterator	clientsIt;
	typedef	std::map<std::string, Channel*>	serverChannels;
	typedef serverChannels::const_iterator	channelsIt;

	private:
		const std::string	_port;
		const std::string	_password;
		int					_serverFd;
		int					_clientFd;
		int					_epollFd;
		serverClients		_users;
		serverChannels		_channels;
		Parser				_parser;
		CommandHandler		_ch;

		void	setup();

		// Polling
		int		registerFd(int fd) const;
		int		unregisterFd(int fd) const;

		// Clients
		void	acceptConnection();
		void	addNewClient(const std::string& host);

		// Receiving data
		std::string	getMessage() const;
		std::string peek() const;
		std::string receive(int length) const;

		// Sending data
		void	sendToClient(const std::string& message, int userFd) const;
		
		// Users and channels
		int		getUserFd(std::string nickname) const;
		void	removeUserFromChannel(const std::string& channelId, int userFd);
		void	removeUserFromServer(int clientFd = -1);
		void	removeChannelFromServer(const std::string& channelId);

	public:
		Server(const std::string& port, const std::string& password);
		~Server();
		void	run(void);
		void	cleanUp();

		/*
			Server API
		*/
		void	sendReply(const std::string& reply) const;
		void	sendToAll(const std::string& message) const;
		void	sendToTarget(const std::string& message, std::string& nickname) const;
		void	sendMessageToChannel(const std::string& message, const std::string& channelId) const;
		void	sendMessageToChannelsWhereUser(const std::string& message) const;

		bool 	isPasswordValid(const std::string& password) const;
		bool	doesNicknameExist(const std::string& nickname) const;
		bool	doesChannelExist(const std::string& channelName) const;
		void	addChannel(const std::string& channelName);
		void	disconnectClientFromServer(int clientFd = -1);
		void	disconnectAllClientsFromServer();

		/*
			User API
		*/
		// Setters
		void	setUserNickname(const std::string& nickname);
		void	setUserUsername(const std::string& username);
		void 	setUserPasswordGivenStatus();
		void 	setUserNicknameGivenStatus();
		void	setUserRegisteredStatus();
		// void	setUserInvisibleMode(bool status);
		
		// Getters
		const std::string&		getUserNickname() const;
		const std::string&		getUserHost() const;
		const std::string		getUserIdentifier() const;
		bool 					hasUserGivenPassword(void) const;
		bool 					hasUserGivenNickname(void) const;
		bool 					isUserRegistered() const;
		// bool					isUserInvisibleMode() const;


		/*
			Channel API
		*/
		// Getters
		const std::string&	getChannelName(const std::string& channelId) const;
		const std::string	getChannelUsersList(const std::string& channelId) const;
		const std::string&	getChannelKey(const std::string& channelId) const;
		size_t				getChannelLimit(const std::string& channelId) const;
		const std::string&	getChannelTopic(const std::string& channelId) const;
		

		bool				isChannelInviteMode(const std::string& channelId) const;
		bool				isChannelKeyMode(const std::string& channelId) const;
		bool				isChannelLimitMode(const std::string& channelId) const;
		bool				isChannelTopicMode(const std::string& channelId) const;

		bool				isChannelKeyValid(const std::string& channelId, const std::string& key) const;
		bool				isChannelFull(const std::string& channelId) const;
		bool				isChannelTopicSet(const std::string& channelId) const;


		// Setters
		void				setChannelKey(const std::string& channelId, const std::string&  key);
		void				setChannelLimit(const std::string& channelId, size_t limit);
		void				setChannelTopic(const std::string& channelId, const std::string& topic);
		void				setChannelInviteMode(const std::string& channelId, bool status);
		void 				setChannelTopicMode(const std::string& channelId, bool status);
		void				clearChannelKey(const std::string& channelId);


		// Membership
		// Users
		bool				isChannelUser(const std::string& channelId, std::string nickname = "") const;
		void				removeUserFromChannel(const std::string& channelId, std::string nickname = "");

		// Members
		void				addMemberToChannel(const std::string& channelId);
		bool				isChannelMember(const std::string& channelId, std::string nickname = "") const;

		// Operators
		void				promoteChannelMember(const std::string& channelId, std::string nickname = "");
		bool				isChannelOperator(const std::string& channelId, std::string nickname = "") const;
		void				demoteChannelOperator(const std::string& channelId, const std::string& nickname);

		// Invitatees
		void				addInviteeToChannel(const std::string& channelId, const std::string& nickname);
		bool				isChannelInvitee(const std::string& channelId) const;
		void				promoteChannelInvitee(const std::string& channelId);
};
