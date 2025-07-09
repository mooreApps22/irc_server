#include "Channel.hpp"
#include "Logger.hpp"
#include "Parser.hpp"
#include <string>


Channel::Channel(const std::string& name)
	:	_name(name),
		_key(""),
		_userLimit(0),
		_topic(""),
		_inviteMode(false),
		_topicMode(false)
{
	// Logger::log(INFO, "Channel has been created.)", _name);
}

Channel::~Channel()
{

}


/*
	Getters
*/

const std::string&	Channel::getName() const
{
	return (_name);	
}

const std::vector<int>	Channel::getMembersIdList() const
{
	std::vector<int>	users;
	for(usersIt it = _users.begin(); it != _users.end(); it++)
	{
		if(it->second.second > INVITEE)
			users.push_back(it->first);
	}
	return (users);
}

const std::string	Channel::getMembersNicknamesList(int fd) const
{
	std::string	list = "";
	std::string	user = "";

	for(usersIt it = _users.begin(); it != _users.end(); it++)
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
	return (user + list);
}

const std::string&	Channel::getKey() const
{
	return (_key);	
}

size_t	Channel::getUserLimit() const
{
	return (_userLimit);	
}

const std::string&	Channel::getTopic() const
{
	return (_topic);
}

bool	Channel::isInviteMode() const
{
	return (_inviteMode);	
}

bool	Channel::isKeyMode() const
{
	return (!_key.empty());
}

bool	Channel::isLimitMode() const
{
	return (_userLimit > 0);	
}

bool	Channel::isTopicMode() const
{
	return (_topicMode);	
}

bool	Channel::isKeyValid(const std::string& key) const
{
	return (_key == key);
}

bool	Channel::isMembersFull() const
{
	size_t count = getNumberMembers();
	return (count > 0 && count >= _userLimit);
}

bool	Channel::isMembersEmpty() const
{
	return (getNumberMembers() == 0);
}

bool	Channel::isEmpty() const
{
	return (_users.empty());
}

bool	Channel::isTopicSet() const
{
	return (!_topic.empty());
}

size_t	Channel::getNumberMembers() const
{
	size_t count = 0;
	for(usersIt it = _users.begin(); it != _users.end(); it++)
	{
		if (it->second.second > INVITEE)
			count++;
	}
	return (count);
}


/*
	Setters
*/

void	Channel::setKey(const std::string& key)
{
	_key = key;
}

void	Channel::setUserLimit(size_t limit)
{
	_userLimit = limit;
}

void	Channel::setTopic(const std::string& topic)
{
	// Logger::log(DEBUG, "is setTopic() being called?");
	_topic = topic;
}

void	Channel::clearKey()
{
	_key.clear();
}

void	Channel::setInviteMode(bool state)
{
	_inviteMode = state;
}

void	Channel::setTopicMode(bool state)
{
	_topicMode = state;
}

/*
	Membership
*/

void	Channel::addMember(int userFd, User* user)
{
	_users[userFd] = std::pair<User*, Membership>(user, MEMBER);
	user->addChannel(Parser::toLower(_name));
}

bool	Channel::isUserMember(int userFd) const
{
	usersIt	it = _users.find(userFd);
	
	if (it == _users.end())
		return (false);
	else
		return (it->second.second == MEMBER);
}


/*
	Operators
*/

void	Channel::promoteMember(int userFd)
{
	_users.at(userFd).second = OPERATOR;
}

bool	Channel::isUserOperator(int userFd) const
{
	usersIt	it = _users.find(userFd);
	
	if (it == _users.end())
		return (false);
	else
		return (it->second.second == OPERATOR);
}

void	Channel::demoteOperator(int userFd)
{
	_users.at(userFd).second = MEMBER;
}

/*
	Invitations
*/

void	Channel::addInvitee(int userFd, User* user)
{
	_users[userFd] = std::pair<User*, Membership>(user, INVITEE);
	user->addChannel(Parser::toLower(_name));
}

bool	Channel::isChannelInvitee(int userFd) const
{
	usersIt it = _users.find(userFd);

	if (it == _users.end())
		return (false);
	else
		return (it->second.second == INVITEE);
}

void	Channel::promoteInvitee(int userFd)
{
	_users.at(userFd).second = MEMBER;
}

void	Channel::removeUser(int userFd)
{
	_users.at(userFd).first->removeChannel(Parser::toLower(_name));
	_users.erase(userFd);
}

void	Channel::removeAllUsers()
{
	for(usersIt it = _users.begin(); it != _users.end(); it = _users.begin())
	{
		removeUser(it->first);
	}
}

