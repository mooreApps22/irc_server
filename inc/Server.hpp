#pragma once
#include "IServerAPI.hpp"
#include "User.hpp"
#include "Parser.hpp"
#include "CommandHandler.hpp"
# include <string>
#include <map>


class Server: public IServerAPI
{
    private:
		const std::string				_port;
		const std::string				_password;
		int								_server_fd;
		int								_client_fd;
		int								_epoll_fd;
		std::map<int, User*>			_users;
		std::map<std::string, Channel*>	_channels;
		Parser							_parser;
		CommandHandler					_ch;

		void		setup();
		int			register_fd(int fd);
		void		accept_connection();
		void		add_new_user(const std::string& host);
		std::string	get_message();
		std::string peek();
		std::string receive(int length);
		void		sendToUser(const std::string& message, int user_fd);
		int			getUserFd(std::string nickname);
		void		clean_up();

    public:
        Server(const std::string& port, const std::string& password);
        ~Server();
		void	run(void);

		// Server API
		// Server actions related
		void	send_reply(const std::string& reply);
		void	sendToAll(const std::string& message);
		void	sendToUser(const std::string& message, std::string& nick);
		void	sendMessageToChannel(const std::string& channelName, const std::string& message);
		chanIt	getChannelsBegin(void);
		chanIt	getChannelsEnd(void);
		bool 	isPasswordValid(const std::string& password);
		void	disconnectUser(void);

		// User actions related
		void				setUserNick(const std::string& nickname);
		const std::string&	getUserNick();
		void				setUserUsername(const std::string& username);
		const std::string	getUserIdentifier();
		void				setUserRegisteredStatus(bool status);
		bool 				isUserRegistered();
		void				setUserInvisibleMode(bool status);
		bool				isUserInvisible();
		void 				setUserPasswordState(bool state);
		bool 				getUserPasswordState(void);
		usrsIt				getUsersBegin(void);
		usrsIt				getUsersEnd(void);

		User*				getUser(void);
		
		//Channel
		bool				doesChannelExist(const std::string& channelName);
		void				addChannel(const std::string& channelName);
		void				addUserToChannel(const std::string& channelName);
		bool				isChannelFull(const std::string& channelName);
		bool				doesChannelHaveLimit(const std::string& channelName);
		void				promoteChannelMember(const std::string& channelName);
		void				promoteChannelMember(const std::string& channelName, const std::string& nickname);
		void				demoteChannelOperator(const std::string& channelName, const std::string& nickname);
		void				promoteChannelInvitee(const std::string& channelName);
		bool				isChannelPasswordProtected(const std::string& channelName);
		bool				isChannelPasswordValid(const std::string& channelName, const std::string& key);
		void				setChannelPassword(const std::string& channelName, const std::string&  key);
		void				clearChannelPassword(const std::string& channelName);
		const std::string&	getChannelPassword(const std::string& channelName);
		bool				isChannelInviteOnly(const std::string& channelName);
		bool				isChannelTopicSet(const std::string& channelName);
		bool				isUserInvited(const std::string& channelName);
		const std::string&	getChannelTopic(const std::string& channelName);
		const std::string	getChannelUsersList(const std::string& channelName);
		bool				isChannelUser(const std::string& channelName);
		bool				isTargetInChannel(const std::string& channelName, const std::string& nick);

		//Kick
		void				removeUserFromChannel(const std::string& channelName, const std::string& nick);
		bool				isChannelTopicProtected(const std::string& channelName); //
		bool				isUserChannelOperator(const std::string& channelName); //
		void				setChannelInviteOnly(const std::string& channelName, bool status); //

		void 				setChannelTopicRestricted(const std::string& channelName, bool status); //
		void				addInviteeToChannel(const std::string& channelName, const std::string& nickname);
		void				setNewTopic(const std::string& channelName, const std::string& topic);


		void				setChannelHasLimit(const std::string& channelName, bool status);
		void				setChannelLimit(const std::string& channelName, int limit);
		bool				isTargetChannelMember(const std::string& channelName, std::string& userNickname);
		bool				isTargetChannelOperator(const std::string& channelName, std::string& userNickname);

};
