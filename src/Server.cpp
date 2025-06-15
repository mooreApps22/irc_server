#include "../inc/Server.hpp"
#include "signal.hpp"
#include <iostream>
#include <netdb.h>
#include <cstring>
#include <errno.h>
#include <sys/socket.h>
#include <sys/epoll.h>
#include <unistd.h>

// #include <stdexcept>
// #include <string>
// #include <sys/socket.h>
// #include <arpa/inet.h>
// #include <netinet/in.h>
// 
// #include <stdio.h>
// #include <cstdlib>

#define BACKLOG		10
#define BUFFER_SIZE	1024
#define CRLF		"\r\n"

Server::Server(const std::string& port, const std::string& password)
	:	_port(port),
		_password(password),
		_server_fd(-1),
		_epoll_fd(-1)
{
	std::cout << "Server Constructor called." << std::endl;
	sig::set_signals();
}

Server::Server(const Server& other)
	:	_port(other._port),
		_password(other._password)
{
	std::cout << "Server Copy Constructor called." << std::endl;
	*this = other;
}

Server& Server::operator=(const Server& other)
{
	std::cout << "Server Copy Assignment Opperator called." << std::endl;
	(void)other;
	return (*this);
}

Server::~Server()
{
	std::cout << "Server Destructor called." << std::endl;
	clean_up();
}

void	Server::setup(void)
{
	struct addrinfo hints;
	struct addrinfo *res;

	std::cout << "Server Setup called." << std::endl;

	memset(&hints, 0, sizeof hints);
	hints.ai_family = AF_UNSPEC;
	hints.ai_socktype = SOCK_STREAM;
	hints.ai_flags = AI_PASSIVE;

	getaddrinfo(NULL, _port.c_str(), &hints, &res);

	// struct sockaddr_in *ipv4;
	// void *addr;
	// char ipstr[INET6_ADDRSTRLEN];

	// ipv4 = (struct sockaddr_in *)res->ai_addr;
	// addr = &(ipv4->sin_addr);
	// inet_ntop(res->ai_family, addr, ipstr, sizeof ipstr);
	// std::cout << "ip: " << ipstr;

	_server_fd = socket(res->ai_family, res->ai_socktype | SOCK_NONBLOCK, res->ai_protocol);
	if (_server_fd == -1)
		throw std::runtime_error("Socket creation failed");

	if (bind(_server_fd, res->ai_addr, res->ai_addrlen) == -1)
		throw std::runtime_error("Bind failed");
	freeaddrinfo(res);
	
	if (listen(_server_fd, BACKLOG) == -1)
		throw std::runtime_error("Listen failed");
	std::cout << "Server listening on port... " << std::endl;
	
	_epoll_fd = epoll_create1(0);
	if (_epoll_fd == -1)
		throw std::runtime_error("The epoller could not be created.");
}

void	Server::run(void)
{
	setup();


	// Accepting connections
	struct sockaddr_storage	client_addr;
	socklen_t				client_len = sizeof(client_addr);
	int 					client_fd = -1;

	// Epoll
	struct epoll_event client_event;
	int events_ready;
	struct epoll_event events_queue[10];

	//  Receive
	char 					buffer[BUFFER_SIZE];
	int 					bytes_read;

	while (!sig::stop)	
	{
		client_fd = accept(_server_fd, (struct sockaddr*)&client_addr, &client_len);
		if (client_fd == -1)
		{
			if (errno != EAGAIN && errno != EWOULDBLOCK)
				throw std::runtime_error("Accept failed");
		}
		else
		{
			std::cout << "Client connected! fd: " << client_fd << std::endl;
			send(client_fd, "Connection stablished!\n", 23, 0);

			// Add client to epoll for reading
			client_event.data.fd = client_fd;
			client_event.events = EPOLLIN;
			// client_event.events = EPOLLIN | EPOLLET;
			if (epoll_ctl(_epoll_fd, EPOLL_CTL_ADD, client_fd, &client_event) == -1)
				std::cerr << "Could not add fd " << client_fd << " to the epoll." << std::endl;
		}

		events_ready = epoll_wait(_epoll_fd, events_queue, 10, 0);
		if (events_ready > 0)
		{
			for (int i = 0; i < events_ready; i++)
			{
				client_fd = events_queue[i].data.fd;
				
				bytes_read = recv(client_fd, buffer, sizeof(buffer) -1, MSG_DONTWAIT | MSG_PEEK);
				buffer[bytes_read] = '\0';
				if (bytes_read > 0)
				{			
					std::string	peek_str = buffer;
					size_t		crlf = peek_str.find(CRLF);

					if (crlf != std::string::npos)
					{
						bytes_read = recv(client_fd, buffer, crlf + 2, MSG_DONTWAIT); // No flag means that recv is blocking, however we know that there is data to read!!!
							buffer[bytes_read] = '\0';
							std::cout << "Received from fd" << client_fd << ": " << buffer;
					}
				}
				else if (bytes_read == -1 && (errno == EAGAIN || errno == EWOULDBLOCK))
					std::cout << "Nothing to read just yet!" << std::endl;
				else
				{
					std::cerr << "Read failed or client disconnected" << std::endl;
					close (client_fd);
				}				
			}
		}
	}
}

void	Server::clean_up(void)
{
	if (_server_fd != -1)
		close(_server_fd);
	if (_epoll_fd != -1)
		close(_epoll_fd);
}
