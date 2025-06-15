#ifndef SERVER_HPP
# define SERVER_HPP
# include <string>

class Server
{
    private:
		const std::string	_port;	
		const std::string	_password;
		int					_server_fd;
		int					_epoll_fd;

		void	setup();
		void	clean_up();
		void	accept_connections();
		void	check_events();

    public:
        Server();
        Server(const std::string& port, const std::string& password);
        Server(const Server& other);
        Server& operator=(const Server& other);
        ~Server();
		
		void	run(void);
};

#endif
