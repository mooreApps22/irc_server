#include "../inc/data.hpp"
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

		memset(&server_addr, 0, sizeof(server_addr));
		server_addr.sin_family = AF_INET;
		server_addr.sin_port = htons(6667);
		server_addr.sin_addr.s_addr = inet_addr("127.0.0.1");

		std::cout << "Sin Family: " << server_addr.sin_family  << std::endl;
		std::cout << "Sin Port: " << server_addr.sin_port  << std::endl;
		std::cout << "Sin Address: " << server_addr.sin_addr.s_addr  << std::endl;
		//Auto fill IP Address
		struct addrinfo	hints;
		struct addrinfo	*res;
		struct addrinfo	*r;
		int				status;
		char			buffer[INET6_ADDRSTRLEN];

		memset(&hints, 0, sizeof hints);
		hints.ai_family = AF_UNSPEC;
		hints.ai_socktype = SOCK_STREAM;
		status = getaddrinfo(av[1], 0, &hints, &res);
		if (status != 0)
		{
			std::cerr << "getaddrinfo error status: " << status << std::endl;
			return (2);
		}
		std::cout << "IP Addr for :" << av[1] << std::endl;
		r = res;
		while (r)
		{
			if (r->ai_family == AF_INET)
			{
				struct sockaddr_in *ipv4 = (struct sockaddr_in *)r->ai_addr;
				inet_ntop(r->ai_family, &(ipv4->sin_addr), buffer, sizeof buffer); // not allowable 
				printf("IPv4: %s\n", buffer);
			}
			r = r->ai_next;
		}
		freeaddrinfo(res);

		if (bind(server_fd, (struct sockaddr*)&server_addr, sizeof(server_addr)) < 0)
		{
			perror("bind failed");
			close(server_fd);
			return (1);
		}
		if (listen(server_fd, 10) < 0)
		{
			perror("listen failed");
			close(server_fd);
			return (1);
		}

		std::cout << "Listening..." << std::endl;
		struct sockaddr_in client_addr;
		socklen_t client_len = sizeof client_addr;
		int client_fd = accept(server_fd, (struct sockaddr*)&client_addr, &client_len);
		if (client_fd < 0)
		{
			perror("Accept failed");
			close(server_fd);
			return (1);
		}
		std::cout << "Client connected!" << std::endl;
		int	bytes_read = read(client_fd, buffer, sizeof buffer - 1);
		if (bytes_read > 0)
		{
			buffer[bytes_read] = '\0';
			std::cout << "Received:" << buffer << std::endl;
		}
		close(client_fd);
		close(server_fd);
	}
	catch (const std::exception &e)
	{
		std::cerr << e.what() << std::endl;
		return (1);
	}
	return (0);
}


