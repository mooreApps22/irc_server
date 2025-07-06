#include "CommandHandler.hpp"
#include "Parser.hpp"
#include "macros.hpp"
#include "Logger.hpp"
#include <string>

void	CommandHandler::_userFp(const parsedMessage& parsedMsg) const
{
	std::string	userNickname = _srvAPI.getUserNickname();
	std::string	command = parsedMsg.command;
	std::string username;
	
	if(!_srvAPI.hasUserGivenNickname())
	{
		_srvAPI.sendReply(ERR_NONICKNAMEGIVEN(userNickname, command));
		return ;
	}

	if(_srvAPI.isUserRegistered())
	{
		_srvAPI.sendReply(ERR_ALREADYREGISTRED(userNickname));
		return ;
	}
	
	if(parsedMsg.params.size() < 4)
	{
		_srvAPI.sendReply(ERR_NEEDMOREPARAMS(userNickname, command));
		return ;
	}

	username = parsedMsg.params.at(0);
	_srvAPI.setUserUsername(username);
	_srvAPI.setUserRegisteredStatus();

 	_srvAPI.sendReply(RPL_WELCOME(userNickname));
	_srvAPI.sendReply(RPL_YOURHOST(userNickname));
	_srvAPI.sendReply(RPL_CREATED(userNickname));
	_srvAPI.sendReply(RPL_MYINFO(userNickname));
}
