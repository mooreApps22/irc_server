#include "CommandHandler.hpp"
#include "Parser.hpp"
#include "macros.hpp"
#include "User.hpp"
#include <string>


void	CommandHandler::_quitFp(const parsedMessage& parsedMsg) const
{
	std::string	userNickname = _srvAPI.getUserNickname();
	std::string	userHost = _srvAPI.getUserHost();
	std::string	userID = _srvAPI.getUserIdentifier();
	std::string	quitMessage;

	User::channels clientChannels = _srvAPI.getUserChannels();

	if (parsedMsg.params.size() == 1)
		quitMessage = parsedMsg.params[0];

	_srvAPI.sendReply(ERROR_RPL(userNickname, userHost));
	for (User::channels::const_iterator it = clientChannels.begin(); it != clientChannels.end(); it++)
	{
		if (_srvAPI.isChannelUser(*it))
		{
			_srvAPI.sendMessageToChannel(*it, QUIT_RPL(userID, userNickname,quitMessage));
		}
	}
	_srvAPI.disconnectClientFromServer();
}