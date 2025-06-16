#ifndef SERVER_HPP
# define SERVER_HPP
#include "User.hpp"
# include <string>
#include <map>

class Server
{
    private:
		const std::string		_port;
		const std::string		_password;
		int						_server_fd;
		int						_epoll_fd;
		std::map<int, User*>	_incoming;

		void		setup();
		int			register_fd(int fd);
		void		accept_connection();
		std::string	get_message(int fd);
		void		execute_commad();		
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
