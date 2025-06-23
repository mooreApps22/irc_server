#ifndef USER_HPP
# define USER_HPP
# include <string>
# include <ctime>

class User
{
    private:
		std::string	_nickname;
		std::string	_username;
		std::string	_host;
		bool		_registered;
		bool		_password;
		bool		_invisible;
		std::string _partial_message;
		time_t		_last_contact;

    public:
        User(const std::string& host);
        ~User();

		void				setNickname(const std::string& nickname);
		const std::string&	getNickname() const;
		void				setUsername(const std::string& nickname);
		const std::string	getIdentifier() const;
		void				setRegisteredStatus(const bool status);
		bool				isRegistered() const;
		void				setInvisibleStatus(const bool status);
		bool				isInvisible() const;
		void				buffer(const std::string& message);
		const std::string	get_partial_message();
		bool				getPasswordStatus() const;
		void				setPasswordStatus(const bool state);
		time_t				getLastContact(void);
		void				setLastContact(time_t now);
};

#endif
