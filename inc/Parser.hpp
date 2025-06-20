#ifndef PARSER_HPP
# define PARSER_HPP
# include <string>
#include <vector>
#define CRLF		"\r\n"

typedef std::vector<std::string>::iterator paramsIt;
typedef struct parsed_message
{
	std::string					prefix;
	std::string					command;
	std::vector<std::string>	params;
//	std::string					trailing;
	paramsIt getParams()
	{
		return params.begin();
	}
}	parsed_message;

class Parser
{
    private:
		std::string::iterator		_it;
		// std::string					_prefix;
		// std::string					_command;
		// std::vector<std::string>	_params;
		// std::string					_trailing;

		bool is_message(parsed_message& parsed_msg);
		bool is_colon();
		bool is_prefix(parsed_message& parsed_msg);
		bool is_servername();
		bool is_nickname();
		bool is_space();
		bool is_command(parsed_message& parsed_msg);
		bool is_params(parsed_message& parsed_msg);
		bool is_middle(parsed_message& parsed_msg);
		bool is_nospcrlfcl();
		bool is_null();
		bool is_cr();
		bool is_lf();
		bool is_crlf();
		bool is_trailing(parsed_message& parsed_msg);

    public:
        Parser();
        ~Parser();

		bool			is_partial(std::string& message);
		int				get_message_length(std::string& message);
		bool			parse_message(std::string& message, parsed_message& parsed_msg);
		std::string&	get_command(void);

		std::string::iterator	_end; // Delete
		void	print(); // Delete
};

#endif
