#include "Parser.hpp"
#include "Logger.hpp"
#include "macros.hpp"
#include <string>

Parser::Parser()
{
	// Logger::log(INFO, "Parser Constructor called.");
}

Parser::~Parser()
{
	// Logger::log(INFO, "Parser Destructor called.");
}

bool Parser::parseMessage(std::string& message, parsedMessage& parsedMsg)
{
	std::string::iterator begin = message.begin();
	_it = begin;

	std::string messageSrippedCRLF(message.begin(), message.end() - 2);
	Logger::log(INFO, ">>>>>>>", messageSrippedCRLF);
	return isMessage(parsedMsg);
}

/*
	Message grammar
*/

// message	=  [ ":" prefix SPACE ] command [ params ] crlf
bool Parser::isMessage(parsedMessage& parsedMsg)
{
	std::string::iterator begin = _it;

	if (isColon())
	{
		if (!isPrefix(parsedMsg))
		 	return false;
		if (!isSpace())
			return false;
	}
	else
		_it = begin;

	if (!isCommand(parsedMsg))
		return false;

	begin = _it;
	if (!isParams(parsedMsg))
		_it = begin;
	
// 	if (!isCRLF())
// 		return false;
	return true;
}

bool Parser::isColon()
{
	return *_it++ == ':';
}

// prefix	 =  servername / ( nickname [ [ "!" user ] "@" host ] )
bool Parser::isPrefix(parsedMessage& parsedMsg)
{
	(void)parsedMsg;
	return (false);
}

// bool Parser::is_servername()
// {
// 	return false;
// }

// bool Parser::isNickname()
// {
// 	return false;
// }

// SPACE	  =  %x20		; space character
bool Parser::isSpace()
{
	return *_it++ == ' ';
}

// command	=  1*letter / 3digit ; 1 or more letter OR eactly 3 digit
bool Parser::isCommand(parsedMessage& parsedMsg)
{
	std::string::iterator begin = _it;

	while(std::isalpha(*_it))
	{
		*_it = std::toupper(*_it);
		_it++;
	}
	if (_it > begin)
	{
		std::string command(begin, _it);
		parsedMsg.command = command;
		// Logger::log(DEBUG, "Command",  parsedMsg.command);
		return true;
	}
	
	while(std::isdigit(*_it))
		_it++;

	if (_it > begin)
	{
		std::string command(begin, _it++);
		 parsedMsg.command = command;
		// Logger::log(DEBUG, "Command",  parsedMsg.command);
		if ( parsedMsg.command.length() == 3)
			return true;
		else
			return false;
	}
	return false;
}

// params	 =  *14( SPACE middle ) [ SPACE ":" trailing ]
//			=/ 14( SPACE middle ) [ SPACE [ ":" ] trailing ]
bool Parser::isParams(parsedMessage& parsedMsg)
{
	std::string::iterator begin;
	std::string::iterator backtrack;
	int i;
	
	for(i = 0; i < 14; i++)
	{
		begin = _it;
		if (!isSpace())
		{
			_it = begin;
			break;
		}
		if (!isMiddle(parsedMsg))
		{
			_it = begin;
			break;
		}
	}

	begin = _it;
	if (!isSpace())
	{
		_it = begin;
		return true;
	}
	if ( parsedMsg.params.size() < 14)
	{
		if (!isColon())
		{
			_it = begin;
			return true;
		}
	}
	else
	{
		backtrack = _it;
		if (!isColon())
			_it = backtrack;
	}

	backtrack = _it;
	if (!isTrailing(parsedMsg))
		_it = begin;

	return true;
}

// middle	 =  nospcrlfcl *( ":" / nospcrlfcl )
bool Parser::isMiddle(parsedMessage& parsedMsg)
{
	std::string::iterator begin = _it;
	std::string::iterator checkpoint;

	if (!isNospcrlfcl())
		return false;

	while (true)
	{
		checkpoint = _it;
		if (!isColon())
		{
			_it = checkpoint;
			if (!isNospcrlfcl())
			{
				_it = checkpoint;
				break;
			}
		}
	}
	std::string param(begin, _it);
	parsedMsg.params.push_back(param);
	// Logger::log(DEBUG, "Param", param);
	return true;
}

