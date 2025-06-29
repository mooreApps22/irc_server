#include "CommandHandler.hpp"
#include "IServerAPI.hpp"
#include "macros.hpp"
#include "Logger.hpp"
#include <vector>
#include <string>

void	CommandHandler::_inviteFp(parsed_message& parsed_msg)
{
	std::string	replyMessage;
	std::string	userNickname = _srvAPI.getUserNick();
	std::string	command = parsed_msg.command;	
	std::string	userID = _srvAPI.getUserIdentifier();

	Logger::log(INFO,  parsed_msg.command + " received.");

	if (!_srvAPI.isUserRegistered())
	{
		replyMessage = build_reply(std::string(SERVER_NAME), std::string(ERR_NOTREGISTERED), userNickname, command, "You have not registered");
		_srvAPI.send_reply(replyMessage);
		return ;
	}

	if (parsed_msg.params.size() != 2)
	{
		replyMessage = build_reply(SERVER_NAME, ERR_NEEDMOREPARAMS, userNickname, command, "Not enough parameters");
		_srvAPI.send_reply(replyMessage);
		return ;
	}

	std::string target = parsed_msg.params.at(0);
	if (isNickUnique(target))
	{
		replyMessage = build_reply(SERVER_NAME, ERR_NOSUCHNICK, userNickname, target, "No such nick/channel");
		_srvAPI.send_reply(replyMessage);
		return ;
	}

	std::string channelName = parsed_msg.params.at(1);
	if (!_srvAPI.doesChannelExist(channelName))
	{
		replyMessage = build_reply(SERVER_NAME, ERR_NOSUCHCHANNEL, userNickname, channelName, "No such channel");
		_srvAPI.send_reply(replyMessage);
		return;
	}

	if (!_srvAPI.isChannelUser(channelName))
	{
		replyMessage = build_reply(SERVER_NAME, ERR_NOTONCHANNEL, userNickname, channelName, "You're not on that channel");
		_srvAPI.send_reply(replyMessage);
		return;
	}

	if (_srvAPI.isTargetInChannel(channelName, target))
	{
		replyMessage = build_reply(SERVER_NAME, ERR_USERONCHANNEL, target, channelName, "is already on channel");
		_srvAPI.send_reply(replyMessage);
		return;
	}

	if (_srvAPI.isChannelInviteOnly(channelName) && !_srvAPI.isUserChannelOperator(channelName))
	{
			replyMessage = build_reply(SERVER_NAME, ERR_CHANOPRIVSNEEDED, userNickname, channelName, "You're not channel operator");
			_srvAPI.send_reply(replyMessage);
			return;
	}
	replyMessage = build_reply(userID, command, target, channelName);
	_srvAPI.sendToUser(replyMessage, target);

	replyMessage = build_reply(SERVER_NAME, RPL_INVITING, userNickname, target, channelName);
	_srvAPI.send_reply(replyMessage);
	_srvAPI.sendToUser(replyMessage, target);


	if (_srvAPI.isUserChannelOperator(channelName))
		_srvAPI.addInviteeToChannel(channelName, target);
	
}