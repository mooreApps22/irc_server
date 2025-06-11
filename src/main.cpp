#include "../inc/data.hpp"
#include <iostream>
#include <stdexcept>
#include <string>

int	main(int ac, char **av)
{
	try	
	{
		if (ac != 3)
			throw std::runtime_error("Usage: ./ircserv <port> <password>");
		std::string port = av[1];
		std::string password = av[2];
		std::cout << "Port: " << port << std::endl;
		std::cout << "Password: " << password << std::endl;
	}
	catch (const std::exception &e)
	{
		std::cerr << e.what() << std::endl;
		return (1);
	}
	return (0);
}
