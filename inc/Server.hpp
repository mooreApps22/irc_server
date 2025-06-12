#ifndef SERVER_HPP
# define SERVER_HPP
# include <iostream>
# include <string>
# include <netinet/in.h>

class Server
{
    private:
		int						_server_fd;
		int						_client_fd;
		const std::string		_port;	
		const std::string		_password;	
		struct sockaddr_in		_server_addr;
    public:
        Server();
        Server(const std::string& port, const std::string& password);
        Server(const Server& other);
        Server& operator=(const Server& other);
        ~Server();
		
		void	setup(void);
		void	run(void);
};

#endif
