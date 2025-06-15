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
#define MAX_EVENTS	10
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
	struct epoll_event events[MAX_EVENTS];
	int n_events;

	setup();
	register_fd(_server_fd);

	while (!sig::stop)	
	{
		n_events = epoll_wait(_epoll_fd, events, MAX_EVENTS, 0);
		if (n_events == -1)
			throw std::runtime_error("Events poll error.");

		for (int i = 0; i < n_events; i++)
		{
			if (events[i].data.fd == _server_fd)
				accept_connection();
			else
				use_fd(events[i].data.fd);
		}
	}
}

void	Server::register_fd(int fd)
{
	struct epoll_event ev;
	ev.events = EPOLLIN;
	ev.data.fd = fd;
	if (epoll_ctl(_epoll_fd, EPOLL_CTL_ADD, fd, &ev) == -1)
		std::cerr << "Could not add fd " << fd << " to the epoll." << std::endl;
}

void	Server::accept_connection()
{
	struct sockaddr_storage	client_addr;
	socklen_t				client_len = sizeof(client_addr);
	int 					client_fd = -1;

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
		register_fd(client_fd);
	}
}

void	Server::use_fd(int fd)
{
	char	buffer[BUFFER_SIZE];
	int		bytes_read;

	bytes_read = recv(fd, buffer, sizeof(buffer) -1, MSG_DONTWAIT | MSG_PEEK);
	buffer[bytes_read] = '\0';
	if (bytes_read > 0)
	{			
		std::string	peek_str = buffer;
		size_t		crlf = peek_str.find(CRLF);

		if (crlf != std::string::npos)
		{
			bytes_read = recv(fd, buffer, crlf + 2, MSG_DONTWAIT); // No flag means that recv is blocking, however we know that there is data to read!!!
				buffer[bytes_read] = '\0';
				std::cout << "Received from fd" << fd << ": " << buffer;
		}
	}
	else if (bytes_read == -1 && (errno == EAGAIN || errno == EWOULDBLOCK))
		std::cout << "Nothing to read just yet!" << std::endl;
	else
	{
		std::cerr << "Read failed or client disconnected" << std::endl;
		close (fd);
	}
}				


void	Server::clean_up(void)
{
	if (_server_fd != -1)
		close(_server_fd);
	if (_epoll_fd != -1)
		close(_epoll_fd);
}
