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
		usrs							_users;
		chan							_channels;
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

		// API methods
		void	send_reply(const std::string& reply);
		bool 	isUserRegistered();
		bool 	isPasswordValid(const std::string& password);
		void 	setUserPassword(bool state);
		bool 	getUserPassword(void);
		usrs	getUsers(void);
		void	setUserNick(const std::string& nickname);
		void	disconnectUser(void);
};
