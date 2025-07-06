#include "CommandHandler.hpp"
#include "Parser.hpp"
#include "Logger.hpp"
#include "macros.hpp"
#include <string>


void	CommandHandler::_passFp(const parsedMessage& parsedMsg) const
{
	std::string	userNickname = _srvAPI.getUserNickname();
	std::string	command = parsedMsg.command;

	if(_srvAPI.isUserRegistered())
	{
		_srvAPI.sendReply(ERR_ALREADYREGISTRED(userNickname));
		return ;
	}
	
	if(parsedMsg.params.size() != 1)
	{
		_srvAPI.sendReply(ERR_PASSWDMISMATCH(userNickname));
		return ;
	}

	if(_srvAPI.isPasswordValid(parsedMsg.params.at(0)))
		_srvAPI.setUserPasswordGivenStatus();
	else
		_srvAPI.sendReply(ERR_PASSWDMISMATCH(userNickname));
}