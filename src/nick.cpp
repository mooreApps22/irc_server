#include "CommandHandler.hpp"
#include "Parser.hpp"
#include "macros.hpp"
#include "Logger.hpp"
#include <string>

void	CommandHandler::_nickFp(const parsedMessage& parsedMsg) const
{
	std::string	userNickname = _srvAPI.getUserNickname();
	std::string	command = parsedMsg.command;
	std::string	newNickname;
	std::string userID;

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

	if (_srvAPI.doesNicknameExist(newNickname))
	{
		if (newNickname != userNickname)
			_srvAPI.sendReply(ERR_NICKNAMEINUSE(userNickname, newNickname));
		return ;
	}

	if(_srvAPI.isUserRegistered())
	{
		userID = _srvAPI.getUserIdentifier();
		_srvAPI.sendReply(NICK_RPL(userID, newNickname));
		_srvAPI.sendMessageToChannelsWhereUser(NICK_RPL(userID, newNickname));
	}
	_srvAPI.setUserNickname(newNickname);
	if (!_srvAPI.hasUserGivenNickname())
		_srvAPI.setUserNicknameGivenStatus();
}