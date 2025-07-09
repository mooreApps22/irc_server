#include "CommandHandler.hpp"
#include "Parser.hpp"
#include "macros.hpp"
#include "Logger.hpp"
#include <vector>
#include <string>

/*
	PART <channel>,<channel> [Part Message]
	
   The PART command causes the user sending the message to be removed
   from the list of active members for all given channels listed in the
   parameter string.  If a "Part Message" is given, this will be sent
   instead of the default message, the nickname.  This request is always
   granted by the server.
   Servers MUST be able to parse arguments in the form of a list of
   target, but SHOULD NOT use lists when sending PART messages to
   clients.
   Numeric Replies:
		   ERR_NEEDMOREPARAMS 
		   ERR_NOSUCHCHANNEL
		   ERR_NOTONCHANNEL
*/

void	CommandHandler::_partFp(const parsedMessage& parsedMsg) const
{
	std::string	userNickname = _srvAPI.getUserNickname();
	std::string	command = parsedMsg.command;	
	std::string	userID = _srvAPI.getUserIdentifier();
	std::string	partMessage;

	if (!_srvAPI.isUserRegistered())
	{
		_srvAPI.sendReply(ERR_NOTREGISTERED(userNickname, command));
		return ;
	}
	
	// Logger::log(DEBUG, "PART: user is registered");

	if (parsedMsg.params.size() != 1 && parsedMsg.params.size() != 2)
	{
		_srvAPI.sendReply(ERR_NEEDMOREPARAMS(userNickname, command));
		return ;
	}

	// Logger::log(DEBUG, "PART: has enough params");
	
	std::vector<std::string> channelNames = Parser::splitParam(parsedMsg.params[0], ',');

	if (parsedMsg.params.size() == 2)
		partMessage = parsedMsg.params[1];

	//Channel Loop
	for (std::vector<std::string>::iterator chIt = channelNames.begin(); chIt != channelNames.end(); ++chIt)
	{
		// Logger::log(DEBUG, "PART: Made it to the outer loop.");
		if (!_srvAPI.doesChannelExist(*chIt))
		{
			_srvAPI.sendReply(ERR_NOSUCHCHANNEL(userNickname, *chIt));
			continue ;	
		}

		std::string channelId = Parser::toLower(*chIt);
		std::string channelName = _srvAPI.getChannelName(channelId);

		if (!_srvAPI.isChannelUser(channelId))
		{
			_srvAPI.sendReply(ERR_NOTONCHANNEL(userNickname, channelName));
			continue ;	
		}
		
		// Perform PART for this channel
		//<userID> PART <channelName>
		_srvAPI.sendReply(PART_RPL(userID, channelName, partMessage));
		_srvAPI.sendMessageToChannel(PART_RPL(userID, channelName, partMessage), channelId);
		_srvAPI.removeUserFromChannel(channelId);
	}
}
