#include "CommandHandler.hpp"
#include "Parser.hpp"
#include "macros.hpp"
#include "Logger.hpp"
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

void	CommandHandler::_topicFp(const parsedMessage& parsedMsg) const
{
	// Logger::log(INFO,  parsedMsg.command + " received.");

	std::string	userNickname = _srvAPI.getUserNickname();
	std::string	command = parsedMsg.command;	
	std::string	userID = _srvAPI.getUserIdentifier();

	if (!_srvAPI.isUserRegistered())
	{
		_srvAPI.sendReply(ERR_NOTREGISTERED(userNickname, command));
		return ;
	}
	// Logger::log(INFO, "TOPIC: user is registered");
	if (parsedMsg.params.size() != 1 && parsedMsg.params.size() != 2)
	{
		_srvAPI.sendReply(ERR_NEEDMOREPARAMS(userNickname, command));
		return ;
	}
	// Logger::log(INFO, "TOPIC: has enough params");

	std::string channelName = parsedMsg.params[0];

	if (!_srvAPI.doesChannelExist(channelName))
	{
		_srvAPI.sendReply(ERR_NOSUCHCHANNEL(userNickname, channelName));
		return ;
	}

	std::string channelId = Parser::toLower(channelName);
	channelName = _srvAPI.getChannelName(channelId);

	if (parsedMsg.params.size() == 1)
	{
		// Logger::log(INFO, "TOPIC: ready to get topic");
		if (!_srvAPI.isChannelTopicSet(channelId))
			_srvAPI.sendReply(RPL_NOTOPIC(userNickname, channelName));
		else
			_srvAPI.sendReply(RPL_TOPIC(userNickname, channelName, _srvAPI.getChannelTopic(channelId)));
	}
	else if (parsedMsg.params.size() == 2)
	{
		// Logger::log(INFO, "TOPIC: ready to set topic");

		if (!_srvAPI.isChannelUser(channelId))
		{
			_srvAPI.sendReply(ERR_NOTONCHANNEL(userNickname, channelName));
			return ;
		}

		std::string topic = parsedMsg.params[1];
		if (_srvAPI.isChannelTopicMode(channelId) && !_srvAPI.isChannelOperator(channelId))
		{
			_srvAPI.sendReply(ERR_CHANOPRIVSNEEDED(userNickname, channelName));
			return ;
		}
		_srvAPI.setChannelTopic(channelId, topic);
		_srvAPI.sendReply(TOPIC_RPL(userID, channelName, topic));
		_srvAPI.sendMessageToChannel(TOPIC_RPL(userID, channelName, topic), channelId);
	}
}
