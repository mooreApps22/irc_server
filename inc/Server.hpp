#ifndef SERVER_HPP
# define SERVER_HPP
#include "User.hpp"
#include "Parser.hpp"
# include <string>
#include <map>

class Server
{
    private:
		const std::string		_port;
		const std::string		_password;
		int						_server_fd;
		int						_epoll_fd;
		std::map<int, User*>	_users;
		Parser					_parser;

		void		setup();
		int			register_fd(int fd);
		void		accept_connection();
		void		add_new_user(int fd);
		void		delete_user(int fd);
		std::string	get_message(int fd);
		std::string peek(int fd);
		std::string receive(int fd, int length);

		// void		execute_commad();
		void		reply_message(std::string& message, int fd);
		void		send_reply(const std::string& reply, int fd);
		void		clean_up();

    public:
        Server();
        Server(const std::string& port, const std::string& password);
        Server(const Server& other);
        Server& operator=(const Server& other);
        ~Server();
		
		void	run(void);
};

#endif
