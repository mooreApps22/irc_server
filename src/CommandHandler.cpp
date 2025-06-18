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


void	CommandHandler::execute(parsed_message& parsed_message)
{
	const std::string command = parsed_message.command;
	commands::iterator it = _commands.find(command);
	if (it != _commands.end())
	{
		(this->*(it->second))(parsed_message);
	}
	else
		Logger::log(INFO, "Unknown Command.", command);
}

void	CommandHandler::_passFp(parsed_message& parsed_message)
{
	Logger::log(INFO, parsed_message.command + " received.");
	_srvAPI.send_reply("You've sent a" + parsed_message.command + "request!");
}

void	CommandHandler::_nickFp(parsed_message& parsed_message)
{
	Logger::log(INFO, parsed_message.command + " received.");
	_srvAPI.send_reply("You've sent a" + parsed_message.command + "request!");
}

void	CommandHandler::_userFp(parsed_message& parsed_message)
{
	Logger::log(INFO, parsed_message.command + " received.");
	_srvAPI.send_reply("You've sent a" + parsed_message.command + "request!");
}

void	CommandHandler::_realFp(parsed_message& parsed_message)
{
	Logger::log(INFO, parsed_message.command + " received.");
	_srvAPI.send_reply("You've sent a" + parsed_message.command + "request!");
}

void	CommandHandler::_joinFp(parsed_message& parsed_message)
{
	Logger::log(INFO, parsed_message.command + " received.");
	_srvAPI.send_reply("You've sent a" + parsed_message.command + "request!");
}

void	CommandHandler::_privMsgFp(parsed_message& parsed_message)
{
	Logger::log(INFO, parsed_message.command + " received.");
	_srvAPI.send_reply("You've sent a" + parsed_message.command + "request!");
}

void	CommandHandler::_kickFp(parsed_message& parsed_message)
{
	Logger::log(INFO, parsed_message.command + " received.");
	_srvAPI.send_reply("You've sent a" + parsed_message.command + "request!");
}

void	CommandHandler::_inviteFp(parsed_message& parsed_message)
{
	Logger::log(INFO, parsed_message.command + " received.");
	_srvAPI.send_reply("You've sent a" + parsed_message.command + "request!");
}

void	CommandHandler::_modeFp(parsed_message& parsed_message)
{
	Logger::log(INFO, parsed_message.command + " received.");
	_srvAPI.send_reply("You've sent a" + parsed_message.command + "request!");
}

void	CommandHandler::_topicFp(parsed_message& parsed_message)
{
	Logger::log(INFO, parsed_message.command + " received.");
	_srvAPI.send_reply("You've sent a" + parsed_message.command + "request!");
}
