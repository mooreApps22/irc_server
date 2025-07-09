#pragma once
# include <string>
# include <set>

enum Registation
{
	CONNECTED,
	PASS_GIVEN,
	NICK_GIVEN,
	REGISTERED
};

class User
{	
	private:
		typedef	std::set<std::string>		channels;
		std::string		_nickname;
		std::string		_username;
		std::string		_host;
		Registation		_registrationState;
		bool			_invisibleMode;
		std::string		_partialMessage;
		channels		_channels;

	public:
		typedef channels::const_iterator	channelsIt;
	
		User(const std::string& host);
		~User();

		// Setters
		void				setNickname(const std::string& nickname);
		void				setUsername(const std::string& nickname);
		void				setPasswordGivenStatus();
		void				setNicknameGivenStatus();
		void				setRegisteredStatus();
		void				setInvisibleMode(const bool status);

		// Getters
		const std::string&	getNickname() const;
		const std::string&	getHost() const;
		const std::string	getIdentifier() const;
		bool				hasGivenPassword() const;
		bool				hasGivenNickname() const;
		bool				isRegistered() const;
		// bool				isInvisibleMode() const;
		channelsIt			getChannelsBegin() const;
		channelsIt			getChannelsEnd() const;

		// Messages
		void				buffer(const std::string& message);
		const std::string	getPartialMessage();
		
		// Channels
		void				addChannel(const std::string& channelId);
		void				removeChannel(const std::string& channelId);
};
