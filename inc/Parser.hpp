#ifndef PARSER_HPP
# define PARSER_HPP
# include <string>

class Parser
{
    private:
		std::string	_prefix;
		std::string	_command;
		std::string	_params;

    public:
        Parser();
        ~Parser();

		bool	is_partial(std::string& message);
		bool	get_message_length(std::string& message);
		bool	parse_message(std::string& message);
};

#endif