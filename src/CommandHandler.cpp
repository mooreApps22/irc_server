#include "CommandHandler.hpp"
#include "IServerAPI.hpp"
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
	_commands["PING"]		= &CommandHandler::_pingFp;
	_commands["PRIVMSG"]	= &CommandHandler::_privMsgFp;
	_commands["TOPIC"] 		= &CommandHandler::_topicFp;
	_commands["USER"] 		= &CommandHandler::_userFp;
}

CommandHandler::~CommandHandler()
{
	Logger::log(INFO, "CommandHandler destructed.");
}

void	CommandHandler::execute(parsed_message& parsed_msg)
{
	Logger::log(INFO, "execute() called.");
	const std::string command =  parsed_msg.command;
	commands::iterator it = _commands.find(command);

	if (it != _commands.end())
	{
		(this->*(it->second))(parsed_msg);
	}
	else
	{
		Logger::log(INFO, "Unknown Command.", command);

		std::string reply_message;
		std::string user_nickname = _srvAPI.getUserNick();

		reply_message = build_reply(SERVER_NAME, ERR_UNKNOWNCOMMAND, user_nickname, command, "Unknown command");
		_srvAPI.send_reply(reply_message);
	}
}

<<<<<<< HEAD


=======
>>>>>>> main
void	CommandHandler::_passFp(parsed_message& parsed_msg)
{
	Logger::log(INFO,  parsed_msg.command + " received.");
	
	std::string	user_nickname = _srvAPI.getUserNick();
	std::string	command = parsed_msg.command;
	std::string reply_message;

	if(_srvAPI.isUserRegistered())
	{
		reply_message = build_reply(SERVER_NAME, ERR_ALREADYREGISTRED, user_nickname, "Unauthorized command (already registered)");
		_srvAPI.send_reply(reply_message);
		return ;
	}
	
	if(parsed_msg.params.size() != 1)
	{
		reply_message = build_reply(SERVER_NAME, ERR_PASSWDMISMATCH, user_nickname, command, "Not enough parameters");
		_srvAPI.send_reply(reply_message);
		_srvAPI.disconnectUser();
		return ;
	}

	if(_srvAPI.isPasswordValid(parsed_msg.params.at(0)))
		_srvAPI.setUserPasswordState(true);
	else
	{
		reply_message = build_reply(SERVER_NAME, ERR_PASSWDMISMATCH, user_nickname, "Password incorrect");
		_srvAPI.send_reply(reply_message);
		_srvAPI.disconnectUser();
	}
}

void	CommandHandler::_pingFp(parsed_message& parsed_msg)
{
	Logger::log(INFO,  parsed_msg.command + " received.");

	std::string replyMessage;
	std::string userNickname = _srvAPI.getUserNick();

	if (parsed_msg.params.size() == 0)
		replyMessage = build_reply(SERVER_NAME, ERR_NOORIGIN, userNickname, "No origin specified");
	else
		replyMessage = build_reply(SERVER_NAME, "PONG", SERVER_NAME, parsed_msg.params.at(0));
	_srvAPI.send_reply(replyMessage);
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
		reply_message = build_reply(SERVER_NAME, ERR_NONICKNAMEGIVEN, user_nickname, command, "No nickname given");
		_srvAPI.send_reply(reply_message);
		_srvAPI.disconnectUser();
		return ;
	}

	if(_srvAPI.isUserRegistered())
	{
		reply_message = build_reply(SERVER_NAME, ERR_ALREADYREGISTRED, user_nickname, "Unauthorized command (already registered)");
		_srvAPI.send_reply(reply_message);
		return ;
	}
	
	if(parsed_msg.params.size() != 4)
	{
		reply_message = build_reply(SERVER_NAME, ERR_NEEDMOREPARAMS, user_nickname, command, "Not enough parameters");
		_srvAPI.send_reply(reply_message);
		_srvAPI.disconnectUser();
		return ;
	}

	username = parsed_msg.params.at(0);
	_srvAPI.setUserUsername(username);


	_srvAPI.setUserRegisteredStatus(true);

	std::string message = "Welcome to the Internet Relay Network ";
	message += _srvAPI.getUserIdentifier();
	reply_message = build_reply(SERVER_NAME, RPL_WELCOME, user_nickname, message);
	_srvAPI.send_reply(reply_message);

	message = "Your host is ";
	message += SERVER_NAME;
	message += ", running version ";
	message += VERSION;
	reply_message = build_reply(SERVER_NAME, RPL_YOURHOST, user_nickname, message);
	_srvAPI.send_reply(reply_message);

	message = "This server was created ";
	message += CREATION_DATE;
	reply_message = build_reply(SERVER_NAME, RPL_CREATED, user_nickname, message);
	_srvAPI.send_reply(reply_message);

	message = SERVER_NAME;
	message += SPACE;
	message += VERSION;
	message += SPACE;
	message += USER_MODES;
	message += SPACE;
	message += CHANNEL_MODES;
	reply_message = build_reply(SERVER_NAME, RPL_MYINFO, user_nickname, message);
	_srvAPI.send_reply(reply_message);
}

bool	CommandHandler::isNickUnique(const std::string nick)
{
	for (usrsIt it = _srvAPI.getUsersBegin(); it != _srvAPI.getUsersEnd(); it++)
	{
		if(it->second->getNickname() == nick)
			return false;
	}
	return true;
}

const std::string CommandHandler::build_reply(const std::string& arg1, const std::string& arg2, const std::string& arg3, const std::string arg4, const std::string arg5, const std::string arg6)
{
	std::string reply_message = COLON;
	reply_message += arg1;

	reply_message += SPACE;
	if (arg2.size() == 0)
		reply_message += COLON;
	reply_message += arg2;

	if (arg3.size() > 0)
	{
		reply_message += SPACE;
		if (arg4.size() == 0)
			reply_message += COLON;
		reply_message += arg3;
	}

	if (arg4.size() > 0)
	{
		reply_message += SPACE;
		if (arg5.size() == 0)
			reply_message += COLON;
		reply_message += arg4;
	}

	if (arg5.size() > 0)
	{
		reply_message += SPACE;
		if (arg6.size() == 0)
			reply_message += COLON;
		reply_message += arg5;
	}
	
	if (arg6.size() > 0)
	{
		reply_message += SPACE;
		reply_message += COLON;
		reply_message += arg6;
	}
	if (arg2 != "PONG")
		std::cout << "Built message: " << reply_message << std::endl;
	return reply_message;
}
