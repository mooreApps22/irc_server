#ifndef PARSER_HPP
# define PARSER_HPP
# include <string>
#include <vector>
#define CRLF		"\r\n"

class Parser
{
    private:
		std::string::iterator		_it;
		std::string					_prefix;
		std::string					_command;
		std::vector<std::string>	_params;
		std::string					_trailing;

		bool is_message();
		bool is_colon();
		bool is_prefix();
		bool is_servername();
		bool is_nickname();
		bool is_space();
		bool is_command();
		bool is_params();
		bool is_middle();
		bool is_nospcrlfcl();
		bool is_null();
		bool is_cr();
		bool is_lf();
		bool is_crlf();
		bool is_trailing();

    public:
        Parser();
        ~Parser();

		bool			is_partial(std::string& message);
		int				get_message_length(std::string& message);
		bool			parse_message(std::string& message);
		std::string&	get_command(void);

		std::string::iterator	_end; // Delete
		void	print(); // Delete
};

#endif
