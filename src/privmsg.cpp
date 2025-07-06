#include "CommandHandler.hpp"
#include "Parser.hpp"
#include "macros.hpp"
#include "Logger.hpp"
#include <vector>
#include <string>

void	CommandHandler::_privMsgFp(const parsedMessage& parsedMsg) const
{
// 	Logger::log(INFO,  parsedMsg.command + " received.");
	
	std::string 				userNickname = _srvAPI.getUserNickname();
	std::string					command = parsedMsg.command;
	std::vector<std::string>	targets;
	std::string					message;
	std::string					userID = _srvAPI.getUserIdentifier();

	if (!_srvAPI.isUserRegistered())
	{
		_srvAPI.sendReply(ERR_NOTREGISTERED(userNickname, command));
		return ;
	}

	if (parsedMsg.params.size() == 0)
	{
		_srvAPI.sendReply(ERR_NORECIPIENT(userNickname, command));
		return ;
	}

	if (parsedMsg.params.size() == 1)
	{
		_srvAPI.sendReply(ERR_NOTEXTTOSEND(userNickname));
		return ;
	}

	if (parsedMsg.params.size() > 2)
	{
		_srvAPI.sendReply(ERR_TOOMANYTARGETS(userNickname, command));
		return ;
	}

// 	Logger::log(DEBUG, parsedMsg.command + " Parsing targets: ", parsedMsg.params.at(0));
	targets = Parser::splitParam(parsedMsg.params.at(0), ',');
	message = parsedMsg.params.at(1);

	std::string channelId;
	std::string channelName;
	for (std::vector<std::string>::iterator msgto = targets.begin(); msgto != targets.end(); msgto++)
	{
		// Logger::log(DEBUG, parsedMsg.command + " Target: ", *msgto);
		if (Parser::isNickname(*msgto))
		{
			if (!_srvAPI.doesNicknameExist(*msgto))
			{
				_srvAPI.sendReply(ERR_NOSUCHNICK(userNickname, *msgto));
				continue;
			}
			if (*msgto != userNickname)
				_srvAPI.sendToTarget(PRIVMSG_RPL(userID, *msgto, message), *msgto);
		}
		else if (Parser::isChannel(*msgto))
		{
			channelId = Parser::toLower(*msgto);
			if (!_srvAPI.doesChannelExist(channelId))
			{
				_srvAPI.sendReply(ERR_NOSUCHCHANNEL(userNickname, *msgto));
				continue ;
			}

			channelName = _srvAPI.getChannelName(channelId);

			_srvAPI.sendMessageToChannel(channelId, PRIVMSG_RPL(userID, channelName, message));
			// if (_srvAPI.isChannelUser(channelId))
			// 	_srvAPI.sendMessageToChannel(channelId, PRIVMSG_RPL(userID, channelName, message));
			// else
			// {
			// 	replyMessage = buildReply(SERVER_NAME,
			// 		ERR_CANNOTSENDTOCHAN, userNickname, *msgto, "Cannot send to channel");
			// 	_srvAPI.sendReply(replyMessage);
			// }
		}
		else
			_srvAPI.sendReply(ERR_NOSUCHNICK(userNickname, *msgto));
	}
}