// nospcrlfcl =  %x01-09 / %x0B-0C / %x0E-1F / %x21-39 / %x3B-FF ; any octet except NUL, CR, LF, " " and ":"
bool Parser::isNospcrlfcl()
{
	std::string::iterator begin = _it;
	if (isNull())
		return false;
	_it = begin;

	if (isCR())
		return false;
	_it = begin;

	if (isLF())
		return false;
	_it = begin;

	if (isSpace())
		return false;
	_it = begin;

	return (!isColon());
}

bool Parser::isNull()
{
	return *_it++ == '\0';
}

bool Parser::isCR()
{
	return *_it++ == '\r';
}

bool Parser::isLF()
{
	return *_it++ == '\n';
}

// crlf	   =  %x0D %x0A   ; "carriage return" "linefeed"
bool Parser::isCRLF()
{
	return isCR() && isLF();
}

// trailing   =  *( ":" / " " / nospcrlfcl ) ; 0 or more
bool Parser::isTrailing(parsedMessage& parsedMsg)
{
	std::string::iterator begin = _it;
	std::string::iterator backtrack;

	while(true)
	{
		backtrack = _it;
		if (!isColon())
			_it= backtrack;
		else
			continue;
		if (!isSpace())
			_it = backtrack;
		else
			continue;
		if (!isNospcrlfcl())
		{
			_it = backtrack;
			break;
		}
	}

	std::string trailing(begin, _it);
	//  parsedMsg.trailing = trailing;
	 parsedMsg.params.push_back(trailing);
	// Logger::log(DEBUG, "Trailing", trailing);
	return true;
}


/*
	Helpers
*/

bool	Parser::isSpecial(std::string::iterator it)
{
	return *it == '[' || *it == ']' || *it == '\\' || *it == '`' || *it == '_' || *it == '^' || *it == '{' || *it == '|' || *it == '}';
}

bool	Parser::isChstring(std::string::iterator it)
{
	return *it != ' ' && *it != 7 && *it != '\0' && *it != '\r' && *it != '\n' && *it != ',';
}

bool Parser::isPartial(std::string& message)
{
	return message.find(CRLF) == std::string::npos;
}

int Parser::getMessageLength(std::string& message)
{
	return message.find(CRLF) + 2;
}

// nickname   =  ( letter / special ) *8( letter / digit / special / "-" )
// letter	 =  %x41-5A / %x61-7A	   ; A-Z / a-z
// digit	  =  %x30-39				 ; 0-9
// special	=  %x5B-60 / %x7B-7D		 ; "[", "]", "\", "`", "_", "^", "{", "|", "}"
bool	Parser::isNickname(std::string& nickname)
{
	std::string::iterator it = nickname.begin();
	if (!std::isalpha(*it) && !Parser::isSpecial(it))
		return false;
	it++;
	for(int i = 0; it != nickname.end(); i++, it++)
	{
		if (i == 8)
			return false;
		if(!std::isalnum(*it) && !Parser::isSpecial(it) && *it != '-')
			return false;
	}	
	return true;
}

// <channel>	::= ('#' | '&') <chstring>
// <chstring>   ::= <any 8bit code except SPACE, BELL, NUL, CR, LF and comma (',')>
bool	Parser::isChannel(std::string& channel)
{
	std::string::iterator it = channel.begin();

	if (*it != '#' && *it != '&')
		return false;
	it++;
	for(; it != channel.end(); it++)
	{
		if (!isChstring(it))
			return false;
	}
	return true;
}

// msgtarget  =  msgto *( "," msgto )
// msgto	  =  channel / ( user [ "%" host ] "@" servername )
// msgto	  =/ ( user "%" host ) / targetmask
// msgto	  =/ nickname / ( nickname "!" user "@" host )
std::vector<std::string> Parser::splitParam(const std::string& msgtarget, char delimiter)
{
	std::vector<std::string>	splitted;
	std::string					element;
	std::string::size_type		start = 0;
	std::string::size_type		end;

	while ((end = msgtarget.find(delimiter, start)) != std::string::npos)
	{
		element = msgtarget.substr(start, end - start);
		splitted.push_back(element);
		start = end + 1;
	}
	element = msgtarget.substr(start, end - start);
	splitted.push_back(element);
	return (splitted);
}

const std::string Parser::toLower(const std::string& string)
{
	std::string toLower;
	toLower.empty();

	for (std::string::const_iterator it = string.begin(); it != string.end(); it++)
		toLower.push_back(std::tolower(*it));
	return (toLower);
}