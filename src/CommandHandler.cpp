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

	// CommandHandler::setClientFd(fd);
	if (it != _commands.end())
	{
		(this->*(it->second))(parsed_msg);
	}
	else
	{
		Logger::log(INFO, "Unknown Command.", command);
		// TODO Send code repy
	}
}

void	CommandHandler::_inviteFp(parsed_message& parsed_msg)
{
	Logger::log(INFO,  parsed_msg.command + " received.");
	_srvAPI.send_reply("You've sent a" +  parsed_msg.command + "request!");
}

void	CommandHandler::_kickFp(parsed_message& parsed_msg)
{
	Logger::log(INFO,  parsed_msg.command + " received.");
	_srvAPI.send_reply("You've sent a " +  parsed_msg.command + " request!");
}

void	CommandHandler::_modeFp(parsed_message& parsed_msg)
{
	Logger::log(INFO,  parsed_msg.command + " received.");
	std::string user_nickname = _srvAPI.getUserNick();
	std::string	command = parsed_msg.command;
	std::string reply_message;
	bool		status = true;
	std::string mode;

	if (!_srvAPI.isUserRegistered())
	{
		reply_message = build_reply(SERVER_NAME, ERR_NOTREGISTERED, user_nickname, command, "You have not registered");
		_srvAPI.send_reply(reply_message);
		return ;
	}

	if (parsed_msg.params.size() == 0)
	{
		reply_message = build_reply(SERVER_NAME, ERR_NEEDMOREPARAMS, user_nickname, command, "Not enough parameters");
		_srvAPI.send_reply(reply_message);
		return ;
	}

	std::string	param = parsed_msg.params.at(0);
	if (Parser::is_nickname(param))
	{
		std::string nickname = param;
		if (nickname != user_nickname)
		{
			reply_message = build_reply(SERVER_NAME, ERR_USERSDONTMATCH, user_nickname, "Cannot change mode for other users");
			_srvAPI.send_reply(reply_message);
			return ;
		}
		if (parsed_msg.params.size() == 1)
		{
			if (_srvAPI.isUserInvisible())
				mode = "+i";
			else
				mode = "-i";
			reply_message = build_reply(user_nickname, "MODE", user_nickname, mode);
			_srvAPI.send_reply(reply_message);
			return ;
		}
		param = parsed_msg.params.at(1);
		for (std::string::iterator it = param.begin(); it != param.end(); it++)
		{
			if (*it == '+')
				status = true;
			else if (*it == '-')
				status = false;
			else if (*it == 'i')
			{
				if (_srvAPI.isUserInvisible() != status)
				{
					_srvAPI.setUserInvisibleMode(status);
					if (status == true)
						mode = "+i";
					else
						mode = "-i";
					reply_message = build_reply(user_nickname, "MODE", user_nickname, mode);
					_srvAPI.send_reply(reply_message);
				}
			}
			else
			{
				reply_message = build_reply(SERVER_NAME, ERR_UMODEUNKNOWNFLAG, user_nickname, "Unknown MODE flag");
				_srvAPI.send_reply(reply_message);
			}	
		}        
	}
	else if (Parser::is_channel(param))
	{
		std::cout << "MODE channel" << std::endl;
	}
	else
	{
		std::cout << "MODE none" << std::endl;
		// :Rubicon.GeekShed.net 401 norbac no :No such nick/channel
	}

	
}

