#include "CommandHandler.hpp"
#include "IServerAPI.hpp"
#include "macros.hpp"
#include "Logger.hpp"
#include <vector>
#include <string>

	/*
		TOPIC <channel> [topic]

		 The TOPIC command is used to change or view the topic of a channel.
   The topic for channel <channel> is returned if there is no <topic>
   given.  If the <topic> parameter is present, the topic for that
   channel will be changed, if this action is allowed for the user
   requesting it.  If the <topic> parameter is an empty string, the
   topic for that channel will be removed.

	*/

void	CommandHandler::_topicFp(parsed_message& parsed_msg)
{
	Logger::log(INFO,  parsed_msg.command + " received.");
	std::cout << "TOPIC command is parsing." << std::endl;

	std::string	replyMessage;
	std::string	userNickname = _srvAPI.getUserNick();
	std::string	command = parsed_msg.command;	
	std::string	userID = _srvAPI.getUserIdentifier();

	if (!_srvAPI.isUserRegistered())
	{
		replyMessage = build_reply(SERVER_NAME, ERR_NOTREGISTERED, userNickname, command, "Not enough parameters");
		_srvAPI.send_reply(replyMessage);
		return ;
	}
	Logger::log(INFO, "TOPIC: user is registered");
	if (parsed_msg.params.size() < 2)
	{
		replyMessage = build_reply(SERVER_NAME, ERR_NEEDMOREPARAMS, userNickname, command, "Not enough parameters");
		_srvAPI.send_reply(replyMessage);
		return ;
	}
	Logger::log(INFO, "TOPIC: has enough params");

	std::vector<std::string> channelNames = Parser::splitParam(parsed_msg.params[0], ',');
	std::vector<std::string> userNames = Parser::splitParam(parsed_msg.params[1], ',');

	if ((channelNames.size() != userNames.size()) && !(channelNames.size() == 1 && userNames.size() > 0))
	{
		replyMessage = build_reply(SERVER_NAME, ERR_NEEDMOREPARAMS, userNickname, command, "Not enough parameters");
		_srvAPI.send_reply(replyMessage);
		std::cout << "Users: " << userNames.size() << "Channels: " << channelNames.size() << std::endl;
		return ;
	}
	Logger::log(INFO, "TOPIC: users >= channels");


	std::string	reason;

	if (parsed_msg.params.size() > 2)
		reason = parsed_msg.params[2];
	else
		reason = userNickname;

	//Channel Loop
	for (paramsIt chIt = channelNames.begin(); chIt != channelNames.end(); ++chIt)
	{
		Logger::log(INFO, "TOPIC: Made it to the outer loop.");
		if (!_srvAPI.doesChannelExist(*chIt))
		{
			replyMessage = build_reply(SERVER_NAME, ERR_NOSUCHCHANNEL, userNickname, *chIt, "No such channel");
			_srvAPI.send_reply(replyMessage);
			continue ;	
		}
		Logger::log(INFO, "TOPIC: channel param exist");
		if (!_srvAPI.isUserChannelOperator(*chIt))
		{
			replyMessage = build_reply(SERVER_NAME, "482", userNickname, *chIt, "You're not a channel operator");
			_srvAPI.send_reply(replyMessage);
			continue ;
		}
		Logger::log(INFO, "TOPIC: Kicker is Channel Operator");
		// User Loop
		for (paramsIt userIt = userNames.begin(); userIt != userNames.end(); ++userIt)
		{
			Logger::log(INFO, "TOPIC: made it to the inner loop");
			if (!_srvAPI.isTargetInChannel(*chIt, *userIt))
			{
				replyMessage = build_reply(SERVER_NAME, ERR_NOSUCHNICK, userNickname, *userIt, "User not in channel");
				_srvAPI.send_reply(replyMessage);
				continue ;
			}
			Logger::log(INFO, "TOPIC: the target user is in the channel");
			std::string kickMessage = build_reply(userID, "TOPIC", *chIt, *userIt, reason);
			_srvAPI.sendMessageToChannel(*chIt, kickMessage);
			_srvAPI.removeUserFromChannel(*chIt, *userIt);
			std::cout << *userIt << " was kicked the fuck out by " << userNickname << std::endl;
		}
	}
}


