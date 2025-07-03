#include "CommandHandler.hpp"
#include "IServerAPI.hpp"
#include "macros.hpp"
#include "Logger.hpp"
#include <string>

void	CommandHandler::_nickFp(const parsedMessage& parsedMsg) const
{
	std::string	userNickname = _srvAPI.getUserNickname();
	std::string	command = parsedMsg.command;
	std::string	newNickname;
	std::string user_identifier;

	if (!_srvAPI.hasUserGivenPassword())
	{
		_srvAPI.sendReply(ERR_PASSWDMISMATCH(userNickname));
		return ;
	}
	
	if(parsedMsg.params.size() != 1)
	{
		_srvAPI.sendReply(ERR_NONICKNAMEGIVEN(userNickname, command));
		return ;
	}
	newNickname = parsedMsg.params.at(0);

	if (!Parser::isNickname(newNickname))
	{
		_srvAPI.sendReply(ERR_ERRONEUSNICKNAME(userNickname, newNickname));
		return ;
	}

	if (!_srvAPI.isNicknameUnique(newNickname))
	{
		if (newNickname != userNickname)
			_srvAPI.sendReply(ERR_NICKNAMEINUSE(userNickname, newNickname));
		return ;
	}

	if(_srvAPI.isUserRegistered())
	{
		user_identifier = _srvAPI.getUserIdentifier();
		_srvAPI.sendToAll(NICK_RPL(user_identifier, newNickname)); // TODO send only registered users
	}
	_srvAPI.setUserNickname(newNickname);
	if (!_srvAPI.hasUserGivenNickname())
		_srvAPI.setUserNicknameGivenStatus();
}