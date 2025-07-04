#include "CommandHandler.hpp"
#include "IServerAPI.hpp"
#include "macros.hpp"
#include "Logger.hpp"
#include <vector>
#include <string>


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
	std::vector<std::string> channelNames = Parser::splitParam(channelParam, ',');
	std::vector<std::string> keyNames = Parser::splitParam(keyParam, ',');


	if (keyParam.empty())
		keyNames.clear();

	for (paramsIt chIt = channelNames.begin(), keyIt = keyNames.begin();
		chIt != channelNames.end(); ++chIt)
	{
		std::string channelId = Parser::toLower(*chIt);
		std::string channelName;
		if (!Parser::is_channel(*chIt))
		{
			replyMessage = build_reply(SERVER_NAME, ERR_NOSUCHCHANNEL, userNickname, *chIt, "No such channel");
				_srvAPI.send_reply(replyMessage);
			continue ;
		}

		bool new_channel = false;
		if (!_srvAPI.doesChannelExist(channelId))
		{
			_srvAPI.addChannel(*chIt);

			new_channel = true;
		}

		channelName = _srvAPI.getChannelName(channelId);

		if (!new_channel)
		{
			if (_srvAPI.isChannelUser(channelId))
			{	
				std::cout << "The user is already member of this channel!" << std::endl;
				continue ;}
			
			if (_srvAPI.doesChannelHaveLimit(channelId) && _srvAPI.isChannelFull(channelId))
			{
				replyMessage = build_reply(SERVER_NAME, ERR_CHANNELISFULL, userNickname, channelName, "Cannot join channel (+l)");
				_srvAPI.send_reply(replyMessage);
				continue ;
			}

			if (_srvAPI.isChannelPasswordProtected(channelId))
			{
				bool password = false;
				if (keyIt != keyNames.end())
				{
					password = _srvAPI.isChannelPasswordValid(channelId, *keyIt);
					keyIt++;
				}
				if (!password)
				{
					replyMessage = build_reply(SERVER_NAME, ERR_BADCHANNELKEY, userNickname, channelName, "Cannot join channel (+k)");
					_srvAPI.send_reply(replyMessage);
					continue ;
				}
			}

			if (_srvAPI.isChannelInviteOnly(channelId) && !_srvAPI.isUserInvited(channelId))
			{
				replyMessage = build_reply(SERVER_NAME, ERR_INVITEONLYCHAN, userNickname, channelName, "Cannot join channel (+i)");
				_srvAPI.send_reply(replyMessage);
				continue ;				
			}
		}

		if (_srvAPI.isUserInvited(channelId))
			_srvAPI.promoteChannelInvitee(channelId);
		else
		{
			_srvAPI.addUserToChannel(channelId);
			if (new_channel)
				_srvAPI.promoteChannelMember(channelId);
		}
		replyMessage = build_reply(userID, command, channelName);
		_srvAPI.send_reply(replyMessage);
		_srvAPI.sendMessageToChannel(channelId, replyMessage);

		if (!_srvAPI.isChannelTopicSet(channelId))
			replyMessage = build_reply(SERVER_NAME, RPL_NOTOPIC, userNickname, channelName, "No topic is set");
		else
			replyMessage = build_reply(SERVER_NAME, RPL_TOPIC, userNickname, channelName, _srvAPI.getChannelTopic(channelId));
		_srvAPI.send_reply(replyMessage);

		std::string message = _srvAPI.getChannelUsersList(channelId);
		replyMessage = build_reply(SERVER_NAME, RPL_NAMREPLY, userNickname, "=", channelName, message);
		_srvAPI.send_reply(replyMessage);

		replyMessage = build_reply(SERVER_NAME, RPL_ENDOFNAMES, userNickname, channelName, "End of NAMES list");
		_srvAPI.send_reply(replyMessage);
	}
}
