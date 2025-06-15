#ifndef USER_HPP
# define USER_HPP
# include <string>

class User
{
    private:
		const std::string	_nickname;	
		bool				_password;

    public:
        User();
        ~User();
		
		void	run(void);
};

#endif