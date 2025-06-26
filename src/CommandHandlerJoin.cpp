#include "CommandHandler.hpp"
#include "IServerAPI.hpp"
#include "macros.hpp"
#include "Logger.hpp"
#include <vector>
#include <string>
/*
	#define ERR_INVITEONLYCHAN 		"473"	// "<channel> :Cannot join channel (+i)"
	#define ERR_CHANNELISFULL 		"471"	// "<channel> :Cannot join channel (+l)"
	#define ERR_NOSUCHCHANNEL 		"403"	// "<channel name> :No such channel"
	#define ERR_BADCHANNELKEY 		"475"	// "<channel> :Cannot join channel (+k)"
	#define ERR_TOOMANYCHANNELS 	"405"	// "<channel name> :You have joined too many channels"
*/

// TODO: Delete (duplicated from Parser method())
bool	CommandHandler::isChString(std::string::iterator it)
{
	return *it != ' ' && *it != 7 && *it != '\0' && *it != '\r' && *it != '\n' && *it != ',';
}

// TODO: Delete (duplicated from Parser method())
bool	CommandHandler::isValidChannelName(std::string& channel)
{
	std::string::iterator it = channel.begin();

	if (*it != '#' && *it != '&')
		return false;
	it++;
	for(; it != channel.end(); it++)
	{
		if (!isChString(it))
			return false;
	}
	return true;
}

std::vector<std::string>	mySplit(const std::string& str, char delimiter)
{
	std::vector<std::string>	split;
	std::string::size_type		start = 0;
	std::string::size_type		end;

	while ((end = str.find(delimiter, start)) != std::string::npos)
	{
		split.push_back(str.substr(start, end - start));
		start = end + 1;
	}
	split.push_back(str.substr(start));
	return (split);
}

void	CommandHandler::_joinFp(parsed_message& parsed_msg)
{
	Logger::log(INFO,  parsed_msg.command + " received.");
	
	std::string	replyMessage;
	std::string	userNickname = _srvAPI.getUserNick();
	std::string	command = parsed_msg.command;	

	if (!_srvAPI.isUserRegistered())
	{
		replyMessage = build_reply(std::string(SERVER_NAME), std::string(ERR_NOTREGISTERED), userNickname, command, "You have not registered");
		_srvAPI.send_reply(replyMessage);
		return ;
	}
	if (parsed_msg.params.size() == 0)
	{
		replyMessage = build_reply(SERVER_NAME, ERR_NEEDMOREPARAMS, userNickname, command, "Not enough parameters");
		_srvAPI.send_reply(replyMessage);
		return ;
	}

	std::string channelParam = parsed_msg.params[0];
	std::string keyParam = (parsed_msg.params.size() > 1) ? parsed_msg.params[1] : "";
	std::vector<std::string> channelNames = mySplit(channelParam, ','); // TODO Refactor yo Parser
	std::vector<std::string> keyNames = mySplit(keyParam, ','); // TODO Refactor yo Parser

	if (keyParam.empty())
		keyNames.clear();

	for (paramsIt chIt = channelNames.begin(), keyIt = keyNames.begin();
		chIt != channelNames.end(); ++chIt) //, keyIt = keyNames.begin()
	{
		std::cout << "Loop::Channels: " << *chIt << std::endl;
		//if channel name is valid
		if (!isValidChannelName(*chIt)) // TODO: Change to -if (!Parser::is_channel(*chIt))	-
		{
			replyMessage = build_reply(SERVER_NAME, ERR_NOSUCHCHANNEL, userNickname, *chIt, "No such channel");
				_srvAPI.send_reply(replyMessage);
			continue ;
		}
		//if channel (aka *it) doesnt't exist, create it and make the user the operator
		if (!_srvAPI.doesChannelExist(*chIt))
		{
			_srvAPI.addChannel(*chIt);
			_srvAPI.send_reply("Creating a channel:" + *chIt);
			_srvAPI.addUserToChannel(*chIt);
			_srvAPI.setUserAsOperator(*chIt);
			// _srvAPI.send_reply("You were added to " + *chIt);
			continue ;
		}

		//if key is set the key must match (if a key is used it must be iterated)
		if (_srvAPI.isChannelPasswordProtected(*chIt))
		{
			bool password = false;
			if (keyIt != keyNames.end())
			{
				password = _srvAPI.isChannelPasswordValid(*chIt, *keyIt);
				keyIt++;
			}
			if (!password)
			{
				replyMessage = build_reply(SERVER_NAME, ERR_BADCHANNELKEY, userNickname, *chIt, "Cannot join channel (+k)");
				_srvAPI.send_reply(replyMessage);
				continue ;
			}
		}
		
		//if the channel isInviteOnly() then don't allow to join
		if (_srvAPI.isChannelInviteOnly(*chIt) && !_srvAPI.isUserInvited(*chIt))
		{
			replyMessage = build_reply(SERVER_NAME, ERR_INVITEONLYCHAN, userNickname, *chIt, "Cannot join channel (+i)");
			_srvAPI.send_reply(replyMessage);
			continue ;
		}
		
		//if the channel isFull() then don't allow to join
		if (_srvAPI.doesChannelHaveLimit(*chIt) && _srvAPI.isChannelFull(*chIt))
		{
			replyMessage = build_reply(SERVER_NAME, ERR_CHANNELISFULL, userNickname, *chIt, "Cannot join channel (+l)");
			_srvAPI.send_reply(replyMessage);
			continue ;
		}
		_srvAPI.addUserToChannel(*chIt);
	}
}
