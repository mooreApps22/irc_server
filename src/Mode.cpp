#include "CommandHandler.hpp"
#include "ChannelModeHandler.hpp"

#include "IServerAPI.hpp"
#include "Logger.hpp"
#include "macros.hpp"
#include <string>
#include <cstdlib>

void	CommandHandler::_modeFp(parsed_message& parsed_msg)
{
	Logger::log(INFO,  parsed_msg.command + " received.");
	std::string user_nickname = _srvAPI.getUserNick();
	std::string	command = parsed_msg.command;
	std::string reply_message;
	bool		status = true;
	std::string mode;
	std::string user_identifier = _srvAPI.getUserIdentifier(); 


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
		ChannelModeHandler(_srvAPI, parsed_msg).handle();
	}
	else
	{
		reply_message = build_reply(SERVER_NAME, ERR_NOSUCHNICK, user_nickname, param, ":No such nick/channel");
		_srvAPI.send_reply(reply_message);
	}

	
}