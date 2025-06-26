#include "../inc/Channel.hpp"
#include "../inc/User.hpp"
#include <string>
#include <iostream>
#include <map>

Channel::Channel(const std::string& name)
	:	_name(name),
		_topic("This channels topic has not been set."),
		_key("This channels key has not been set"),
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

std::string	Channel::getTopic() const
{
	return(_topic);	
}

std::string	Channel::getKey() const
{
	return(_key);	
}
size_t	Channel::getUserLimit() const
{
	return(_user_limit);	
}

const std::map<std::string, User*>&	Channel::getMembers() const
{
	return(_members);	
}

const std::map<std::string, User*>&	Channel::getChops() const
{
	return(_operators);	
}

const std::map<std::string, User*>&	Channel::getInvitees() const
{
	return(_invitees);	
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
	return (_members.size() + _operators.size() == _user_limit);
}

// Setters
void	Channel::setTopic(const std::string& topic)
{
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

void	Channel::setKey(const std::string&  key)
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
// void	Channel::addUser(User* user, const std::string& name)
// {
// 	_members.insert(std::pair<std::string, User*>(name, user));
// }

void	Channel::addUser(int user_fd, User* user)
{
	_membersFd.insert(std::pair<int, User*>(user_fd, user));
}

void	Channel::removeUser(const std::string& name)
{
	_members.erase(name);
}

bool	Channel::isUser(const std::string& name)
{
	return (_members.find(name) != _members.end());
}

// Operators
void	Channel::addChop(User* user, const std::string& name)
{
	_operators.insert(std::pair<std::string, User*>(name, user));
}

void	Channel::removeChop(const std::string& name)
{
	_operators.erase(name);
}

bool	Channel::isChop(const std::string& name)
{
	return (_operators.find(name) != _operators.end());
}

// Invitations
void	Channel::addInvitee(User* user, const std::string& name)
{
	_invitees.insert(std::pair<std::string, User*>(name, user));
}

void	Channel::removeInvitee(const std::string& name)
{
	_invitees.erase(name);
}

bool	Channel::isInvitee(const std::string& name)
{
	return (_invitees.find(name) != _invitees.end());
}

const char*	Channel::hashSymbolException::what() const throw()
{
	return ("Channel names have to begin with a '#'!");
}

