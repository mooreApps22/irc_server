#ifndef USER_HPP
# define USER_HPP
# include <string>

class User
{
    private:
		std::string	_nickname;	
		bool		_registered;

    public:
        User();
        ~User();

		void				set_nickname(const std::string& nickname);
		const std::string&	get_nickname();
		bool				is_registered();

};

#endif