#ifndef PARSER_HPP
# define PARSER_HPP
# include <string>
# include <vector>
# define CRLF		"\r\n"

typedef std::vector<std::string>::iterator paramsIt;

typedef struct parsed_message
{
	std::string					prefix;
	std::string					command;
	std::vector<std::string>	params;
//	std::string					trailing;
	
	paramsIt getParamsBegin()
	{
		return params.begin();
	}

	paramsIt getParamsEnd()
	{
		return params.end();
	}
}	parsed_message;

class Parser
{
    private:
		std::string::iterator		_it;

		bool		is_message(parsed_message& parsed_msg);
		bool		is_colon();
		bool		is_prefix(parsed_message& parsed_msg);
		bool		is_servername();
		bool		is_nickname();
		bool		is_space();
		bool		is_command(parsed_message& parsed_msg);
		bool		is_params(parsed_message& parsed_msg);
		bool		is_middle(parsed_message& parsed_msg);
		bool		is_nospcrlfcl();
		bool		is_null();
		bool		is_cr();
		bool		is_lf();
		bool		is_crlf();
		bool		is_trailing(parsed_message& parsed_msg);

		static bool	is_special(std::string::iterator it);
		static bool	is_chstring(std::string::iterator it);

    public:
        Parser();
        ~Parser();

		bool							is_partial(std::string& message);
		int								get_message_length(std::string& message);
		bool							parse_message(std::string& message, parsed_message& parsed_msg);
		static bool						is_nickname(std::string& nickname);
		static bool						is_channel(std::string& channel);
		static std::vector<std::string>	splitParam(const std::string& msgtarget, char c);
};

#endif
