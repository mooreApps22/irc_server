// #include "../inc/data.hpp"
#include "Server.hpp"
#include "Logger.hpp"
#include <iostream>
#include <stdexcept>
#include <string>


/*
	Server Terminal: ./ircserv 6667 password
	
	Client Terminal: nc -C 127.0.0.1 6667
*/

int	main(int ac, char **av)
{
	Logger::log(INFO, "ircserv compiled.");
	if (ac != 3) {
		std::cerr << "Usage: ./ircserv <port> <password>" << std::endl;
		return (1);
	}
	Logger::log(INFO, "port and password validated.");
	const std::string	port(av[1]);
	const std::string	password(av[2]);
	Server server(port, password);	
	
	try
	{	
		server.run();
	}
	catch (const std::exception &e)
	{
		std::cerr << "Exception catched: ";
		std::cerr << e.what() << std::endl;
		return (1);
	}
	return (0);
}
