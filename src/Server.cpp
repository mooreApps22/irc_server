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

#include <arpa/inet.h>

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
					parsed_message	parsed_msg;

					if (_parser.parse_message(message, parsed_msg))
					{
						_ch.execute(parsed_msg);
					}
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
		char host[INET6_ADDRSTRLEN];
		
	
		std::cout << "Client connected! fd: " << _client_fd << std::endl;
		Logger::log(INFO, "Client connected! fd", _client_fd);

		if (client_addr.ss_family == AF_INET) // IPv4
			inet_ntop(AF_INET, &((struct sockaddr_in *)&client_addr)->sin_addr, host, sizeof host);
		else	 // IPv6
			inet_ntop(AF_INET6, &((struct sockaddr_in6 *)&client_addr)->sin6_addr, host, sizeof host);

		send(_client_fd, "Connection stablished!\n", 23, 0);
		if (register_fd(_client_fd) == -1)
		{
			send(_client_fd, "ERROR: error processing your request, please try again!\r\n", 51, 0);
			close(_client_fd);
		}
		else
			add_new_user(host);
	}
}

void Server::add_new_user(const std::string& host)
{
	_users[_client_fd] = new User(host);	
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

int		Server::getUserFd(std::string nickname)
{
	for (usrsIt it = _users.begin(); it != _users.end(); it++)
	{
		if (it->second->getNickname() == nickname)
			return it->first;
	}
	return 0;
}

void	Server::clean_up(void)
{
	if (_server_fd != -1)
		close(_server_fd);
	if (_epoll_fd != -1)
		close(_epoll_fd);
}

/*
	API methods related to server actions
*/

void	Server::send_reply(const std::string& reply)
{
	Logger::log(DEBUG, "Sending reply", reply);
	send(_client_fd, reply.c_str(), reply.length(), 0);
	send(_client_fd, CRLF, 2, 0);
}

void	Server::sendToAll(const std::string& message)
{
	Logger::log(DEBUG, "Sending message to all users", message);
	for (usrsIt it = getUsersBegin(); it != getUsersEnd(); it++)
		sendToUser(message, it->first);
}

void	Server::sendToUser(const std::string& message, int user_fd)
{
	Logger::log(DEBUG, "Sending message to user", message);
	send(user_fd, message.c_str(), message.length(), 0);
	send(user_fd, CRLF, 2, 0);
}

void	Server::sendToUser(const std::string& message, std::string& nickname)
{
	Logger::log(DEBUG, "Sending message to user", message);
	int user_fd = getUserFd(nickname);
	sendToUser(message, user_fd);
}

void	Server::disconnectUser(void)
{
	close(_client_fd);
	// TODO delete user from all channels (membership and invites)
	delete _users[_client_fd];
	_users.erase(_client_fd);
}

bool 	Server::isPasswordValid(const std::string& password)
{
	return _password == password;
}


/*
	API methods related to user actions
*/

void	Server::setUserNick(const std::string& nickname)
{
	_users[_client_fd]->setNickname(nickname);
}

const std::string&	Server::getUserNick()
{
	return _users[_client_fd]->getNickname();
}

void	Server::setUserUsername(const std::string& username)
{
	_users[_client_fd]->setUsername(username);
}

const std::string	Server::getUserIdentifier()
{
	return _users[_client_fd]->getIdentifier();
}

void	Server::setUserRegisteredStatus(bool status)
{
	_users[_client_fd]->setRegisteredStatus(status);
}

bool 	Server::isUserRegistered()
{
	return _users[_client_fd]->isRegistered();
}

void 	Server::setUserPasswordState(bool state)
{
	_users[_client_fd]->setPasswordStatus(state);
}

bool 	Server::getUserPasswordState(void)
{
	return _users[_client_fd]->getPasswordStatus();
}

void	Server::setUserInvisibleMode(bool status)
{
	_users[_client_fd]->setInvisibleStatus(status);
}

bool	Server::isUserInvisible()
{
	return _users[_client_fd]->isInvisible();
}

usrsIt	Server::getUsersBegin(void)
{
	return _users.begin();
}

usrsIt	Server::getUsersEnd(void)
{
	return _users.end();
}


User*	Server::getUser()
{
	return (_users[_client_fd]);
}
