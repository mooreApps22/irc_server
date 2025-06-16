#include "Parser.hpp"
#include <iostream>

Parser::Parser()
	:	_prefix(""),
		_command(""),
		_params("")
{
	std::cout << "Parser Constructor called." << std::endl;
}

Parser::~Parser()
{
	std::cout << "Parser Destructor called." << std::endl;
}

bool Parser::is_partial(std::string& message)
{
	return message.find(CRLF) == std::string::npos;
}

int Parser::get_message_length(std::string& message)
{
	(void) message;
	return 0;
}

bool Parser::parse_message(std::string& message)
{
	(void) message;
	return false;
}

// message    =  [ ":" prefix SPACE ] command [ params ] crlf
// prefix     =  servername / ( nickname [ [ "!" user ] "@" host ] )
// command    =  1*letter / 3digit ; 1 or more letter OR eactly 3 digit
// params     =  *14( SPACE middle ) [ SPACE ":" trailing ]
//            =/ 14( SPACE middle ) [ SPACE [ ":" ] trailing ]

// nospcrlfcl =  %x01-09 / %x0B-0C / %x0E-1F / %x21-39 / %x3B-FF
//                     ; any octet except NUL, CR, LF, " " and ":"
// middle     =  nospcrlfcl *( ":" / nospcrlfcl )
// trailing   =  *( ":" / " " / nospcrlfcl ) ; 0 or more

// SPACE      =  %x20        ; space character
// crlf       =  %x0D %x0A   ; "carriage return" "linefeed"