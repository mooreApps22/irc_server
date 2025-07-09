#include "CommandHandler.hpp"
#include "Parser.hpp"
#include "macros.hpp"
#include "Logger.hpp"
#include <string>

void	CommandHandler::_inviteFp(const parsedMessage& parsedMsg) const
{
	std::string	userNickname = _srvAPI.getUserNickname();
	std::string	command = parsedMsg.command;	
	std::string	userID = _srvAPI.getUserIdentifier();

	// Logger::log(INFO,  parsedMsg.command + " received.");

	if (!_srvAPI.isUserRegistered())
	{
		_srvAPI.sendReply(ERR_NOTREGISTERED(userNickname, command));
		return ;
	}

	if (parsedMsg.params.size() != 2)
	{
		_srvAPI.sendReply(ERR_NEEDMOREPARAMS(userNickname, command));
		return ;
	}

	std::string target = parsedMsg.params.at(0);
	if (!_srvAPI.doesNicknameExist(target))
	{
		_srvAPI.sendReply(ERR_NOSUCHNICK(userNickname, target));
		return ;
	}

	std::string channelName = parsedMsg.params.at(1);
	
	if (!_srvAPI.doesChannelExist(channelName))
	{
		_srvAPI.sendReply(ERR_NOSUCHCHANNEL(userNickname, channelName));
		return ;
	}

	std::string channelId = Parser::toLower(channelName);
	channelName = _srvAPI.getChannelName(channelId);

	if (!_srvAPI.isChannelUser(channelId))
	{
		_srvAPI.sendReply(ERR_NOTONCHANNEL(userNickname, channelName));
		return ;
	}

	if (_srvAPI.isChannelUser(channelId, target))
	{
		_srvAPI.sendReply(ERR_USERONCHANNEL(target, channelName));
		return ;
	}

	if (_srvAPI.isChannelInviteMode(channelId) && !_srvAPI.isChannelOperator(channelId))
	{
		_srvAPI.sendReply(ERR_CHANOPRIVSNEEDED(userNickname, channelName));
		return ;
	}

	_srvAPI.sendToTarget(INVITE_RPL(userID, target, channelName), target);
	_srvAPI.sendReply(RPL_INVITING(userNickname, target, channelName));

	if (_srvAPI.isChannelOperator(channelId))
		_srvAPI.addInviteeToChannel(channelId, target);
}