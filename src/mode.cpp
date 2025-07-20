#include "CommandHandler.hpp"
#include "ChannelModeHandler.hpp"
#include "Parser.hpp"
#include "macros.hpp"
#include "Logger.hpp"
#include <vector>
#include <string>
#include <cstdlib>
#include <sstream>

void	CommandHandler::_modeFp(const parsedMessage& parsedMsg) const
{
	// Logger::log(INFO,  parsedMsg.command + " received.");
	std::string userNickname = _srvAPI.getUserNickname();
	std::string	command = parsedMsg.command;
	std::string userID = _srvAPI.getUserIdentifier(); 
	std::string mode;
	bool		status = true;


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

	std::string	param = parsedMsg.params.at(0);
	if (Parser::isNickname(param))
	{
		std::string nickname = param;
		if (!_srvAPI.doesNicknameExist(nickname))
		{
			_srvAPI.sendReply(ERR_NOSUCHNICK(userNickname, nickname));
			return ;
		}
		if (parsedMsg.params.size() == 1)
		{
			if (Parser::toLower(nickname) != Parser::toLower(userNickname))
				return ;
			mode ="+";
			// if (_srvAPI.isUserInvisibleMode())
			// 	mode += "i";
			_srvAPI.sendReply(RPL_UMODEIS(userNickname, mode));
			return ;
		}
		
		param = parsedMsg.params.at(1);
		if (Parser::toLower(nickname) != Parser::toLower(userNickname))
		{
			_srvAPI.sendReply(ERR_USERSDONTMATCH(userNickname));
			return ;
		}
		// bool iMode = _srvAPI.isUserInvisibleMode();
		for (std::string::iterator it = param.begin(); it != param.end(); it++)
		{
			if (*it == '+')
				status = true;
			else if (*it == '-')
				status = false;
			// else if (*it == 'i')
			// 	iMode = status;
			else
				_srvAPI.sendReply(ERR_UMODEUNKNOWNFLAG(userNickname));
			(void) status;
		}
		// if (iMode != _srvAPI.isUserInvisibleMode())
		// {
			// _srvAPI.setUserInvisibleMode(iMode);
			// if (iMode == true)
			// 	mode = "+i";
			// else
			// 	mode = "-i";
			// _srvAPI.sendReply(UMODE_RPL(userNickname, mode));
		// }		
	}
	else if (Parser::isChannel(param))
		ChannelModeHandler(_srvAPI, parsedMsg).handle();
	else
	{
		_srvAPI.sendReply(ERR_NOSUCHNICK(userNickname, param));
	}	
}