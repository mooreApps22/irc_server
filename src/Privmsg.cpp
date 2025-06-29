#include "CommandHandler.hpp"
#include "IServerAPI.hpp"
#include "macros.hpp"
#include "Logger.hpp"
#include <vector>
#include <string>

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

	std::string channelId;
	std::string channelName;
	for (std::vector<std::string>::iterator msgto = targets.begin(); msgto != targets.end(); msgto++)
	{
		Logger::log(DEBUG, parsed_msg.command + " Target: ", *msgto);
		if (Parser::is_nickname(*msgto))
		{
			if (isNickUnique(*msgto))
			{
				reply_message = build_reply(SERVER_NAME, ERR_NOSUCHNICK, user_nickname, *msgto, "No such nick/channel");
				_srvAPI.send_reply(reply_message);
				continue;
			}
			if (*msgto != user_nickname)
			{
				reply_message = build_reply(user_identifier, command, *msgto, message);
				_srvAPI.sendToUser(reply_message, *msgto);
			}
		}
		else if (Parser::is_channel(*msgto))
		{
			channelId = Parser::toLower(*msgto);
			if (!_srvAPI.doesChannelExist(channelId))
			{
				reply_message = build_reply(SERVER_NAME, ERR_NOSUCHCHANNEL, user_nickname, *msgto, "No such channel");
				_srvAPI.send_reply(reply_message);
				continue ;
			}

			channelName = _srvAPI.getChannelName(channelId);

			reply_message = build_reply(user_identifier, command, channelName, message);
			_srvAPI.sendMessageToChannel(channelId, reply_message);
			// if (_srvAPI.isChannelUser(*msgto))
			// {
			// 	reply_message = build_reply(user_identifier, command, *msgto, message);
			// 	_srvAPI.sendMessageToChannel(*msgto, reply_message);
			// }
			// else
			// {
			// 	reply_message = build_reply(SERVER_NAME,
			// 		ERR_CANNOTSENDTOCHAN, user_nickname, *msgto, "Cannot send to channel");
			// 	_srvAPI.send_reply(reply_message);
			// }
		}
		else
		{
			reply_message = build_reply(SERVER_NAME, ERR_NOSUCHNICK, user_nickname, channelName, "No such nick/channel");
			_srvAPI.send_reply(reply_message);
		}
	}
}
