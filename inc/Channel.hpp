#pragma once
# include <iostream>
# include <string>
# include <map>
# include "User.hpp"
# include "Logger.hpp"
# include <vector>

enum Membership
{
	INVITEE,
	MEMBER,
	OPERATOR
};

class Channel
{
    private:
        const std::string		_name;
		std::string				_topic;
		std::string				_key; 					// (channel password)
		size_t					_user_limit;
		std::map<int, std::pair<User*, Membership> >	_users;	//
		bool					_mode_invite_only;		// i: set/remove Invite-only channel
		bool					_mode_topic_restricted; // t: set/remove the restrictions of the topic command to ch-ops
		bool					_mode_has_key;			// k: set/remove the channel key (password)
		bool					_mode_has_limit;		// l: set/remove the userlimit
														// o: give/take chop privilege

		size_t					getNumberUsers() const;
	
    public:
		//Special Users
        Channel(const std::string& name);
        Channel(const Channel& other);
        Channel& operator=(const Channel& other);

		// Getters
		const std::string&			getName() const;
		const std::string&			getTopic() const;
		bool						isTopicSet() const;
		bool						isKeyValid(const std::string& key) const;
		size_t						getUserLimit() const;
		const std::string			getUsersList(int fd) const;
		const std::vector<int>		getUsers() const;
		bool						isInviteOnly() const;
		bool						isTopicRestricted() const;
		bool						needsChannelKey() const;
		bool						hasUserLimit() const;
		bool						isFull() const;
		bool						isEmpty() const;
	
		// Setters
		void						setTopic(const std::string& topic);
		void						setInviteOnly(bool state);
		void						setTopicRestricted(bool state);
		void						setKey(const std::string&  key);
		void						clearKey();
		void						setUserLimit(size_t limit);
		void						clearUserLimit();
		void						setHasLimit(bool state);

	
		// Membership
		void						addMember(int userFd, User* user);
		bool						isMember(int userFd);
	
		// Operators
		void						promoteMember(int userFd);
		void						demoteOperator(int userFd);
		bool						isOperator(int userFd);
	
		// Invitations
		void						addInvitee(int userFd, User* user);
		void						promoteInvitee(int userFd);
		bool						isInvitee(int userFd);

		void						removeUser(int userFd);
		
		class hashSymbolException : public std::exception
		{
			public:
				const char *what() const throw();
		};
};
