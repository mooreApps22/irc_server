#include "../inc/data.hpp"
#include <iostream>
#include <stdexcept>
#include <string>
#include <sys/socket.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <netinet/in.h>

int	main(int ac, char **av)
{
	try	
	{
		if (ac != 3)
			throw std::runtime_error("Usage: ./ircserv <port> <password>");
		// ARGS
		std::string port = av[1]; // port # of IRC server which listens for incoming IRC connections 
		std::string password = av[2]; // The connection password, the IRC client needs it to connect to server
		std::cout << "Port: " << port << std::endl;
		std::cout << "Password: " << password << std::endl;
		// SOCKET
		int server_fd = socket(AF_INET, SOCK_STREAM, 0);
		std::cout << server_fd << std::endl;
	
		//Converting to Network  
		uint32_t num = 4242424242;
		uint32_t net_num = htonl(num);

		std::cout << "Host order: " <<  num << std::endl;
		std::cout << "Network order: " <<  net_num << std::endl;
		// Converting to Host	
		uint32_t host_num = 3322111122;
		uint32_t net_num2 = ntohl(host_num);

		std::cout << "Network order: " <<  net_num2 << std::endl;
		std::cout << "Host order: " <<  host_num << std::endl;
		// PREPARING A SOCKET STRUCTURE
		struct sockaddr_in server_addr;

		std::memset(&server_addr, 0, sizeof(server_addr));
		server_addr.sin_family = AF_INET;
		server_addr.sin_port = htons(6667);
		server_addr.sin_addr.s_addr = inet_addr("127.0.0.1");
		
		close(server_fd);
	}
	catch (const std::exception &e)
	{
		std::cerr << e.what() << std::endl;
		return (1);
	}
	return (0);
}