void	CommandHandler::_nickFp(parsed_message& parsed_msg)
{
	Logger::log(INFO,  parsed_msg.command + " received.");
	
	std::string	user_nickname = _srvAPI.getUserNick();
	std::string	command = parsed_msg.command;
	std::string	new_nickname;
	std::string	reply_message;
	std::string user_identifier;

	if (!_srvAPI.getUserPasswordState())
	{
		reply_message = build_reply(SERVER_NAME, ERR_PASSWDMISMATCH, user_nickname, "Password incorrect");
		_srvAPI.send_reply(reply_message);
		_srvAPI.disconnectUser();
		return ;
	}
	
	if(parsed_msg.params.size() != 1)
	{
		reply_message = build_reply(SERVER_NAME, ERR_NONICKNAMEGIVEN, user_nickname, command, "No nickname given");
		_srvAPI.send_reply(reply_message);
		if( !_srvAPI.isUserRegistered())
			_srvAPI.disconnectUser();
		return ;
	}

	new_nickname = *parsed_msg.getParamsBegin();
	Logger::log(DEBUG, "Checking nick correctness");

	if (!Parser::is_nickname(new_nickname))
	{
		reply_message = build_reply(SERVER_NAME, ERR_ERRONEUSNICKNAME, user_nickname, new_nickname, "Erroneous nickname");
		_srvAPI.send_reply(reply_message);
		_srvAPI.disconnectUser();
		return ;
	}

	Logger::log(DEBUG, "Checking nick uniqueness");
	if (!isNickUnique(new_nickname))
	{
		if (new_nickname != user_nickname)
		{
			reply_message = build_reply(SERVER_NAME, ERR_NICKNAMEINUSE, user_nickname, new_nickname, "Nickname is already in use");
			_srvAPI.send_reply(reply_message);
		}	
		if( !_srvAPI.isUserRegistered())
			_srvAPI.disconnectUser();
		return ;
	}	
	if(_srvAPI.isUserRegistered())
	{
		user_identifier = _srvAPI.getUserIdentifier(); 
		
		reply_message = build_reply(user_identifier, "NICK", new_nickname);
		_srvAPI.sendToAll(reply_message);
		// TODO send to the rest of users
	}
	_srvAPI.setUserNick(new_nickname);
}

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
	_srvAPI.send_reply("PONG");
}

void	CommandHandler::_privMsgFp(parsed_message& parsed_msg)
{
	Logger::log(INFO,  parsed_msg.command + " received.");
	
	std::string 				user_nickname = _srvAPI.getUserNick();
	std::string					command = parsed_msg.command;
	std::vector<std::string>	targets;
	std::string 				reply_message;
	std::string					message;
	std::string					user_identifier = _srvAPI.getUserIdentifier();

	if (!_srvAPI.isUserRegistered())
	{
		reply_message = build_reply(SERVER_NAME, ERR_NOTREGISTERED, user_nickname, command, "You have not registered");
		_srvAPI.send_reply(reply_message);
		return ;
	}

	if (parsed_msg.params.size() == 0)
	{
		message = "No recipient given (";
		message += command;
		message += ")";

		reply_message = build_reply(SERVER_NAME, ERR_NORECIPIENT, user_nickname, message);
		_srvAPI.send_reply(reply_message);
		return ;
	}

	if (parsed_msg.params.size() == 1)
	{
		reply_message = build_reply(SERVER_NAME, ERR_NOTEXTTOSEND, user_nickname, "No text to send");
		_srvAPI.send_reply(reply_message);
		return ;
	}

	if (parsed_msg.params.size() > 2)
	{
		reply_message = build_reply(SERVER_NAME, ERR_TOOMANYTARGETS, user_nickname, command, "Too many targets. Nomessage delivered");
		_srvAPI.send_reply(reply_message);
		return ;
	}

	Logger::log(DEBUG, parsed_msg.command + " Parsing targets: ", parsed_msg.params.at(0));
	targets = Parser::splitParam(parsed_msg.params.at(0), ',');
	message = parsed_msg.params.at(1);

	for (std::vector<std::string>::iterator msgto = targets.begin(); msgto != targets.end(); msgto++)
	{
		Logger::log(DEBUG, parsed_msg.command + " Target: ", *msgto);
		if (Parser::is_nickname(*msgto))
		{
			if (isNickUnique(*msgto))
			{
				reply_message = build_reply(SERVER_NAME, ERR_NOSUCHNICK, user_nickname, *msgto, "No such nick/channel");
				_srvAPI.send_reply(reply_message);
			}
			if (*msgto != user_nickname)
			{
				reply_message = build_reply(user_identifier, command, *msgto, message);
				_srvAPI.sendToUser(reply_message, *msgto);
			}
		}
		else if (Parser::is_channel(*msgto))
		{
			// if (*msgto !exists)
			// {
			// 	reply_message = build_reply(SERVER_NAME, ERR_NOSUCHNICK, user_nickname, *msgto, "No such nick/channel");
			// 	_srvAPI.send_reply(reply_message);
			// 	return ;
			// }
		}
		else
		{
			reply_message = build_reply(SERVER_NAME, ERR_NOSUCHNICK, user_nickname, *msgto, "No such nick/channel");
			_srvAPI.send_reply(reply_message);
		}
	}
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
	reply_message += arg2;
	
	reply_message += SPACE;
	if (arg4.size() == 0)
		reply_message += COLON;
	reply_message += arg3;
	
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

	std::cout << "Built message: " << reply_message << std::endl;
	return reply_message;
}
