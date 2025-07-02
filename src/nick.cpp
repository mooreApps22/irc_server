#include "CommandHandler.hpp"
#include "IServerAPI.hpp"
#include "macros.hpp"
#include "Logger.hpp"
#include <string>

void	CommandHandler::_nickFp(const parsedMessage& parsedMsg) const
{
	std::string	userNickname = _srvAPI.getUserNickname();
	std::string	command = parsedMsg.command;
	std::string	new_nickname;
	std::string user_identifier;

	if (!_srvAPI.getUserPasswordState())
	{
		_srvAPI.sendReply(ERR_PASSWDMISMATCH(userNickname));
		return ;
	}
	
	if(parsedMsg.params.size() != 1)
	{
		_srvAPI.sendReply(ERR_NONICKNAMEGIVEN(userNickname, command));
		return ;
	}
	new_nickname = parsedMsg.params.at(0);
// 	Logger::log(DEBUG, "Checking nickname correctness");

	if (!Parser::isNickname(new_nickname))
	{
		_srvAPI.sendReply(ERR_ERRONEUSNICKNAME(userNickname, new_nickname));
		return ;
	}

// 	Logger::log(DEBUG, "Checking nick uniqueness");
	if (!_srvAPI.isNicknameUnique(new_nickname))
	{
		if (new_nickname != userNickname)
			_srvAPI.sendReply(ERR_NICKNAMEINUSE(userNickname, new_nickname));

		return ;
	}

	if(_srvAPI.isUserRegistered())
	{
		user_identifier = _srvAPI.getUserIdentifier();
		_srvAPI.sendToAll(NICK_RPL(user_identifier, new_nickname)); // TODO test only registered users!!!
	}
	_srvAPI.setUserNickname(new_nickname);
}