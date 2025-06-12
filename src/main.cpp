#include "../inc/data.hpp"
#include "../inc/Server.hpp"
#include <iostream>
#include <stdexcept>
#include <string>
#include <sys/socket.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <cstring>
#include <netdb.h>
#include <errno.h>
#include <stdio.h>

/*
	Server Terminal: ./ircserv 6667 password
	
	Client Terminal: nc 127.0.0.1 6667
*/

int	main(int ac, char **av)
{
	try	
	{
		if (ac != 3)
			throw std::runtime_error("Usage: ./ircserv <port> <password>");
		Server server(av[1], av[2]);

		server.setup();
		server.run();
	}
	catch (const std::exception &e)
	{
		std::cerr << e.what() << std::endl;
		return (1);
	}
	return (0);
}
