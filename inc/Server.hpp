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
		void		add_new_user();
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
		bool 	isPasswordValid(const std::string& password);
		void	disconnectUser(void);

		// User actions related
		void				setUserNick(const std::string& nickname);
		const std::string	getUserNick();
		void				setUserRegisteredStatus(bool status);
		bool 				isUserRegistered();
		void 				setUserPasswordState(bool state);
		bool 				getUserPasswordState(void);
		usrsIt				getUsersBegin(void);
		usrsIt				getUsersEnd(void);
		// Channel API methods
		Channel*	getChannel(const std::string& channel_name);
		void 		addUserToChannel(User* user, const std::string& channel_name);
		User*		getUser(const int fd);
		int			getFdFromNickname(const std::string& nickname);
		void		addChannel(const std::string& name, Channel* channel);
};
