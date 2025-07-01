#include "CommandHandler.hpp"
#include "IServerAPI.hpp"
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

void	CommandHandler::_partFp(parsed_message& parsed_msg)
{
	Logger::log(INFO,  parsed_msg.command + " received.");
	std::cout << "PART command is parsing." << std::endl;

	std::string	replyMessage;
	std::string	partMessage;
	std::string	userNickname = _srvAPI.getUserNick();
	std::string	command = parsed_msg.command;	
	std::string	userID = _srvAPI.getUserIdentifier();

	if (!_srvAPI.isUserRegistered())
	{
		replyMessage = build_reply(SERVER_NAME, ERR_NOTREGISTERED, userNickname, command, "You have not registered");
		_srvAPI.send_reply(replyMessage);
		return ;
	}
	
	Logger::log(DEBUG, "PART: user is registered");

	if (parsed_msg.params.size() != 1 && parsed_msg.params.size() != 2)
	{
		replyMessage = build_reply(SERVER_NAME, ERR_NEEDMOREPARAMS, userNickname, command, "Not enough parameters");
		_srvAPI.send_reply(replyMessage);
		return ;
	}

	Logger::log(DEBUG, "PART: has enough params");
	
	std::vector<std::string> channelNames = Parser::splitParam(parsed_msg.params[0], ',');

	if (parsed_msg.params.size() == 2)
		partMessage = parsed_msg.params[1];

	//Channel Loop
	for (paramsIt chIt = channelNames.begin(); chIt != channelNames.end(); ++chIt)
	{
		std::string channelId = Parser::toLower(*chIt);

		Logger::log(DEBUG, "PART: Made it to the outer loop.");

		if (!_srvAPI.doesChannelExist(channelId))
		{
			replyMessage = build_reply(SERVER_NAME, ERR_NOSUCHCHANNEL, userNickname, *chIt, "No such channel");
			_srvAPI.send_reply(replyMessage);
			continue ;	
		}

		std::string channelName = _srvAPI.getChannelName(channelId);

		if (!_srvAPI.isChannelUser(channelId))
		{
			replyMessage = build_reply(SERVER_NAME, ERR_NOTONCHANNEL, userNickname, *chIt, "You're not on that channel");
			_srvAPI.send_reply(replyMessage);
			continue ;	
		}
		// Perform PART for this channel
		//<userID> PART <channelName>
		if (parsed_msg.params.size() == 2)
			replyMessage = build_reply(userID, command, channelName, partMessage);
		else
			replyMessage = build_reply(userID, command, channelName);
		_srvAPI.send_reply(replyMessage);
		_srvAPI.sendMessageToChannel(channelId, replyMessage);
		_srvAPI.removeUserFromChannel(channelId);
	}
	
}
