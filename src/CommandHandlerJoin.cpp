#include "CommandHandler.hpp"
#include "IServerAPI.hpp"
#include "macros.hpp"
#include "Logger.hpp"
#include <vector>
#include <string>
/*
	#define ERR_INVITEONLYCHAN 		"473"	// "<channel> :Cannot join channel (+i)"
	#define ERR_CHANNELISFULL 		"471"	// "<channel> :Cannot join channel (+l)"
	#define ERR_NOSUCHCHANNEL 		"403"	// "<channel name> :No such channel"
	#define ERR_BADCHANNELKEY 		"475"	// "<channel> :Cannot join channel (+k)"
	#define ERR_TOOMANYCHANNELS 	"405"	// "<channel name> :You have joined too many channels"
*/

// TODO: Delete (duplicated from Parser method())
bool	CommandHandler::isChString(std::string::iterator it)
{
	return *it != ' ' && *it != 7 && *it != '\0' && *it != '\r' && *it != '\n' && *it != ',';
}

// TODO: Delete (duplicated from Parser method())
bool	CommandHandler::isValidChannelName(std::string& channel)
{
	std::string::iterator it = channel.begin();

	if (*it != '#' && *it != '&')
		return false;
	it++;
	for(; it != channel.end(); it++)
	{
		if (!isChString(it))
			return false;
	}
	return true;
}

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

bool	CommandHandler::processJoinParams(std::string chanParams, std::string keyParams)
{
	if (chanParams.empty())
	{
		_srvAPI.send_reply("No valid JOIN parameters.");
		return (false);
	}

	std::vector<std::string> channelNames = mySplit(chanParams, ',');
	std::vector<std::string> keyNames = mySplit(keyParams, ',');

	if (keyParams.empty())
		keyNames.clear();
	for (paramsIt chanIt = channelNames.begin(); chanIt != channelNames.end(); ++chanIt) //, keyIt = keyNames.begin()
	{
		//if channel name is valid
		if (!isValidChannelName(*chanIt)) // TODO: Change to -if (!Parser::is_channel(*chanIt))	-
		{
			_srvAPI.send_reply("Is no valid channel.");
			return (false);	
		}
		//if channel (aka *it) doesnt't exist, create it and make the user the operator
		if (!_srvAPI.doesChannelExist(*chanIt))
		{
			_srvAPI.addChannel(*chanIt);
			_srvAPI.send_reply("Creating a channel:" + *chanIt);
			_srvAPI.addUserToChannel(*chanIt);
			_srvAPI.send_reply("You were added to " + *chanIt);
			return (true);
		}
		//if key is set the key must match (if a key is used it must be iterated)

		//if the channel isInviteOnly() then don't allow to join

		//if the channel isFull() then don't allow to join
		/*if (_srvAPI.doesChannelHaveLimit(*chanIt) && _srvAPI.isChannelFull(*chanIt))
		{
			_srvAPI.send_reply("Channel is full, sorry!");
			return (false);	
		}*/
		/*_srvAPI.addUserToChannel(*chanIt);*/
		/*_srvAPI.send_reply("You were added to " + *chanIt);*/
		//send notice about available channel command
			// PRIVMSG to everyone in the channel
		//send notice of list of users who are on the channel 
		//if they are an operator, send channel operator commands
			/*
				KICK, INVITE, TOPIC, MODE(+/-itkol)
			*/
		//send the the current TOPIC of the channel
		_srvAPI.send_reply(*chanIt);
	}
	return (true);
}

//void	CommandHandler::createORJoinChannels(st)

void	CommandHandler::_joinFp(parsed_message& parsed_msg)
{
	Logger::log(INFO,  parsed_msg.command + " received.");
	
	std::string	replyMessage;
	std::string	userNickname = _srvAPI.getUserNick();
	std::string	command = parsed_msg.command;	

	if (!_srvAPI.isUserRegistered())
	{
		replyMessage = build_reply(std::string(SERVER_NAME), std::string(ERR_NOTREGISTERED), userNickname, command, "You have not registered");
		_srvAPI.send_reply(replyMessage);
		return ;
	}
	if (parsed_msg.params.size() == 0)
	{
		replyMessage = build_reply(SERVER_NAME, ERR_NEEDMOREPARAMS, userNickname, command, "Not enough parameters");
		_srvAPI.send_reply(replyMessage);
		return ;
	}

	std::string channelParam = parsed_msg.params[0];
	std::string keyParam = (parsed_msg.params.size() > 1) ? parsed_msg.params[1] : "";
	
	processJoinParams(channelParam, keyParam);
//	createORJoinChannels(it);
	//checkChannelKeys(it);


	_srvAPI.send_reply("You've sent a" +  parsed_msg.command + "request!");
}
