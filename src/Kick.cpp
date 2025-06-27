#include "CommandHandler.hpp"
#include "IServerAPI.hpp"
#include "macros.hpp"
#include "Logger.hpp"
#include <vector>
#include <string>

void	CommandHandler::_kickFp(parsed_message& parsed_msg)
{
	Logger::log(INFO,  parsed_msg.command + " received.");

	/*
		KICK <channel>,<channel> <user>,<user> [comment]
			replyMessage = build_reply(SERVER_NAME, RPL_ENDOFNAMES, userNickname, *chIt, "End of NAMES list");
			_srvAPI.send_reply(replyMessage);
		}
		
	*/

	std::string	replyMessage;
	std::string	userNickname = _srvAPI.getUserNick();
	std::string	command = parsed_msg.command;	
	std::string	userID = _srvAPI.getUserIdentifier();

	std::string channelParam = parsed_msg.params[0];
	std::vector<std::string> channelNames = Parser::splitParam(channelParam, ',');


	for (paramsIt chIt = channelNames.begin(); chIt != channelNames.end(); ++chIt)
	{
		if (_srvAPI.doesChannelExist(*chIt))
		{
			std::string userParam = parsed_msg.params[0];
			std::vector<std::string> userNames = Parser::splitParam(userParam, ',');

			for (paramsIt userIt = userNames.begin(); userIt != userNames.end(); ++userIt)
			{
				if (_srvAPI.isUserInChannel(*chIt, *userIt))
					_srvAPI.removeUserFromChannel(*chIt, *userIt);
			}
		}

	

	_srvAPI.send_reply("You've sent a " +  parsed_msg.command + " request!");
	}
}


