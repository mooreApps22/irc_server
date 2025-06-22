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

void	CommandHandler::_joinFp(parsed_message& parsed_msg)
{
	Logger::log(INFO,  parsed_msg.command + " received.");
	_srvAPI.send_reply("You've sent a" +  parsed_msg.command + "request!");
}

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
	
	std::string	user_nickname = _srvAPI.getUserNick();
	std::string	command = parsed_msg.command;
	std::string	nickname;
	std::string	reply_message;

	if (!_srvAPI.getUserPasswordState())
	{
		reply_message = build_reply(ERR_PASSWDMISMATCH, user_nickname, ": Password incorrect");
		_srvAPI.send_reply(reply_message);
		_srvAPI.disconnectUser();
		return ;
	}
	
	if(parsed_msg.params.capacity() != 1)
	{
		reply_message = build_reply(ERR_NONICKNAMEGIVEN, user_nickname, command, ":No nickname given");
		_srvAPI.send_reply(reply_message);
		if( !_srvAPI.isUserRegistered())
			_srvAPI.disconnectUser();
		return ;
	}

	nickname = *parsed_msg.getParamsBegin();
	Logger::log(DEBUG, "Checking nick corectness");

	if (!Parser::is_nickname(nickname))
	{
		reply_message = build_reply(ERR_ERRONEUSNICKNAME, user_nickname, nickname, ":Erroneous nickname");
		_srvAPI.send_reply(reply_message);
		_srvAPI.disconnectUser();
		return ;
	}

	Logger::log(DEBUG, "Checking nick uniqueness");
	if (!_isNickUnique(nickname))
	{
		if (nickname != user_nickname)
		{
			reply_message = build_reply(ERR_NICKNAMEINUSE, user_nickname, nickname, ":Nickname is already in use");
			_srvAPI.send_reply(reply_message);
		}	
		if( !_srvAPI.isUserRegistered())
			_srvAPI.disconnectUser();
		return ;
	}	
	if(_srvAPI.isUserRegistered())
	{
		std::string message = ":";
		message += _srvAPI.getUserIdentifier(); 
		message += SPACE;
		message += "NICK";
		message += SPACE;
		message += ":";
		message += nickname;
		_srvAPI.send_reply(message);
		// TODO send to the rest of users
	}
	_srvAPI.setUserNick(nickname);
}

void	CommandHandler::_passFp(parsed_message& parsed_msg)
{
	Logger::log(INFO,  parsed_msg.command + " received.");
	
	std::string	user_nickname = _srvAPI.getUserNick();
	std::string	command = parsed_msg.command;
	std::string reply_message;

	if(_srvAPI.isUserRegistered())
	{
		reply_message = build_reply(ERR_ALREADYREGISTRED, user_nickname, ":Unauthorized command (already registered)");
		_srvAPI.send_reply(reply_message);
		return ;
	}
	
	if(parsed_msg.params.capacity() != 1)
	{
		reply_message = build_reply(ERR_PASSWDMISMATCH, user_nickname, command, ":Not enough parameters");
		_srvAPI.send_reply(reply_message);
		_srvAPI.disconnectUser();
		return ;
	}

	if(_srvAPI.isPasswordValid(parsed_msg.params.at(0)))
		_srvAPI.setUserPasswordState(true);
	else
	{
		reply_message = build_reply(ERR_PASSWDMISMATCH, user_nickname, ": Password incorrect");
		_srvAPI.send_reply(reply_message);
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
	
	std::string	user_nickname = _srvAPI.getUserNick();
	std::string	command = parsed_msg.command;
	std::string reply_message;
	std::string username;
	// std::string realname;

	
	if(user_nickname == "*")
	{
		reply_message = build_reply(ERR_NONICKNAMEGIVEN, user_nickname, command, ":No nickname given");
		_srvAPI.send_reply(reply_message);
		_srvAPI.disconnectUser();
		return ;
	}

	if(_srvAPI.isUserRegistered())
	{
		reply_message = build_reply(ERR_ALREADYREGISTRED, user_nickname, ":Unauthorized command (already registered)");
		_srvAPI.send_reply(reply_message);
		return ;
	}
	
	if(parsed_msg.params.capacity() != 4)
	{
		reply_message = build_reply(ERR_NEEDMOREPARAMS, user_nickname, command, ":Not enough parameters");
		_srvAPI.send_reply(reply_message);
		_srvAPI.disconnectUser();
		return ;
	}

	username = parsed_msg.params.at(0);
	_srvAPI.setUserUsername(username);


	_srvAPI.setUserRegisteredStatus(true);

	std::string message = ":Welcome to the Internet Relay Network ";
	message += _srvAPI.getUserIdentifier();
	reply_message = build_reply(RPL_WELCOME, user_nickname, message);
	_srvAPI.send_reply(reply_message);

	message = ":Your host is ";
	message += SERVER_NAME;
	message += ", running version ";
	message += VERSION;
	reply_message = build_reply(RPL_YOURHOST, user_nickname, message);
	_srvAPI.send_reply(reply_message);

	message = ":This server was created ";
	message += CREATION_DATE;
	reply_message = build_reply(RPL_CREATED, user_nickname, message);
	_srvAPI.send_reply(reply_message);

	message = SERVER_NAME;
	message += SPACE;
	message += VERSION;
	message += SPACE;
	message += USER_MODES;
	message += SPACE;
	message += CHANNEL_MODES;
	reply_message = build_reply(RPL_MYINFO, user_nickname, message);
	_srvAPI.send_reply(reply_message);
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

const std::string CommandHandler::build_reply(const std::string& code, const std::string& dest, const std::string message)
{
	std::string reply_message = SERVER_PEFIX;
	reply_message += SPACE;
	reply_message += code;
	reply_message += SPACE;
	reply_message += dest;
	reply_message += SPACE;
	reply_message += message;

	std::cout << "Built message: " << reply_message << std::endl;
	return reply_message;
}

const std::string CommandHandler::build_reply(const std::string& code, const std::string& dest, const std::string arg, const std::string message)
{
	std::string reply_message = SERVER_PEFIX;
	reply_message += SPACE;
	reply_message += code;
	reply_message += SPACE;
	reply_message += dest;
	reply_message += SPACE;
	reply_message += arg;
	reply_message += SPACE;
	reply_message += message;

	std::cout << "Built message: " << reply_message << std::endl;
	return reply_message;
}