#include "CommandHandler.hpp"
#include "Parser.hpp"
#include "macros.hpp"
#include "Logger.hpp"
#include <vector>
#include <string>

/*
	KICK <channel>,<channel> <user>,<user> [comment]
*/

void	CommandHandler::_kickFp(const parsedMessage& parsedMsg) const
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
	// Logger::log(INFO, "KICK: user is registered");

	if (parsedMsg.params.size() < 2)
	{
		_srvAPI.sendReply(ERR_NEEDMOREPARAMS(userNickname, command));
		return ;
	}
	// Logger::log(INFO, "KICK: has enough params");

	std::vector<std::string> channelNames = Parser::splitParam(parsedMsg.params[0], ',');
	std::vector<std::string> userNames = Parser::splitParam(parsedMsg.params[1], ',');

	if ((channelNames.size() != userNames.size()) && !(channelNames.size() == 1 && userNames.size() > 0))
	{
		_srvAPI.sendReply(ERR_NEEDMOREPARAMS(userNickname, command));
		return ;
	}
	// Logger::log(INFO, "KICK: users >= channels");


	std::string	reason;

	if (parsedMsg.params.size() > 2)
		reason = parsedMsg.params[2];
	else
		reason = userNickname;

	//Channel Loop
	for (std::vector<std::string>::iterator chIt = channelNames.begin(); chIt != channelNames.end(); ++chIt)
	{
		// Logger::log(INFO, "KICK: Made it to the outer loop.");
		if (!_srvAPI.doesChannelExist(*chIt))
		{
			_srvAPI.sendReply(ERR_NOSUCHCHANNEL(userNickname, *chIt));
			continue ;	
		}
		std::string channelId = Parser::toLower(*chIt);
		std::string channelName = _srvAPI.getChannelName(channelId);

		// Logger::log(INFO, "KICK: channel param exist");
		if (!_srvAPI.isChannelUser(channelId))
		{
			_srvAPI.sendReply(ERR_NOTONCHANNEL(userNickname, channelName));
			continue ;
		}
		// Logger::log(INFO, "KICK: Kicker is on Channel");
		if (!_srvAPI.isChannelOperator(channelId))
		{
			_srvAPI.sendReply(ERR_CHANOPRIVSNEEDED(userNickname, channelName));
			continue ;
		}
		// Logger::log(INFO, "KICK: Kicker is Channel Operator");
		// User Loop
		for (std::vector<std::string>::iterator userIt = userNames.begin(); userIt != userNames.end(); ++userIt)
		{
			// Logger::log(INFO, "KICK: made it to the inner loop");
			if (!_srvAPI.isChannelUser(channelId, *userIt))
			{
				_srvAPI.sendReply(ERR_USERNOTINCHANNEL(*userIt, channelName));
				continue ;
			}
			// Logger::log(INFO, "KICK: the target user is in the channel");
			_srvAPI.sendReply(KICK_RPL(userID, channelName, *userIt, reason));
			_srvAPI.sendMessageToChannel(KICK_RPL(userID, channelName, *userIt, reason), channelId);
			_srvAPI.removeUserFromChannel(channelId, *userIt);
		}
	}
}


