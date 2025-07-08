#include "CommandHandler.hpp"
#include "IServerAPI.hpp"
#include "Logger.hpp"
#include "macros.hpp"
#include "User.hpp"
#include <string>

CommandHandler::CommandHandler(IServerAPI& srvAPI)
	:	_srvAPI(srvAPI)
{
	// Logger::log(INFO, "CommandHandler constructed.");
	_commands["INVITE"]		= &CommandHandler::_inviteFp;
	_commands["JOIN"]		= &CommandHandler::_joinFp;
	_commands["KICK"]		= &CommandHandler::_kickFp;
	_commands["MODE"]		= &CommandHandler::_modeFp;
	_commands["NICK"]		= &CommandHandler::_nickFp;
	_commands["PASS"]		= &CommandHandler::_passFp;
	_commands["PING"]		= &CommandHandler::_pingFp;
	_commands["PRIVMSG"]	= &CommandHandler::_privMsgFp;
	_commands["TOPIC"] 		= &CommandHandler::_topicFp;
	_commands["USER"] 		= &CommandHandler::_userFp;
	_commands["PART"] 		= &CommandHandler::_partFp;
	_commands["QUIT"] 		= &CommandHandler::_quitFp;
}

CommandHandler::~CommandHandler()
{
	// Logger::log(INFO, "CommandHandler destructed.");
}

void	CommandHandler::execute(const parsedMessage& parsedMsg) const
{
	const std::string command =  parsedMsg.command;
	// Logger::log(INFO, "CommandHandler::execute() called.", command);

	commands::const_iterator it = _commands.find(command);

	if (it != _commands.end())
	{
		(this->*(it->second))(parsedMsg);
	}
	else
	{
		// Logger::log(INFO, "Unknown Command.", command);
		const std::string userNickname = _srvAPI.getUserNickname();
		_srvAPI.sendReply(ERR_UNKNOWNCOMMAND(userNickname, command));
	}
}

void	CommandHandler::disconnectClient() const
{
	std::string	userNickname = _srvAPI.getUserNickname();
	std::string	userID = _srvAPI.getUserIdentifier();
	
	_srvAPI.sendMessageToChannelsWhereUser(EXIT_RPL(userID));
	_srvAPI.disconnectClientFromServer();
}

void	CommandHandler::disconnectServer() const
{
	_srvAPI.sendToAll(SERVER_ERROR_RPL);
	_srvAPI.disconnectAllClientsFromServer();
}
