#include "CommandHandler.hpp"
# include "IServerAPI.hpp"
#include "macros.hpp"
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

void	CommandHandler::execute(parsed_message& parsed_msg)
{
	const std::string command =  parsed_msg.command;
	commands::iterator it = _commands.find(command);

	if (it != _commands.end())
	{
		Logger::log(INFO, "Passing parsed message into funcPtr.");
		(this->*(it->second))(parsed_msg);
	}
	else
		Logger::log(INFO, "Unknown Command.", command);
}

void	CommandHandler::_inviteFp(parsed_message& parsed_msg)
{
	Logger::log(INFO,  parsed_msg.command + " received.");
	_srvAPI.send_reply("You've sent a" +  parsed_msg.command + "request!");
}

// Join was added to it's own file: CommandHandler_Join.cpp

void	CommandHandler::_kickFp(parsed_message& parsed_msg)
{
	Logger::log(INFO,  parsed_msg.command + " received.");
	_srvAPI.send_reply("You've sent a" +  parsed_msg.command + "request!");
}

void	CommandHandler::_modeFp(parsed_message& parsed_msg)
{
	Logger::log(INFO,  parsed_msg.command + " received.");
	_srvAPI.send_reply("You've sent a" +  parsed_msg.command + "request!");
}

void	CommandHandler::_nickFp(parsed_message& parsed_msg)
{
	Logger::log(INFO,  parsed_msg.command + " received.");
	
	std::string message;

	if (!_srvAPI.getUserPasswordState())
	{
		message = SERVER_PEFIX;
		message += SPACE ERR_PASSWDMISMATCH;
		message += SPACE;
		message += _srvAPI.getUserNick();
		message += SPACE;
		message += ": Password incorrect";
		_srvAPI.send_reply(message);
		_srvAPI.disconnectUser();
		return ;
	}
	
	if(parsed_msg.params.capacity() != 1)
	{
		message = SERVER_PEFIX;
		message += SPACE ERR_NONICKNAMEGIVEN;
		message += SPACE;
		message += _srvAPI.getUserNick();
		message += SPACE;
		message += parsed_msg.command;
		message += SPACE;
		message += ":No nickname given";
		_srvAPI.send_reply(message);
		_srvAPI.disconnectUser();
		return ;
	}

	std::string nickname = *parsed_msg.getParamsBegin();
	Logger::log(DEBUG, "Checking nick corectness");

	if (!Parser::is_nickname(nickname))
	{
		message = SERVER_PEFIX;
		message += SPACE ERR_ERRONEUSNICKNAME;
		message += SPACE;
		message += _srvAPI.getUserNick();
		message += SPACE;
		message += nickname;
		message += SPACE;
		message += ":Erroneous nickname";
		_srvAPI.send_reply(message);
		_srvAPI.disconnectUser();
		return ;
	}

	Logger::log(DEBUG, "Checking nick uniqueness");
	if (!_isNickUnique(nickname))
	{
		message = SERVER_PEFIX;
		message += SPACE ERR_NICKNAMEINUSE;
		message += SPACE;
		message += _srvAPI.getUserNick();
		message += SPACE;
		message += nickname;
		message += SPACE;
		message += ":Nickname is already in use";
		_srvAPI.send_reply(message);
		_srvAPI.disconnectUser();
		return ;
	}
		
	_srvAPI.setUserNick(nickname);
	if(_srvAPI.isUserRegistered())
		(void) parsed_msg;	// TODO change UserNIck In all channels keys
}

void	CommandHandler::_passFp(parsed_message& parsed_msg)
{
	Logger::log(INFO,  parsed_msg.command + " received.");
	
	std::string message;

	if(_srvAPI.isUserRegistered())
	{
		message = SERVER_PEFIX;
		message += SPACE ERR_ALREADYREGISTRED;
		message += SPACE;
		message += _srvAPI.getUserNick();
		message += SPACE;
		message += ":Unauthorized command (already registered)";
		_srvAPI.send_reply(message);
		return ;
	}
	
	if(parsed_msg.params.capacity() != 1)
	{
		message = SERVER_PEFIX;
		message += SPACE ERR_NEEDMOREPARAMS;
		message += SPACE;
		message += _srvAPI.getUserNick();
		message += SPACE;
		message += parsed_msg.command;
		message += SPACE;
		message += ":Not enough parameters";
		_srvAPI.send_reply(message);
		return ;
	}

	if(_srvAPI.isPasswordValid(parsed_msg.params.at(0)))
		_srvAPI.setUserPasswordState(true);
	else
	{
		message = SERVER_PEFIX;
		message += SPACE ERR_PASSWDMISMATCH;
		message += SPACE;
		message += _srvAPI.getUserNick();
		message += SPACE;
		message += ": Password incorrect";
		_srvAPI.send_reply(message);
		_srvAPI.disconnectUser();
	}
}

void	CommandHandler::_privMsgFp(parsed_message& parsed_msg)
{
	Logger::log(INFO,  parsed_msg.command + " received.");
	_srvAPI.send_reply("You've sent a" +  parsed_msg.command + "request!");
}

void	CommandHandler::_realFp(parsed_message& parsed_msg)
{
	Logger::log(INFO,  parsed_msg.command + " received.");
	_srvAPI.send_reply("You've sent a" +  parsed_msg.command + "request!");
}

void	CommandHandler::_topicFp(parsed_message& parsed_msg)
{
	Logger::log(INFO,  parsed_msg.command + " received.");
	_srvAPI.send_reply("You've sent a" +  parsed_msg.command + "request!");
}

void	CommandHandler::_userFp(parsed_message& parsed_msg)
{
	Logger::log(INFO,  parsed_msg.command + " received.");
	// TODO check number of params!!!!
	paramsIt it = parsed_msg.getParamsBegin();

	_srvAPI.setUserRegisteredStatus(true);

	std::string message = SERVER_PEFIX SPACE RPL_WELCOME SPACE;
	message += _srvAPI.getUserNick();
	message += " :Welcome to the Internet Relay Network\n";
	message += _srvAPI.getUserNick();
	message += "!";
	message += *++it;
	message += "@";
	message += *++it;

	_srvAPI.send_reply(message);
}

bool	CommandHandler::_isNickUnique(const std::string nick)
{
	for (usrsIt it = _srvAPI.getUsersBegin(); it != _srvAPI.getUsersEnd(); it++)
	{
		if(it->second->getNickname() == nick)
			return false;
	}
	return true;
}
//	_srvAPI.send_reply("You've sent a" +  parsed_msg.command + "request!");
