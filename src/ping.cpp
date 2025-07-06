#include "CommandHandler.hpp"
#include "IServerAPI.hpp"
#include "macros.hpp"
#include <string>

void	CommandHandler::_pingFp(const parsedMessage& parsedMsg) const
{
	std::string replyMessage;
	std::string userNickname = _srvAPI.getUserNickname();

	if (parsedMsg.params.size() == 0)
		_srvAPI.sendReply(ERR_NOORIGIN(userNickname));
	else
		_srvAPI.sendReply(PONG_RPL(parsedMsg.params.at(0)));
}