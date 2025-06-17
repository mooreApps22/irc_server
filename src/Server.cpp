#include "../inc/Server.hpp"
#include "../inc/Logger.hpp"
#include "signal.hpp"
#include <iostream>
#include <netdb.h>
#include <cstring>
#include <errno.h>
#include <sys/socket.h>
#include <sys/epoll.h>
#include <unistd.h>

#define BACKLOG		10
#define BUFFER_SIZE	1024
#define MAX_EVENTS	10

Server::Server(const std::string& port, const std::string& password)
	:	_port(port),
		_password(password),
		_server_fd(-1),
		_epoll_fd(-1)
{
	//std::cout << "Server Constructor called." << std::endl;
	Logger::log(INFO, "Server Constructor called.");
	sig::set_signals();
	_ch = new CommandHandler(*this, _parser);
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
	//std::cout << "Server Destructor called." << std::endl;
	Logger::log(INFO, "Server Destructor called.");
	clean_up();
	delete _ch;
}

void Server::setup(void)
{
	struct addrinfo hints;
	struct addrinfo *res;

//	std::cout << "Server Setup called." << std::endl;
	Logger::log(INFO, "Server Setup called.");

	memset(&hints, 0, sizeof hints);
	hints.ai_family = AF_UNSPEC;
	hints.ai_socktype = SOCK_STREAM;
	hints.ai_flags = AI_PASSIVE;

	getaddrinfo(NULL, _port.c_str(), &hints, &res);

	Logger::log(INFO, "TCP/IP data stored.");
	_server_fd = socket(res->ai_family, res->ai_socktype | SOCK_NONBLOCK, res->ai_protocol);
	if (_server_fd == -1)
		throw std::runtime_error("Socket creation failed");
	Logger::log(INFO, "_server_fd assigned.");

	if (bind(_server_fd, res->ai_addr, res->ai_addrlen) == -1)
		throw std::runtime_error("Bind failed");
	Logger::log(INFO, "_server_fd bind()ed to port.");
	freeaddrinfo(res);
	
	if (listen(_server_fd, BACKLOG) == -1)
		throw std::runtime_error("Listen failed");
	Logger::log(INFO, "Server is listening to port.");
	
	_epoll_fd = epoll_create1(0);
	if (_epoll_fd == -1)
		throw std::runtime_error("The eevent poll could not be created.");
	Logger::log(INFO, "Event poll created.");
	if (register_fd(_server_fd) == -1)
		throw std::runtime_error("The listener could not be registered to the event poll.");
	Logger::log(INFO, "Listener registered to the event poll.");
	std::cout << "Server listening on port... " << std::endl;
}

void Server::run(void)
{
	struct epoll_event	events[MAX_EVENTS];
	int					n_events;
	std::string			message;

	setup();

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
			{	
				message = get_message(events[i].data.fd);
				if (!message.empty())
				{
					if (_parser.parse_message(message))
						_ch->execute();	
					else
						Logger::log(INFO, "Command Syntax Error.");	
				}
				// reply_message(message, events[i].data.fd);
			}
		}
	}
}

int	Server::register_fd(int fd)
{
	struct epoll_event ev;
	ev.events = EPOLLIN;
	ev.data.fd = fd;
	return epoll_ctl(_epoll_fd, EPOLL_CTL_ADD, fd, &ev);
}

void Server::accept_connection()
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
		if (register_fd(client_fd) == -1)
		{
			send(client_fd, "ERROR :processing your request, please try again!\r\n", 51, 0);
			close(client_fd);
		}
		else
			add_new_user(client_fd);
	}
}

void Server::add_new_user(int fd)
{
	_users[fd] = new User();	
}

void Server::delete_user(int fd)
{
		close(fd);
		delete _users[fd];
		_users.erase(fd);
}

std::string	Server::get_message(int fd)
{
	User*		user;
	std::string	message;
	int			length;

	user = _users[fd];
	message = peek(fd);
		
	if (!message.empty())
	{			
		if (_parser.is_partial(message))
		{
			message = receive(fd, message.length());
			user->buffer(message);
			message.clear();
		}
		else
		{
			length = _parser.get_message_length(message);
			message = user->get_partial_message();
			message.append(receive(fd, length));
		}
	}
	// else if (bytes_read == -1 && (errno == EAGAIN || errno == EWOULDBLOCK))
	// 	std::cout << "Nothing to read just yet!" << std::endl;
	else
	{
		std::cerr << "Read failed or client disconnected" << std::endl;
		delete_user(fd);
	}
	return message;
}

std::string Server::peek(int fd)
{
	char	buffer[BUFFER_SIZE];
	int		bytes_read;

	bytes_read = recv(fd, buffer, sizeof(buffer) - 1, MSG_PEEK);
	buffer[bytes_read] = '\0';

	return buffer;
}

std::string Server::receive(int fd, int length)
{
	char	buffer[length + 1];
	int		bytes_read;

	bytes_read = recv(fd, buffer, length, 0);
	buffer[bytes_read] = '\0';

	return buffer;
}

void	Server::reply_message(std::string& message, int fd)
{
	std::cout << "Execute from fd" << fd << ": " << message;
	if (message.find("NICK") == 0)
	{
		if (!_users[fd]->is_registered())
		{
			send_reply(":localhost 464  :Password incorrect", fd);
			send_reply("ERROR", fd);
			delete_user(fd);
		}
	}
}

void	Server::send_reply(const std::string& reply, int fd)
{
	send(fd, reply.c_str(), reply.length(), 0);
	send(fd, CRLF, 2, 0);
}

void	Server::clean_up(void)
{
	if (_server_fd != -1)
		close(_server_fd);
	if (_epoll_fd != -1)
		close(_epoll_fd);
}
