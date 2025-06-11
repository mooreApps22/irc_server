#include "../inc/data.hpp"
#include <iostream>
#include <stdexcept>
#include <string>
#include <sys/socket.h>
#include <unistd.h>

int	main(int ac, char **av)
{
	try	
	{
		if (ac != 3)
			throw std::runtime_error("Usage: ./ircserv <port> <password>");
		std::string port = av[1]; // port # of IRC server which listens for incoming IRC connections 
		std::string password = av[2]; // The connection password, the IRC client needs it to connect to server
		std::cout << "Port: " << port << std::endl;
		std::cout << "Password: " << password << std::endl;
		int server_fd = socket(AF_INET, SOCK_STREAM, 0);
		std::cout << server_fd << std::endl;
		close(server_fd);
	}
	catch (const std::exception &e)
	{
		std::cerr << e.what() << std::endl;
		return (1);
	}
	return (0);
}
