#include "CommandHandler.hpp"
#include "Parser.hpp"
#include "macros.hpp"
#include "Logger.hpp"
#include <vector>
#include <string>


void	CommandHandler::_joinFp(const parsedMessage& parsedMsg) const
{
	std::string	userNickname = _srvAPI.getUserNickname();
	std::string	command = parsedMsg.command;	
	std::string	userID = _srvAPI.getUserIdentifier();

	if (!_srvAPI.isUserRegistered())
	{
		_srvAPI.sendReply(ERR_NOTREGISTERED(userNickname, command));
		return ;
	}
	
	if (parsedMsg.params.size() == 0)
	{
		_srvAPI.sendReply(ERR_NEEDMOREPARAMS(userNickname, command));
		return ;
	}

	std::string channelParam = parsedMsg.params[0];
	std::string keyParam = (parsedMsg.params.size() > 1) ? parsedMsg.params[1] : "";
	std::vector<std::string> channelNames = Parser::splitParam(channelParam, ',');
	std::vector<std::string> keyNames = Parser::splitParam(keyParam, ',');


	if (keyParam.empty())
		keyNames.clear();

	for (std::vector<std::string>::iterator chIt = channelNames.begin(), keyIt = keyNames.begin();
		chIt != channelNames.end(); ++chIt)
	{
		if (!Parser::isChannel(*chIt))
		{
			_srvAPI.sendReply(ERR_NOSUCHCHANNEL(userNickname, *chIt));
			continue ;
		}

		bool newChannel = false;
		if (!_srvAPI.doesChannelExist(*chIt))
		{
			_srvAPI.addChannel(*chIt);
			newChannel = true;
		}

		std::string	channelId = Parser::toLower(*chIt);
		std::string channelName = _srvAPI.getChannelName(channelId);

		if (!newChannel)
		{
			if (_srvAPI.isChannelUser(channelId))
				continue ;
			
			if (_srvAPI.doesChannelHaveLimit(channelId) && _srvAPI.isChannelFull(channelId))
			{
				_srvAPI.sendReply(ERR_CHANNELISFULL(userNickname, channelName));	// TODO test when mode +l
				continue ;
			}

			if (_srvAPI.isChannelKeyProtected(channelId))
			{
				bool password = false;
				if (keyIt != keyNames.end())
				{
					password = _srvAPI.isChannelKeyValid(channelId, *keyIt);
					keyIt++;
				}
				if (!password)
				{
					_srvAPI.sendReply(ERR_BADCHANNELKEY(userNickname, channelName));	// TODO test when mode +k
					continue ;
				}
			}

			if (_srvAPI.isChannelInviteOnly(channelId) && !_srvAPI.isUserInvitee(channelId))
			{
				_srvAPI.sendReply(ERR_INVITEONLYCHAN(userNickname, channelName));	// TODO test when mode +i
				continue ;				
			}
		}

		if (_srvAPI.isUserInvitee(channelId))
			_srvAPI.promoteChannelInvitee(channelId);
		else
		{
			_srvAPI.addUserToChannel(channelId);
			if (newChannel)
				_srvAPI.promoteChannelMember(channelId);
		}
		_srvAPI.sendReply(JOIN_RPL(userID, channelName));
		_srvAPI.sendMessageToChannel(JOIN_RPL(userID, channelName), channelId);

		if (!_srvAPI.isChannelTopicSet(channelId))
			_srvAPI.sendReply(RPL_NOTOPIC(userNickname, channelName));
		else
			_srvAPI.sendReply(RPL_TOPIC(userNickname, channelName, _srvAPI.getChannelTopic(channelId)));	// TODO test when TOPIC is done

		_srvAPI.sendReply(RPL_NAMREPLY(userNickname, channelName, _srvAPI.getChannelUsersList(channelId)));

		
		_srvAPI.sendReply(RPL_ENDOFNAMES(userNickname, channelName));
	}
}
