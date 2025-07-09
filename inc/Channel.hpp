#pragma once
# include "User.hpp"
# include <map>
# include <vector>
# include <string>

enum Membership
{
	INVITEE,
	MEMBER,
	OPERATOR
};

class Channel
{
	private:
		typedef	std::map<int, std::pair<User*, Membership> >	channelUsers;
		typedef channelUsers::const_iterator	usersIt;

		const std::string	_name;
		channelUsers		_users;
		std::string			_key;			// k: set/remove key
		size_t				_userLimit;		// l: set/remove limit
		std::string			_topic;
		bool				_inviteMode;	// i: set/remove Invite-only channel
		bool				_topicMode;		// t: set/remove the restrictions of the topic command to ch-ops
		 									// o: give/take chop privilege

		size_t					getNumberMembers() const;
	
	public:		
		Channel(const std::string& name);
		~Channel();


		// Getters
		const std::string&			getName() const;
		const std::vector<int>		getMembersIdList() const;
		const std::string			getMembersNicknamesList(int fd) const;

		const std::string&			getKey() const;
		size_t						getUserLimit() const;
		const std::string&			getTopic() const;

		bool						isInviteMode() const;
		bool						isKeyMode() const;
		bool						isLimitMode() const;
		bool						isTopicMode() const;

		bool						isKeyValid(const std::string& key) const;
		bool						isMembersFull() const;
		bool						isMembersEmpty() const;
		bool						isEmpty() const;
		bool						isTopicSet() const;


		// Setters
		void						setKey(const std::string&  key);	
		void						setUserLimit(size_t limit);
		void						setTopic(const std::string& topic);
		void						clearKey();

		void						setInviteMode(bool state);
		void						setTopicMode(bool state);


		// Membership
		void						addMember(int userFd, User* user);
		bool						isUserMember(int userFd) const;
		void						removeUser(int userFd);
		void						removeAllUsers();


		// Operators
		void						promoteMember(int userFd);
		bool						isUserOperator(int userFd) const;
		void						demoteOperator(int userFd);

		// Invitations
		void						addInvitee(int userFd, User* user);
		bool						isChannelInvitee(int userFd) const;
		void						promoteInvitee(int userFd);
};
