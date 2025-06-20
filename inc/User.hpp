#ifndef USER_HPP
# define USER_HPP
# include <string>

class User
{
    private:
		std::string	_nickname;	
		bool		_registered;
		bool		_password;
		std::string _partial_message;

    public:
        User();
        ~User();

		void				setNickname(const std::string& nickname);
		const std::string&	getNickname() const;
		void				setRegisteredStatus(const bool status);
		bool				isRegistered() const;
		void				buffer(const std::string& message);
		const std::string	get_partial_message();
		bool				getPasswordStatus() const;
		void				setPasswordStatus(const bool state);
};

#endif
