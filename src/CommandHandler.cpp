#include "CommandHandler.hpp"
#include "Logger.hpp"
#include <vector>
#include <string>


CommandHandler::CommandHandler(IServerAPI& srvAPI)
	:	_srvAPI(srvAPI)
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


void	CommandHandler::execute(parsed_message parsed_message)
{
	const std::string command = parsed_message.command;
	commands::iterator it = _commands.find(command);
	if (it != _commands.end())
	{
		(this->*(it->second))();
	}
	else
		Logger::log(INFO, "Unknown Command.", command);
}

void	CommandHandler::_passFp()
{
	Logger::log(INFO, "PASS received.");
	_srvAPI.send_reply("You've sent a PASS request!");
}

void	CommandHandler::_nickFp()
{
	
	Logger::log(INFO, "NICK  received.");
	_srvAPI.send_reply("You've sent a NICK request!");

}

void	CommandHandler::_userFp()
{
	
	Logger::log(INFO, "USER  received.");
	_srvAPI.send_reply("You've sent a USER request!");

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
