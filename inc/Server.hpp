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

		void		clean_up();

    public:
        Server(const std::string& port, const std::string& password);
        ~Server();
		void	run(void);

		// Server API
		// Server actions related
		void	send_reply(const std::string& reply);
		void	sendToAll(const std::string& message);
		void	sendToUser(const std::string& message, int user_fd);
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
		void 				setUserPasswordState(bool state);
		bool 				getUserPasswordState(void);
		usrsIt				getUsersBegin(void);
		usrsIt				getUsersEnd(void);

		User*				getUser(void);
		
		//Channel
		int					getUserFd(const std::string& nick);
		Channel*			getChannel(const std::string& channelName);
		bool				doesChannelExist(const std::string& channelName);
		void				addChannel(const std::string& channelName);
		void				addUserToChannel(const std::string& channelName);
		bool				isChannelFull(const std::string& channelName);
		bool				doesChannelHaveLimit(const std::string& channelName);
		void				setUserAsOperator(const std::string& channelName);
		bool				isChannelPasswordProtected(const std::string& channelName);
		bool				isChannelPasswordValid(const std::string& channelName, const std::string& key);
		bool				isChannelInviteOnly(const std::string& channelName);
		bool				isUserInvited(const std::string& channelName);
};
