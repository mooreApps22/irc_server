#include "Server.hpp"
#include "Logger.hpp"
#include "signal.hpp"
#include "macros.hpp"
#include "User.hpp"
#include <sys/epoll.h>
#include <netdb.h>
#include <cstring>
#include <iostream>
#include <arpa/inet.h>
#include <vector>
#include <string>
#include <iostream>

Server::Server(const std::string& port, const std::string& password)
	:	_port(port),
		_password(password),
		_serverFd(-1),
		_clientFd(-1),
		_epollFd(-1),
		_ch(*this)
{
	// Logger::log(INFO, "Server Constructor called.");
	sig::setSignals();
}

Server::~Server()
{
	// Logger::log(INFO, "Server Destructor called.");
	cleanUp();
}


void Server::run(void)
{
	struct epoll_event	events[MAX_EVENTS];
	int					nEvents;
	std::string			message;

	setup();

	// Logger::log(DEBUG, "run() called.");
	while (!sig::stop)	
	{
		nEvents = epoll_wait(_epollFd, events, MAX_EVENTS, 0);
		if (nEvents == -1)
			throw std::runtime_error("Events poll error.");

		for (int i = 0; i < nEvents; i++)
		{
			if (events[i].data.fd == _serverFd)
				acceptConnection();
			else
			{	
				_clientFd = events[i].data.fd;
				message = getMessage();
				if (!message.empty())
				{
					parsedMessage	parsedMsg;

					if (_parser.parseMessage(message, parsedMsg))
					{
						// 	Logger::log(DEBUG, "execute() called with the command: ", parsedMsg.command);
						_ch.execute(parsedMsg);
					}
					else
						Logger::log(INFO, "Command Syntax Error.", message);	
				}
			}
		}
	}
	_ch.disconnectServer();
	std::cout << "Server disconnected." << std::endl;
}

void Server::setup(void)
{
	struct addrinfo hints;
	struct addrinfo *res;

	Logger::log(INFO, "setup() called.");

	memset(&hints, 0, sizeof hints);
	hints.ai_family = AF_UNSPEC;
	hints.ai_socktype = SOCK_STREAM;
	hints.ai_flags = AI_PASSIVE;

	getaddrinfo(NULL, _port.c_str(), &hints, &res);

	// Logger::log(INFO, "TCP/IP data stored.");
	_serverFd = socket(res->ai_family, res->ai_socktype | SOCK_NONBLOCK, res->ai_protocol);
	if (_serverFd == -1)
	{
		freeaddrinfo(res);
		throw std::runtime_error("Socket creation failed");
	}
	// Logger::log(INFO, "_serverFd assigned.");

	if (bind(_serverFd, res->ai_addr, res->ai_addrlen) == -1)
	{
		freeaddrinfo(res);
		throw std::runtime_error("Bind failed");
	}
	// Logger::log(INFO, "_serverFd bind()ed to port.");
	freeaddrinfo(res);
	
	if (listen(_serverFd, BACKLOG) == -1)
		throw std::runtime_error("Listen failed");
	Logger::log(INFO, "Server is listening to port.", _port);
	
	_epollFd = epoll_create1(0);
	if (_epollFd == -1)
		throw std::runtime_error("The event poll could not be created.");
	// Logger::log(INFO, "Event poll created.");
	if (registerFd(_serverFd) == -1)
		throw std::runtime_error("The listener could not be registered to the event poll.");
	// Logger::log(INFO, "Listener registered to the event poll.");
	std::cout << "Server listening on port " << _port << std::endl;
}

void	Server::cleanUp(void)
{
	if (_serverFd != -1)
	{
		unregisterFd(_serverFd);
		close(_serverFd);
	}
	if (_epollFd != -1)
		close(_epollFd);
}


/*
	Polling
*/
int	Server::registerFd(int fd) const
{
	struct epoll_event ev;
	ev.events = EPOLLIN;
	ev.data.fd = fd;
	return (epoll_ctl(_epollFd, EPOLL_CTL_ADD, fd, &ev));
}

int	Server::unregisterFd(int fd) const
{
	struct epoll_event ev;
	ev.events = EPOLLIN;
	ev.data.fd = fd;
	return (epoll_ctl(_epollFd, EPOLL_CTL_DEL, fd, &ev));
}


/*
	Clients
*/

void Server::acceptConnection()
{
	struct sockaddr_storage	clientAddr;
	socklen_t				clientLen = sizeof(clientAddr);

	_clientFd = accept(_serverFd, (struct sockaddr*)&clientAddr, &clientLen);
	if (_clientFd == -1)
	{
		if (errno != EAGAIN && errno != EWOULDBLOCK)
			throw std::runtime_error("Accept failed");
	}
	else
	{
		char host[INET6_ADDRSTRLEN];
	
		Logger::log(INFO, "Client connected! fd", _clientFd);

		if (clientAddr.ss_family == AF_INET) // IPv4
			inet_ntop(AF_INET, &((struct sockaddr_in *)&clientAddr)->sin_addr, host, sizeof host);
		else	 							// IPv6
			inet_ntop(AF_INET6, &((struct sockaddr_in6 *)&clientAddr)->sin6_addr, host, sizeof host);

		sendToClient("Connection stablished!", _clientFd);
		if (registerFd(_clientFd) == -1)
		{
			sendToClient("ERROR: error processing your request, please try again!", _clientFd);
			close(_clientFd);
		}
		else
			addNewClient(host);
	}
}

