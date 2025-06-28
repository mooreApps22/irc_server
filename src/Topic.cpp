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
	   
		TOPIC #c :new topic
		:al!~a@C453D4D.74B8F9F9.CB7972B2.IP TOPIC #c :new topic
		->>		<userID> <command> <channelName> <topic>
		TOPIC
		:Rubicon.GeekShed.net 461 al TOPIC :Not enough parameters
		TOPIC #c
		:Rubicon.GeekShed.net 332 al #c :new topic
		->>		SERVERNAME RPL_TOPIC userNickname channelName topic
		:Rubicon.GeekShed.net 333 al #c al 1751132903
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
	if (parsed_msg.params.size() != 1 && parsed_msg.params.size() != 2)
	{
		replyMessage = build_reply(SERVER_NAME, ERR_NEEDMOREPARAMS, userNickname, command, "Not enough parameters");
		_srvAPI.send_reply(replyMessage);
		return ;
	}
	Logger::log(INFO, "TOPIC: has enough params");

	std::string channelName = parsed_msg.params[0];

	if (parsed_msg.params.size() == 1)
	{
		Logger::log(INFO, "TOPIC: ready to get topic");
		replyMessage = build_reply(SERVER_NAME, RPL_TOPIC, userNickname, channelName, _srvAPI.getChannelTopic(channelName));
		_srvAPI.send_reply(replyMessage);
	}
	else if (parsed_msg.params.size() == 2)
	{
		Logger::log(INFO, "TOPIC: ready to set topic");
		if (!_srvAPI.isChannelTopicProtected(channelName) || _srvAPI.isUserChannelOperator(channelName))
		{
			std::string topic = parsed_msg.params[1];

			_srvAPI.setNewTopic(channelName, topic);
			replyMessage = build_reply(userID, command, channelName, topic);
			_srvAPI.send_reply(replyMessage);
			_srvAPI.sendMessageToChannel(channelName, replyMessage);
		}
	}
}
