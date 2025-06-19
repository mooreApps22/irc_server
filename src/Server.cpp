#include "Server.hpp"
#include "Logger.hpp"
#include "signal.hpp"
#include <iostream>
#include <netdb.h>
#include <cstring>
#include <errno.h>
#include <sys/socket.h>
#include <sys/epoll.h>
#include <unistd.h>
#include <string>

#define BACKLOG		10
#define BUFFER_SIZE	1024
#define MAX_EVENTS	10

Server::Server(const std::string& port, const std::string& password)
	:	_port(port),
		_password(password),
		_server_fd(-1),
		_client_fd(-1),
		_epoll_fd(-1),
		_ch(*this)
{
	Logger::log(INFO, "Server Constructor called.");
	sig::set_signals();
}

Server::~Server()
{
	Logger::log(INFO, "Server Destructor called.");
	clean_up();
}

void Server::setup(void)
{
	struct addrinfo hints;
	struct addrinfo *res;

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
				_client_fd = events[i].data.fd;
				message = get_message();
				if (!message.empty())
				{
					parsed_message	parsed_message;

					if (_parser.parse_message(message, parsed_message))
						_ch.execute(parsed_message);
					else
						Logger::log(INFO, "Command Syntax Error.", message);	
				}
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

	_client_fd = accept(_server_fd, (struct sockaddr*)&client_addr, &client_len);
	if (_client_fd == -1)
	{
		if (errno != EAGAIN && errno != EWOULDBLOCK)
			throw std::runtime_error("Accept failed");
	}
	else
	{
		std::cout << "Client connected! fd: " << _client_fd << std::endl;
		Logger::log(INFO, "Client connected! fd", _client_fd);
		send(_client_fd, "Connection stablished!\n", 23, 0);
		if (register_fd(_client_fd) == -1)
		{
			send(_client_fd, "ERROR :processing your request, please try again!\r\n", 51, 0);
			close(_client_fd);
		}
		else
			add_new_user();
	}
}

void Server::add_new_user()
{
	_users[_client_fd] = new User();	
}

std::string	Server::get_message()
{
	User*		user;
	std::string	message;
	int			length;

	user = _users[_client_fd];
	message = peek();
		
	if (!message.empty())
	{			
		if (_parser.is_partial(message))
		{
			message = receive(message.length());
			user->buffer(message);
			message.clear();
		}
		else
		{
			length = _parser.get_message_length(message);
			message = user->get_partial_message();
			message.append(receive(length));
		}
	}
	// else if (bytes_read == -1 && (errno == EAGAIN || errno == EWOULDBLOCK))
	// 	std::cout << "Nothing to read just yet!" << std::endl;
	else
	{
		Logger::log(INFO, "Client disconnected!", _client_fd);
		// std::cerr << "Client disconnected! fd:" << _client_fd << std::endl;
		disconnectUser();
	}
	return message;
}

std::string Server::peek()
{
	char	buffer[BUFFER_SIZE];
	int		bytes_read;

	bytes_read = recv(_client_fd, buffer, sizeof(buffer) - 1, MSG_PEEK);
	buffer[bytes_read] = '\0';

	return buffer;
}

std::string Server::receive(int length)
{
	char	buffer[length + 1];
	int		bytes_read;

	bytes_read = recv(_client_fd, buffer, length, 0);
	buffer[bytes_read] = '\0';

	return buffer;
}

void	Server::clean_up(void)
{
	if (_server_fd != -1)
		close(_server_fd);
	if (_epoll_fd != -1)
		close(_epoll_fd);
}

/*
	API methods
*/

void	Server::send_reply(const std::string& reply)
{
	send(_client_fd, reply.c_str(), reply.length(), 0);
	send(_client_fd, CRLF, 2, 0);
}

bool 	Server::isUserRegistered()
{
	return _users[_client_fd]->getPassword();
}
bool 	Server::isPasswordValid(const std::string& password)
{
	return _password == password;
}

void 	Server::setUserPassword(bool state)
{
	_users[_client_fd]->setPassword(state);
}

void	Server::disconnectUser(void)
{
	close(_client_fd);
	// TODO delete user from all channels 
	delete _users[_client_fd];
	_users.erase(_client_fd);
}
