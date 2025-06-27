#pragma once
# include <iostream>
# include <string>
# include <map>
# include "User.hpp"
# include "Logger.hpp"

class Channel
{
    private:
        const std::string		_name;
		std::string				_topic;
		std::string				_key; 					// (channel password)
		size_t					_user_limit;
		std::map<int, User*>	_members;
		std::map<int, User*>	_operators;
		std::map<int, User*>	_invitees;
		bool					_mode_invite_only;		// i: set/remove Invite-only channel
		bool					_mode_topic_restricted; // t: set/remove the restrictions of the topic command to ch-ops
		bool					_mode_has_key;			// k: set/remove the channel key (password)
		bool					_mode_has_limit;		// l: set/remove the userlimit
														// o: give/take chop privilege
    public:
		//Special Users
        Channel(const std::string& name);
        Channel(const Channel& other);
        Channel& operator=(const Channel& other);

		// Getters
		const std::string			getName() const;
		const std::string&			getTopic() const;
		bool						isTopicSet() const;
		bool						isKeyValid(const std::string& key) const;
		size_t						getUserLimit() const;
		const std::string			getUsersList() const;
		const std::map<int, User*>&	getMembers() const;
		const std::map<int, User*>&	getChops() const;
		const std::map<int, User*>&	getInvitees() const;
		bool						isInviteOnly() const;
		bool						isTopicRestricted() const;
		bool						needsChannelKey() const;
		bool						hasUserLimit() const;
		bool						isFull() const;
	
		// Setters
		void						setTopic(const std::string& topic);
		void						setInviteOnly(bool state);
		void						setTopicRestricted(bool state);
		void						setKey(const std::string&  key);
		void						clearKey();
		void						setUserLimit(size_t limit);
		void						clearUserLimit();
	
		// Membership
		void						addMember(int userFd, User* user);
		void						removeMember(int userFd);
		bool						isMember(int userFd);
	
		// Operators
		void						addOperator(int userFd, User* user);
		void						removeOperator(int userFd);
		bool						isOperator(int userFd);
	
		// Invitations
		void						addInvitee(int userFd, User* user);
		void						removeInvitee(int userFd);
		bool						isInvitee(int userFd);


		class hashSymbolException : public std::exception
		{
			public:
				const char *what() const throw();
		};
};