void Server::addNewClient(const std::string& host)
{
	_users[_clientFd] = new User(host);
}


/*
	Receiving data
*/

std::string	Server::getMessage() const
{
	User*		user;
	std::string	message;
	int			length;

	user = _users.at(_clientFd);
	message = peek();

	if (!message.empty())
	{			
		if (Parser::isPartial(message))
		{
			message = receive(message.length());
			user->buffer(message);
			message.clear();
		}
		else
		{
			length = Parser::getMessageLength(message);
			message = user->getPartialMessage();
			message.append(receive(length));
		}
	}
	else
	{
		Logger::log(INFO, "Client disconnected!", _clientFd);
		_ch.disconnectClient();
	}

	return (message);
}

std::string Server::peek() const
{
	char	buffer[BUFFER_SIZE];
	int		bytesRead;

	bytesRead = recv(_clientFd, buffer, sizeof(buffer) - 1, MSG_PEEK);
	buffer[bytesRead] = '\0';

	return (buffer);
}

std::string Server::receive(int length) const
{
	char	buffer[length + 1];
	int		bytesRead;

	bytesRead = recv(_clientFd, buffer, length, 0);
	buffer[bytesRead] = '\0';

	return (buffer);
}


/*
	Sending data
*/

void	Server::sendToClient(const std::string& message, int userFd) const
{
	Logger::log(INFO, "<<<<<<<", message);

	send(userFd, message.c_str(), message.length(), 0);
	send(userFd, CRLF, 2, 0);
}

/*
	Users and channels
*/
int		Server::getUserFd(std::string nickname) const
{
	for (clientsIt it = _users.begin(); it != _users.end(); it++)
	{
		if (Parser::toLower(it->second->getNickname()) == Parser::toLower(nickname))
			return (it->first);
	}
	return (0);
}

void	Server::removeUserFromChannel(const std::string& channelId, int userFd)
{
	Channel* channel = _channels.at(channelId);
	channel->removeUser(userFd);
	if (channel->isMembersEmpty())
		removeChannelFromServer(channelId);
}

void	Server::removeUserFromServer(int clientFd)
{
	if (clientFd == -1)
		clientFd = _clientFd;

	std::string channelId;
	User* user = _users.at(clientFd);
	for (User::channelsIt it = user->getChannelsBegin(); it != user->getChannelsEnd(); it = user->getChannelsBegin())
	{
		channelId = *it;
		removeUserFromChannel(channelId, clientFd);
	}
	Logger::log(DEBUG, "Deleting user", user->getNickname());
	delete user;
	_users.erase(clientFd);
}

void	Server::removeChannelFromServer(const std::string& channelId)
{
	Channel*	channel = _channels.at(channelId);
	
	if (!channel->isEmpty())
		channel->removeAllUsers();
	
	Logger::log(DEBUG, "Deleting channel", channelId);
	delete channel;
	_channels.erase(channelId);
}


/*
	Server API
*/

void	Server::sendReply(const std::string& replyMessage) const
{
	sendToClient(replyMessage, _clientFd);
}

void	Server::sendToAll(const std::string& message) const
{
	for (clientsIt it = _users.begin(); it != _users.end(); it++)	// TODO only already registerd users!
		sendToClient(message, it->first);
}

bool 	Server::isPasswordValid(const std::string& password) const
{
	return (_password == password);
}

bool	Server::doesNicknameExist(const std::string& nickname) const
{
	for (clientsIt it = _users.begin(); it != _users.end(); it++)
	{
		if(Parser::toLower(it->second->getNickname()) == Parser::toLower(nickname))
			return (true);
	}
	return (false);
}

bool	Server::doesChannelExist(const std::string& channelName) const
{
	std::string	channelId = Parser::toLower(channelName);

	for (channelsIt it = _channels.begin(); it != _channels.end(); it++)
	{
		if (it->first == channelId)
			return (true);
	}
	return (false);
}

void	Server::addChannel(const std::string& channelName)
{
	std::string channelId = Parser::toLower(channelName);
	_channels[channelId] = new Channel(channelName);
}

void	Server::disconnectClientFromServer(int clientFd)
{
	if (clientFd == -1)
		clientFd = _clientFd;

	unregisterFd(clientFd);
	close(clientFd);
	removeUserFromServer(clientFd);
}

void	Server::disconnectAllClientsFromServer()
{
	for(clientsIt it = _users.begin(); it != _users.end(); it = _users.begin())
		disconnectClientFromServer(it->first);
}


void	Server::sendToTarget(const std::string& message, std::string& nickname) const
{
	// Logger::log(DEBUG, "Sending message to user", message);
	int userFd = getUserFd(nickname);
	sendToClient(message, userFd);
}

void	Server::sendMessageToChannel(const std::string& message, const std::string& channelId) const
{
	std::vector<int> users = _channels.at(channelId)->getMembersIdList();
	for (std::vector<int>::const_iterator it = users.begin(); it != users.end(); it++)
	{
		if (*it != _clientFd)
			sendToClient(message, *it);
	}
}

void	Server::sendMessageToChannelsWhereUser(const std::string& message) const
{
	// User::channels clientChannels = getUserChannels(); // TODO
	User* user = _users.at(_clientFd);
	for (User::channelsIt it = user->getChannelsBegin(); it != user->getChannelsEnd(); it++)
	// for (User::channels::const_iterator it = clientChannels.begin(); it != clientChannels.end(); it++)
	{
		if (isChannelUser(*it))
			sendMessageToChannel(message, *it);
	}
}
