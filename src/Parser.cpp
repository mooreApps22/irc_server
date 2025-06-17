#include "Parser.hpp"
#include "Logger.hpp"
#include <iostream>

Parser::Parser()
	:	_prefix(""),
		_command("")
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

bool Parser::parse_message(std::string& message)
{
	std::string::iterator begin = message.begin();
	_it = begin;

	_prefix.empty();
	_command.empty();
	_params.empty();
	_trailing.empty();
	
	Logger::log(DEBUG, "Parsing", message);
	// _end = message.end(); // Delete
	return is_message();
}

// message    =  [ ":" prefix SPACE ] command [ params ] crlf
bool Parser::is_message()
{
	std::string::iterator begin = _it;

	if (is_colon())
	{
		if (!is_prefix())
		 	return false;
		if (!is_space())
			return false;
	}
	else
		_it = begin;

	if (!is_command())
		return false;
	
	begin = _it;
	if (!is_params())
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
bool Parser::is_prefix()
{
	return false;
}

// SPACE      =  %x20        ; space character
bool Parser::is_space()
{
	return *_it++ == ' ';
}

// command    =  1*letter / 3digit ; 1 or more letter OR eactly 3 digit
bool Parser::is_command()
{
	std::string::iterator begin = _it;

	while(std::isalpha(*_it))
		_it++;
	if (_it > begin)
	{
		std::string command(begin, _it);
		_command = command;
		Logger::log(DEBUG, "Command", _command);
		return true;
	}
	
	while(std::isdigit(*_it))
		_it++;

	if (_it > begin)
	{
		std::string command(begin, _it++);
		_command = command;
		Logger::log(DEBUG, "Command", _command);
		if (_command.length() == 3)
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
bool Parser::is_params()
{
	std::string::iterator begin;
	std::string::iterator backtrack;
	int i;
	
	_params.clear();

	for(i = 0; i < 14; i++)
	{
		begin = _it;
		if (!is_space())
		{
			_it = begin;
			break;
		}
		if (!is_middle())
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
	if (_params.size() < 14)
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
	if (!is_trailing())
		_it = begin;

	return true;
}

// middle     =  nospcrlfcl *( ":" / nospcrlfcl )
bool Parser::is_middle()
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
	_params.push_back(param);
	Logger::log(DEBUG, "Param", param);
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
bool Parser::is_trailing()
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
	_trailing = trailing;
	Logger::log(DEBUG, "Trailing", _trailing);
	return true;
}

std::string&	Parser::get_command(void)
{
	return (_command);
}

