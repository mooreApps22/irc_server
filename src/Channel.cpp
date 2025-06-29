#include "../inc/Channel.hpp"
#include "../inc/User.hpp"
#include <string>
#include <iostream>
#include <map>

Channel::Channel(const std::string& name)
	:	_name(name),
		_topic(""),
		_key(""),
		_user_limit(0),
		_mode_invite_only(false),
		_mode_topic_restricted(false),
		_mode_has_key(false),
		_mode_has_limit(false)
{
	Logger::log(INFO, "Channel has been created.)", _name);
	if (_name[0] != '#')
		throw hashSymbolException();
}

Channel::Channel(const Channel& other) : _name(other._name)
{
	*this = other;
}

Channel& Channel::operator=(const Channel& other)
{
	if (this != &other)
	{
		_topic = other._topic;
		_key = other._key;
		_user_limit = other._user_limit;
		_mode_invite_only = other._mode_invite_only;
		_mode_topic_restricted = other._mode_topic_restricted;
		_mode_has_key = other._mode_has_key;
		_mode_has_limit = other._mode_has_limit;
	}
	return (*this);
}

// Getters
const std::string	Channel::getName() const
{
	return(_name);	
}

const std::string&	Channel::getTopic() const
{
	return(_topic);	
}

bool	Channel::isTopicSet() const
{
	return (_topic.size() > 0);
}

/*
std::string	Channel::getKey() const
{
	return(_key);	
}
*/

bool	Channel::isKeyValid(const std::string& key) const
{
	return (_key == key);
}

size_t	Channel::getUserLimit() const
{
	return(_user_limit);	
}

const std::string	Channel::getUsersList(int fd) const
{
	std::string	list = "";
	std::string	user = "";

	for(std::map<int, std::pair<User*, Membership> >::const_iterator it = _users.begin(); it != _users.end(); it++)
	{
		if (it->first != fd && it->second.second > INVITEE)
		{
			list += " ";
			if (it->second.second == OPERATOR)
				list += "@";
			list += it->second.first->getNickname();
		}
		else
		{
			if (it->second.second == OPERATOR)
				user+= "@";
			user += it->second.first->getNickname();
		}
	}	
	return user + list;
}

const std::vector<int>	Channel::getUsers() const
{
	std::vector<int>	users;
	for(std::map<int, std::pair<User*, Membership> >::const_iterator it = _users.begin(); it != _users.end(); it++)
	{
		if(it->second.second > INVITEE)
			users.push_back(it->first);
	}
	return (users);
}

bool	Channel::isInviteOnly() const
{
	return (_mode_invite_only);	
}

bool	Channel::isTopicRestricted() const
{
	return (_mode_topic_restricted);	
}

bool	Channel::needsChannelKey() const
{
	return (_mode_has_key);	
}

bool	Channel::hasUserLimit() const
{
	return (_mode_has_limit);	
}

bool	Channel::isFull() const
{
	size_t count = 0;
	for(std::map<int, std::pair<User*, Membership> >::const_iterator it = _users.begin(); it != _users.end(); it++)
	{
		if (it->second.second > INVITEE)
			count++;
	}
	return (count > 0 && count >= _user_limit);
}

// Setters
void	Channel::setTopic(const std::string& topic)
{
	Logger::log(DEBUG, "is setTopic() being called?");
	_topic = topic;
}

void	Channel::setInviteOnly(bool state)
{
	_mode_invite_only = state;
}

void	Channel::setTopicRestricted(bool state)
{
	_mode_topic_restricted = state;
}

void	Channel::setKey(const std::string& key)
{
	_mode_has_key = true;
	_key = key;
}

void	Channel::clearKey()
{
	_mode_has_key = false;
	_key.clear();
}

void	Channel::setUserLimit(size_t limit)
{
	_mode_has_limit = true;	
	_user_limit = limit;
}

void	Channel::clearUserLimit()
{
	_mode_has_limit = false;	
	_user_limit = 0;
}

// Membership
void	Channel::addMember(int user_fd, User* user)
{
	_users[user_fd] = std::pair<User*, Membership>(user, MEMBER);
}

bool	Channel::isMember(int userFd)
{
	std::map<int, std::pair<User*, Membership> >::iterator it = _users.find(userFd);
	
	if (it == _users.end())
		return (false);
	else
		return (it->second.second == MEMBER);
}

// Operators
void	Channel::promoteMember(int userFd)
{
	_users[userFd].second = OPERATOR;
}

void	Channel::demoteOperator(int userFd)
{
	_users[userFd].second = MEMBER;
}

bool	Channel::isOperator(int userFd)
{
	std::map<int, std::pair<User*, Membership> >::iterator it = _users.find(userFd);
	
	if (it == _users.end())
		return (false);
	else
		return (it->second.second == OPERATOR);
}

// Invitations
void	Channel::addInvitee(int userFd, User* user)
{
	_users[userFd] = std::pair<User*, Membership>(user, INVITEE);
}

void	Channel::promoteInvitee(int userFd)
{
	_users[userFd].second = MEMBER;
}

bool	Channel::isInvitee(int userFd)
{
	std::map<int, std::pair<User*, Membership> >::iterator it = _users.find(userFd);
	
	if (it == _users.end())
		return (false);
	else
		return (it->second.second == INVITEE);
}

void	Channel::removeUser(int userFd)
{
	_users.erase(userFd);
}

const char*	Channel::hashSymbolException::what() const throw()
{
	return ("Channel names have to begin with a '#'!");
}

void	Channel::setHasLimit(bool state)
{
	_mode_has_limit = state;
}
