#include "CommandHandler.hpp"
#include "IServerAPI.hpp"
#include "macros.hpp"
#include "Logger.hpp"
#include <vector>
#include <string>

/* 
		Once a user has joined a channel, they receive notice about all
	   commands their server receives which affect the channel.  This
	   includes MODE, KICK, PART, QUIT and of course PRIVMSG/NOTICE.  The
	   JOIN command needs to be broadcast to all servers so that each server
	   knows where to find the users who are on the channel.  This allows
	   optimal delivery of PRIVMSG/NOTICE messages to the channel.

	   If a JOIN is successful, the user is then sent the channel's topic
	   (using RPL_TOPIC) and the list of users who are on the channel (using
	   RPL_NAMREPLY), which must include the user joining.
*/

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
	std::string	userID = _srvAPI.getUserIdentifier();


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
		if (!Parser::is_channel(*chIt))
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
			replyMessage = build_reply(userID, command, *chIt);
			_srvAPI.send_reply(replyMessage);

			if (!_srvAPI.isChannelTopicSet(*chIt))
				replyMessage = build_reply(SERVER_NAME, RPL_NOTOPIC, userNickname, *chIt, "No topic is set");
			else
				replyMessage = build_reply(SERVER_NAME, RPL_TOPIC, userNickname, *chIt, _srvAPI.getChannelTopic(*chIt));
			_srvAPI.send_reply(replyMessage);

			std::string message = _srvAPI.getChannelUsersList(*chIt);
			replyMessage = build_reply(SERVER_NAME, RPL_NAMREPLY, userNickname, "=", *chIt, message);
			_srvAPI.send_reply(replyMessage);

			replyMessage = build_reply(SERVER_NAME, RPL_ENDOFNAMES, userNickname, *chIt, "End of NAMES list");
			_srvAPI.send_reply(replyMessage);

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
		replyMessage = build_reply(userID, command, *chIt);
		_srvAPI.send_reply(replyMessage);

		if (!_srvAPI.isChannelTopicSet(*chIt))
			replyMessage = build_reply(SERVER_NAME, RPL_NOTOPIC, userNickname, *chIt, "No topic is set");
		else
			replyMessage = build_reply(SERVER_NAME, RPL_TOPIC, userNickname, *chIt, _srvAPI.getChannelTopic(*chIt));
		_srvAPI.send_reply(replyMessage);

		std::string message = _srvAPI.getChannelUsersList(*chIt);
		replyMessage = build_reply(SERVER_NAME, RPL_NAMREPLY, userNickname, "=", *chIt, message);
		_srvAPI.send_reply(replyMessage);

		replyMessage = build_reply(SERVER_NAME, RPL_ENDOFNAMES, userNickname, *chIt, "End of NAMES list");
		_srvAPI.send_reply(replyMessage);


	}
}
