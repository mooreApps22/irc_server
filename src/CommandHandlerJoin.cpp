#include "CommandHandler.hpp"
#include "IServerAPI.hpp"
#include "macros.hpp"
#include "Logger.hpp"
#include <vector>
#include <string>

std::vector<std::string>	mySplit(const std::string& str, char delimiter)
{
	std::vector<std::string>	split;
	std::string::size_type		start = 0;
	std::string::size_type		end;

	while ((end = str.find(delimiter, start)) != std::string::npos)
	{
		split.push_back(str.substr(start, end - start));
		start = end + 1;
	}
	split.push_back(str.substr(start));
	return (split);
}

bool	CommandHandler::validChannelName(const std::string& name)
{
	if (name.empty())
		return (false);
	
	char	firstChar = name[0];

	if (firstChar != '#')
	{
		_srvAPI.send_reply("Channels must start with a #.");
		return (false);
	}
	for (std::string::const_iterator it = name.begin(); it != name.end(); ++it)
	{
		if (*it == ' ' || *it == ',' || static_cast<unsigned char>(*it) < 32)
			return (false);
	}
	return (true);
}


bool	CommandHandler::processJoinParams(User* user, std::string chanParams, std::string keyParams)
{
	if (chanParams.empty())
	{
		_srvAPI.send_reply("No valid JOIN parameters.");
		return (false);
	}
	if (!user)
	{
		_srvAPI.send_reply("No valid JOIN user.");
		return (false);
	}

	std::vector<std::string> channelNames = mySplit(chanParams, ',');
	std::vector<std::string> keyNames;
	if (!keyParams.empty())
		keyNames = mySplit(keyParams, ',');
	for (paramsIt it = channelNames.begin(); it != channelNames.end(); ++it)
	{
		//if channel name is valid
		if (!validChannelName(*it))
			return (false);	
		//if channel (aka *it) doesnt't exist, create it and make the user the operator
		Channel* currentChannel = _srvAPI.getChannel(*it);
		if (!currentChannel)
		{
			
			_srvAPI.send_reply("Creating a current channel");
		}
		//if the channel isFull() then don't allow to join
		//if the channel isInviteOnly() then don't allow to join
		//if key is set the key must match (if a key is used it must be iterated)
		//send notice about available channel command
			// PRIVMSG to everyone in the channel
		//if they are an operator, send channel operator commands
			/*
				KICK, INVITE, TOPIC, MODE(+/-itkol)
			*/
		//send the the current TOPIC of the channel
		_srvAPI.send_reply(*it);
	}
	return (true);
}

//void	CommandHandler::createORJoinChannels(st)

void	CommandHandler::_joinFp(parsed_message& parsed_msg)
{
	
	Logger::log(INFO,  parsed_msg.command + " received.");
	
//	paramsIt it = parsed_msg.getParamsBegin();
	int			fd = getClientFd();
	User*		user = _srvAPI.getUser(fd);
	std::string channelParam = parsed_msg.params[0];
	std::string keyParam = (parsed_msg.params.size() > 1) ? parsed_msg.params[1] : "";
	
	processJoinParams(user, channelParam, keyParam);
//	createORJoinChannels(it);
	//checkChannelKeys(it);


	_srvAPI.send_reply("You've sent a" +  parsed_msg.command + "request!");
}
