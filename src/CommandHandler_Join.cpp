#include "Server.hpp"
#include "User.hpp"
#include "Channel.hpp"
#include "CommandHandler.hpp"
#include "Logger.hpp"
#include "IrcErrorCodes.hpp"
#include <vector>
#include <string>

void	CommandHandler::_joinFp(parsed_message& parsed_msg)
{
	Logger::log(INFO,  parsed_msg.command + " received.");

	std::string	channelName = parsed_msg.params.at(0);
	Logger::log(INFO, "Have ChannelName.");
	std::string password = parsed_msg.params.size() > 1 ? parsed_msg.params.at(1) : "";
	Logger::log(INFO, "Have Password");
	int			fd = _srvAPI.getFdFromNickname(parsed_msg.prefix);
	Logger::log(INFO, "Have fd.");
	User* 		user = _srvAPI.getUser(fd);
	Logger::log(INFO, "Have user.");
	Channel*	channel = _srvAPI.getChannel(parsed_msg.prefix);
	Logger::log(INFO, "Have channel.");

	if (!channel)
	{
		_srvAPI.send_reply(std::string(ERR_NOSUCHCHANNEL) + " :No such channel");
		return ;
	}
	if (channel->needsChannelKey() && channel->getKey() != password)
	{
		_srvAPI.send_reply(std::string(ERR_PASSWDMISMATCH) + " :Invalid channel password");
		return ;
	}
	if (channel->isMember(user->getNickname()))
	{
		_srvAPI.send_reply("You're already a member of " + channelName);
		return ;
	}
	_srvAPI.addUserToChannel(user, channelName);
	_srvAPI.send_reply("You've joined the channel " + channelName);
	Logger::log(INFO, "User was added to channel: ", channelName);
}
