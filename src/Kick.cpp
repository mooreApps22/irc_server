#include "CommandHandler.hpp"
#include "IServerAPI.hpp"
#include "macros.hpp"
#include "Logger.hpp"
#include <vector>
#include <string>

	/*
		KICK <channel>,<channel> <user>,<user> [comment]
	*/

void	CommandHandler::_kickFp(parsed_message& parsed_msg)
{
	Logger::log(INFO,  parsed_msg.command + " received.");
	std::cout << "KICK command is parsing." << std::endl;

	std::string	replyMessage;
	std::string	userNickname = _srvAPI.getUserNick();
	std::string	command = parsed_msg.command;	
	std::string	userID = _srvAPI.getUserIdentifier();

	if (!_srvAPI.isUserRegistered())
	{
		replyMessage = build_reply(SERVER_NAME, ERR_NOTREGISTERED, userNickname, command, "You have not registered");
		_srvAPI.send_reply(replyMessage);
		return ;
	}
	Logger::log(INFO, "KICK: user is registered");
	if (parsed_msg.params.size() < 2)
	{
		replyMessage = build_reply(SERVER_NAME, ERR_NEEDMOREPARAMS, userNickname, command, "Not enough parameters");
		_srvAPI.send_reply(replyMessage);
		return ;
	}
	Logger::log(INFO, "KICK: has enough params");

	std::vector<std::string> channelNames = Parser::splitParam(parsed_msg.params[0], ',');
	std::vector<std::string> userNames = Parser::splitParam(parsed_msg.params[1], ',');

	if ((channelNames.size() != userNames.size()) && !(channelNames.size() == 1 && userNames.size() > 0))
	{
		replyMessage = build_reply(SERVER_NAME, ERR_NEEDMOREPARAMS, userNickname, command, "Not enough parameters");
		_srvAPI.send_reply(replyMessage);
		std::cout << "Users: " << userNames.size() << "Channels: " << channelNames.size() << std::endl;
		return ;
	}
	Logger::log(INFO, "KICK: users >= channels");


	std::string	reason;

	if (parsed_msg.params.size() > 2)
		reason = parsed_msg.params[2];
	else
		reason = userNickname;

	//Channel Loop
	for (paramsIt chIt = channelNames.begin(); chIt != channelNames.end(); ++chIt)
	{
		std::string channelId = Parser::toLower(*chIt);

		Logger::log(INFO, "KICK: Made it to the outer loop.");

		if (!_srvAPI.doesChannelExist(channelId))
		{
			replyMessage = build_reply(SERVER_NAME, ERR_NOSUCHCHANNEL, userNickname, *chIt, "No such channel");
			_srvAPI.send_reply(replyMessage);
			continue ;	
		}

		std::string channelName = _srvAPI.getChannelName(channelId);

		Logger::log(INFO, "KICK: channel param exist");
		if (!_srvAPI.isUserChannelOperator(channelId))
		{
			replyMessage = build_reply(SERVER_NAME, "482", userNickname, channelName, "You're not a channel operator");
			_srvAPI.send_reply(replyMessage);
			continue ;
		}
		Logger::log(INFO, "KICK: Kicker is Channel Operator");
		// User Loop
		for (paramsIt userIt = userNames.begin(); userIt != userNames.end(); ++userIt)
		{
			Logger::log(INFO, "KICK: made it to the inner loop");
			if (!_srvAPI.isTargetInChannel(channelId, *userIt))
			{
				replyMessage = build_reply(SERVER_NAME, ERR_NOSUCHNICK, userNickname, *userIt, "User not in channel");
				_srvAPI.send_reply(replyMessage);
				continue ;
			}
			Logger::log(INFO, "KICK: the target user is in the channel");
			std::string kickMessage = build_reply(userID, "KICK", channelName, *userIt, reason);
			_srvAPI.sendMessageToChannel(channelId, kickMessage);
			_srvAPI.removeUserFromChannel(channelId, *userIt);
			std::cout << *userIt << " was kicked the fuck out by " << userNickname << std::endl;
		}
	}
}


