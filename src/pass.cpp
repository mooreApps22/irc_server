#include "CommandHandler.hpp"
#include "Logger.hpp"
#include "macros.hpp"
#include <string>


void	CommandHandler::_passFp(const parsedMessage& parsedMsg) const
{
	// Logger::log(INFO,  parsedMsg.command + " received.");
	
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
		// _srvAPI.disconnectUser();
		return ;
	}

	if(_srvAPI.isPasswordValid(parsedMsg.params.at(0)))
		_srvAPI.setUserPasswordState(true);
	else
	{
		_srvAPI.sendReply(ERR_PASSWDMISMATCH(userNickname));
		// _srvAPI.disconnectUser();
	}
}