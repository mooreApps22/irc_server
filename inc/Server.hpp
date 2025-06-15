#ifndef SERVER_HPP
# define SERVER_HPP
# include <string>
#include <map>

class Server
{
    private:
		const std::string	_port;
		const std::string	_password;
		int					_server_fd;
		int					_epoll_fd;
		std::map<int, bool>	_incoming;

		void	setup();
		int		register_fd(int fd);
		void	accept_connection();
		void	use_fd(int fd);
		void	execute(std::string& command, int fd);
		void	clean_up();
		// void	check_events();

    public:
        Server();
        Server(const std::string& port, const std::string& password);
        Server(const Server& other);
        Server& operator=(const Server& other);
        ~Server();
		
		void	run(void);
};

#endif
