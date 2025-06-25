#pragma once
# include <iostream>
# include <string>
# include <map>
# include "User.hpp"
# include "Logger.hpp"

class Channel
{
    private:
        const std::string						_name;
		std::string								_topic;
		std::string								_key; // (channel password)
		size_t									_user_limit;
		std::map<std::string, User*>			_members;
		std::map<std::string, User*>			_operators;
		std::map<std::string, User*>			_invitees;
		bool									_mode_invite_only; //			i: set/remove Invite-only channel
		bool									_mode_topic_restricted; //		t: set/remove the restrictions of the topic command to ch-ops
		bool									_mode_has_key; //				k: set/remove the channel key (password)
		bool									_mode_has_limit; //				l: set/remove the userlimit
																//				o: give/take chop privilege
    public:
		//Special Users
        Channel(const std::string& name);
        Channel(const Channel& other);
        Channel& operator=(const Channel& other);
		// Getters
		const std::string								getName() const;
		std::string										getTopic() const;
		std::string										getKey() const;
		size_t											getUserLimit() const;
		const std::map<std::string, User*>&				getUsers() const;
		const std::map<std::string, User*>&				getChops() const;
		const std::map<std::string, User*>&				getInvitees() const;
		bool											isInviteOnly() const;
		bool											isTopicRestricted() const;
		bool											needsChannelKey() const;
		bool											hasUserLimit() const;
		bool											isFull() const;
		// Setters
		void											setTopic(const std::string& topic);
		void											setInviteOnly(bool state);
		void											setTopicRestricted(bool state);
		void											setKey(const std::string&  key);
		void											clearKey();
		void											setUserLimit(size_t limit);
		void											clearUserLimit();
		// Usership
		void											addUser(User* user, const std::string& name);
		void											removeUser(const std::string& name);
		bool											isUser(const std::string& name);
		// Operators
		void											addChop(User* user, const std::string& name);
		void											removeChop(const std::string& name);
		bool											isChop(const std::string& name);
		// Invitations
		void											addInvitee(User* user, const std::string& name);
		void											removeInvitee(const std::string& name);
		bool											isInvitee(const std::string& name);

		class hashSymbolException : public std::exception
		{
			public:
				const char *what() const throw();
		};
};
