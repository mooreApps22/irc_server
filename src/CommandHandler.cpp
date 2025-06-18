#include "CommandHandler.hpp"
#include "Logger.hpp"
#include <vector>
#include <string>


CommandHandler::CommandHandler(Server& server, Parser& parser)
	:	_server(server), 
		_parser(parser)
{
	Logger::log(INFO, "CommandHandler constructed.");

	_commands["INVITE"]		= &CommandHandler::_inviteFp;
	_commands["JOIN"]		= &CommandHandler::_joinFp;
	_commands["KICK"]		= &CommandHandler::_kickFp;
	_commands["MODE"]		= &CommandHandler::_modeFp;
	_commands["NICK"]		= &CommandHandler::_nickFp;
	_commands["PASS"]		= &CommandHandler::_passFp;
	_commands["PRIVMSG"]	= &CommandHandler::_privMsgFp;
	_commands["REAL"] 		= &CommandHandler::_realFp;
	_commands["TOPIC"] 		= &CommandHandler::_topicFp;
	_commands["USER"] 		= &CommandHandler::_userFp;
	(void)_server;
}

CommandHandler::~CommandHandler()
{
	Logger::log(INFO, "CommandHandler destructed.");
}
/*
	  the iterator constructor can also be used to construct from arrays:
	  int myints[] = {16,2,77,29};
	  std::vector<int> fifth (myints, myints + sizeof(myints) / sizeof(int) );
*/
void	CommandHandler::execute(void)
{
	commands::iterator it = _commands.find(_parser.get_command());
	if (it != _commands.end())
	{
		(this->*(it->second))();
	}
	else
		Logger::log(INFO, "Unknown Command.", _parser.get_command());
}

void	CommandHandler::_passFp()
{
	Logger::log(INFO, "PASS received.");
	
}

void	CommandHandler::_nickFp()
{
	
	Logger::log(INFO, "NICK  received.");
}

void	CommandHandler::_userFp()
{
	
	Logger::log(INFO, "USER  received.");
}

void	CommandHandler::_realFp()
{
	Logger::log(INFO, ".");
	
}

void	CommandHandler::_joinFp()
{
	
	Logger::log(INFO, ".");
}

void	CommandHandler::_privMsgFp()
{
	
	Logger::log(INFO, ".");
	
}

void	CommandHandler::_kickFp()
{
	Logger::log(INFO, ".");
}

void	CommandHandler::_inviteFp()
{
	Logger::log(INFO, ".");
	
}

void	CommandHandler::_modeFp()
{
	Logger::log(INFO, ".");
	
}

void	CommandHandler::_topicFp()
{
	Logger::log(INFO, ".");
	
}
