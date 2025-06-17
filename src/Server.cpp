#include "../inc/Server.hpp"
#include "../inc/Logger.hpp"
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
#include <cstdlib>

Server::Server(const std::string& port, const std::string& password)
	:	_server_fd(-1),
		_client_fd(-1),
		_port(port),
		_password(password)
{
	std::cout << "Server Constructor called." << std::endl;
	std::memset(&_server_addr, 0, sizeof(_server_addr));
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
	if (_client_fd != -1)
		close(_client_fd);
	if (_server_fd != -1)
		close(_server_fd);
}

void	Server::setup(void)
{
	int port_num = atoi(_port.c_str());

	Logger::log(INFO, "Server Setup called.");
	_server_fd = socket(AF_INET, SOCK_STREAM, 0);
	if (_server_fd < 0)
		throw std::runtime_error("Socket creation failed");
	_server_addr.sin_family = AF_INET;
	_server_addr.sin_port = htons(port_num);
	_server_addr.sin_addr.s_addr = inet_addr("127.0.0.1");
	if (bind(_server_fd, (struct sockaddr*)&_server_addr, sizeof(_server_addr)) < 0)
		throw std::runtime_error("Bind failed");
	if (listen(_server_fd, 10) < 0)
		throw std::runtime_error("Listen failed");
	std::cout << "Server listening on port... " << std::endl;
}

void	Server::run(void)
{
	struct sockaddr_in	client_addr;
	socklen_t			client_len = sizeof(client_addr);
	char buffer[1024];
	int bytes_read;

	_client_fd = accept(_server_fd, (struct sockaddr*)&client_addr, &client_len);
	if (_client_fd < 0)
		throw std::runtime_error("Accept failed");
	std::cout << "Client connected!" << std::endl;
	bytes_read = read(_client_fd, buffer, sizeof(buffer) - 1);
	if (bytes_read > 0)
	{
		buffer[bytes_read] = '\0';
		std::cout << "Received: " << buffer << std::endl;
	}
	else
	{
		std::cerr << "Read failed or client disconnected" << std::endl;
	}
}
