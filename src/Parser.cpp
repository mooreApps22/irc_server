#include "Parser.hpp"
#include "Logger.hpp"
#include <iostream>

Parser::Parser()
{
	Logger::log(INFO, "Parser Constructor called.");
}

Parser::~Parser()
{
	Logger::log(INFO, "Parser Destructor called.");
}

bool Parser::is_partial(std::string& message)
{
	return message.find(CRLF) == std::string::npos;
}

int Parser::get_message_length(std::string& message)
{
	return message.find(CRLF) + 2;
}

bool Parser::parse_message(std::string& message, parsed_message& parsed_msg)
{
	std::string::iterator begin = message.begin();
	_it = begin;

	Logger::log(DEBUG, "Parsing", message);
	return is_message(parsed_msg);
}

// message    =  [ ":" prefix SPACE ] command [ params ] crlf
bool Parser::is_message(parsed_message& parsed_msg)
{
	std::string::iterator begin = _it;

	if (is_colon())
	{
		if (!is_prefix(parsed_msg))
		 	return false;
		if (!is_space())
			return false;
	}
	else
		_it = begin;

	if (!is_command(parsed_msg))
		return false;
	
	begin = _it;
	if (!is_params(parsed_msg))
		_it = begin;
	
	if (!is_crlf())
		return false;
	return true;
}

bool Parser::is_colon()
{
	return *_it++ == ':';
}

// prefix     =  servername / ( nickname [ [ "!" user ] "@" host ] )
bool Parser::is_prefix(parsed_message& parsed_msg)
{
	(void) parsed_msg;
	return false;
}

// SPACE      =  %x20        ; space character
bool Parser::is_space()
{
	return *_it++ == ' ';
}

// command    =  1*letter / 3digit ; 1 or more letter OR eactly 3 digit
bool Parser::is_command(parsed_message& parsed_msg)
{
	std::string::iterator begin = _it;

	while(std::isalpha(*_it))
		_it++;
	if (_it > begin)
	{
		std::string command(begin, _it);
		 parsed_msg.command = command;
		// Logger::log(DEBUG, "Command",  parsed_msg.command);
		return true;
	}
	
	while(std::isdigit(*_it))
		_it++;

	if (_it > begin)
	{
		std::string command(begin, _it++);
		 parsed_msg.command = command;
		// Logger::log(DEBUG, "Command",  parsed_msg.command);
		if ( parsed_msg.command.length() == 3)
		{
			return true;
		}
		else
			return false;
	}
	return false;
}

// params     =  *14( SPACE middle ) [ SPACE ":" trailing ]
//            =/ 14( SPACE middle ) [ SPACE [ ":" ] trailing ]
bool Parser::is_params(parsed_message& parsed_msg)
{
	std::string::iterator begin;
	std::string::iterator backtrack;
	int i;
	
	for(i = 0; i < 14; i++)
	{
		begin = _it;
		if (!is_space())
		{
			_it = begin;
			break;
		}
		if (!is_middle(parsed_msg))
		{
			_it = begin;
			break;
		}
	}

	begin = _it;
	if (!is_space())
	{
		_it = begin;
		return true;
	}
	if ( parsed_msg.params.size() < 14)
	{
		if (!is_colon())
		{
			_it = begin;
			return true;
		}
	}
	else
	{
		backtrack = _it;
		if (!is_colon())
			_it = backtrack;
	}

	backtrack = _it;
	if (!is_trailing(parsed_msg))
		_it = begin;

	return true;
}

// middle     =  nospcrlfcl *( ":" / nospcrlfcl )
bool Parser::is_middle(parsed_message& parsed_msg)
{
	std::string::iterator begin = _it;
	std::string::iterator checkpoint;

	if (!is_nospcrlfcl())
		return false;

	while (true)
	{
		checkpoint = _it;
		if (!is_colon())
		{
			_it = checkpoint;
			if (!is_nospcrlfcl())
			{
				_it = checkpoint;
				break;
			}
		}
	}
	std::string param(begin, _it);
	 parsed_msg.params.push_back(param);
	// Logger::log(DEBUG, "Param", param);
	return true;
}

// nospcrlfcl =  %x01-09 / %x0B-0C / %x0E-1F / %x21-39 / %x3B-FF ; any octet except NUL, CR, LF, " " and ":"
bool Parser::is_nospcrlfcl()
{
	std::string::iterator begin = _it;
	if (is_null())
		return false;
	_it = begin;

	if (is_cr())
		return false;
	_it = begin;

	if (is_lf())
		return false;
	_it = begin;

	if (is_space())
		return false;
	_it = begin;

	return (!is_colon());
}

bool Parser::is_null()
{
	return *_it++ == '\0';
}

bool Parser::is_cr()
{
	return *_it++ == '\r';
}

bool Parser::is_lf()
{
	return *_it++ == '\n';
}

// crlf       =  %x0D %x0A   ; "carriage return" "linefeed"
bool Parser::is_crlf()
{
	return is_cr() && is_lf();
}

bool Parser::is_servername()
{
	return false;
}

bool Parser::is_nickname()
{
	return false;
}

// trailing   =  *( ":" / " " / nospcrlfcl ) ; 0 or more
bool Parser::is_trailing(parsed_message& parsed_msg)
{
	std::string::iterator begin = _it;
	std::string::iterator backtrack;

	while(true)
	{
		backtrack = _it;
		if (!is_colon())
			_it= backtrack;
		else
			continue;
		if (!is_space())
			_it = backtrack;
		else
			continue;
		if (!is_nospcrlfcl())
		{
			_it = backtrack;
			break;
		}
	}

	std::string trailing(begin, _it);

//	 parsed_msg.trailing = trailing;
	 parsed_msg.params.push_back(trailing);
	// Logger::log(DEBUG, "Trailing", trailing);
	return true;
}

// nickname   =  ( letter / special ) *8( letter / digit / special / "-" )
// letter     =  %x41-5A / %x61-7A       ; A-Z / a-z
// digit      =  %x30-39                 ; 0-9
// special    =  %x5B-60 / %x7B-7D		 ; "[", "]", "\", "`", "_", "^", "{", "|", "}"
bool	Parser::is_nickname(std::string& nickname)
{
	std::string::iterator it = nickname.begin();
	if (!std::isalpha(*it) && !Parser::is_special(it))
		return false;
	it++;
	for(int i = 0; it != nickname.end(); i++, it++)
	{
		if (i == 8)
			return false;
		if(!std::isalnum(*it) && !Parser::is_special(it) && *it != '-')
			return false;
	}	
	return true;
}

bool	Parser::is_special(std::string::iterator it)
{
	return *it == '[' || *it == ']' || *it == '\\' || *it == '`' || *it == '_' || *it == '^' || *it == '{' || *it == '|' || *it == '}';
}
