#include "../inc/CommandHandler.hpp"
#include "../inc/Logger.hpp"
#include <vector>
#include <string>

CommandHandler::CommandHandler(Server& server, Parser& parser)
	:	_server(server), 
		_parser(parser)
{
	Logger::log(INFO, "CommandHandler constructed.");
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
	std::string my_commands[] = {"PASS", "NICK", "USER", "REAL", "JOIN", "PRIVMSG", "KICK", "INVITE", "TOPIC", "MODE"};
		
	std::vector<std::string> commands(my_commands, my_commands + sizeof(my_commands) / sizeof(std::string));
  	void (CommandHandler::*fptr[])() = {&CommandHandler::_passFp, &CommandHandler::_nickFp, &CommandHandler::_userFp, &CommandHandler::_realFp, &CommandHandler::_joinFp, &CommandHandler::_privMsgFp,
										&CommandHandler::_kickFp, &CommandHandler::_inviteFp, &CommandHandler::_topicFp, &CommandHandler::_modeFp};
	
	for (std::vector<std::string>::iterator it = commands.begin(); it < commands.end(); it++)
	{
		if (*it == _parser.get_command())
		{
			(this->*fptr[it - commands.begin()])();
			break ;
		}
		else if (it == commands.end() - 1)
		{
			Logger::log(INFO, "Unknown Command.", _parser.get_command());
		}
	}
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
	Logger::log(INFO, "Handling JOIN.");
	_server.addChannel(_parser.getTrailing()); // <==== I'm trying to 
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
