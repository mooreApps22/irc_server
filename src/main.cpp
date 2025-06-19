 #include "../inc/data.hpp"
#include "../inc/Server.hpp"
#include "../inc/Logger.hpp"
#include <iostream>
// #include <stdexcept>
// #include <string>
// #include <sys/socket.h>
// #include <unistd.h>
// #include <arpa/inet.h>
// #include <netinet/in.h>
// #include <cstring>
// #include <netdb.h>
// #include <errno.h>
// #include <stdio.h>
// #include <csignal>
// #include <cstdlib>

/*
	Server Terminal: ./ircserv -C 6667 password
	
	Client Terminal: nc 127.0.0.1 6667
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
	// server.stop()
	return (0);
}
