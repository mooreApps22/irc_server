#include "CommandHandler.hpp"
#include "IServerAPI.hpp"
#include "Logger.hpp"
#include "macros.hpp"
#include <string>
#include <cstdlib>

typedef struct modes
{
	bool		i;
	bool		k;
	int			l;
	bool		o;
	bool		t;
	bool		passChanged;
	std::string	password;
	int			limit;
	std::string	limitString;
	std::vector<std::string> opSigns;
	std::vector<std::string> targetNicknames;
	std::string	oldKey;
} t_modes;

std::string join_strings(const std::vector<std::string>& elements, const std::string& separator = " ")
{
    std::ostringstream os;
    for (std::vector<std::string>::const_iterator it = elements.begin(); it != elements.end(); ++it)
    {
        if (it != elements.begin())
            os << separator;
        os << *it;
    }
    return os.str();
}
std::string join_strings(std::string start, const std::vector<std::string>& elements, const std::string& separator = " ")
{
    std::ostringstream os;
    os << start;
    for (std::vector<std::string>::const_iterator it = elements.begin(); it != elements.end(); ++it)
    {
        os << separator;
        os << *it;
    }
    return os.str();
}

void	CommandHandler::_modeFp(parsed_message& parsed_msg)
{
	Logger::log(INFO,  parsed_msg.command + " received.");
	std::string user_nickname = _srvAPI.getUserNick();
	std::string	command = parsed_msg.command;
	std::string reply_message;
	bool		status = true;
	std::string mode;
	std::string user_identifier = _srvAPI.getUserIdentifier(); 


	if (!_srvAPI.isUserRegistered())
	{
		reply_message = build_reply(SERVER_NAME, ERR_NOTREGISTERED, user_nickname, command, "You have not registered");
		_srvAPI.send_reply(reply_message);
		return ;
	}

	if (parsed_msg.params.size() == 0)
	{
		reply_message = build_reply(SERVER_NAME, ERR_NEEDMOREPARAMS, user_nickname, command, "Not enough parameters");
		_srvAPI.send_reply(reply_message);
		return ;
	}

	std::string	param = parsed_msg.params.at(0);
	if (Parser::is_nickname(param))
	{
		std::string nickname = param;
		if (nickname != user_nickname)
		{
			reply_message = build_reply(SERVER_NAME, ERR_USERSDONTMATCH, user_nickname, "Cannot change mode for other users");
			_srvAPI.send_reply(reply_message);
			return ;
		}
		if (parsed_msg.params.size() == 1)
		{
			if (_srvAPI.isUserInvisible())
				mode = "+i";
			else
				mode = "-i";
			reply_message = build_reply(user_nickname, "MODE", user_nickname, mode);
			_srvAPI.send_reply(reply_message);
			return ;
		}
		param = parsed_msg.params.at(1);
		for (std::string::iterator it = param.begin(); it != param.end(); it++)
		{
			if (*it == '+')
				status = true;
			else if (*it == '-')
				status = false;
			else if (*it == 'i')
			{
				if (_srvAPI.isUserInvisible() != status)
				{
					_srvAPI.setUserInvisibleMode(status);
					if (status == true)
						mode = "+i";
					else
						mode = "-i";
					reply_message = build_reply(user_nickname, "MODE", user_nickname, mode);
					_srvAPI.send_reply(reply_message);
				}
			}
			else
			{
				reply_message = build_reply(SERVER_NAME, ERR_UMODEUNKNOWNFLAG, user_nickname, "Unknown MODE flag");
				_srvAPI.send_reply(reply_message);
			}	
		}        
	}
	else if (Parser::is_channel(param))
	{
		std::string channelName = param;
		std::string channelId = Parser::toLower(channelName);
		
		if (!_srvAPI.doesChannelExist(channelId))
		{
			reply_message = build_reply(SERVER_NAME, ERR_NOSUCHCHANNEL, user_nickname, channelName, "No such channel");
			_srvAPI.send_reply(reply_message);
			return ;
		}

		channelName = _srvAPI.getChannelName(channelId);
		
		if (parsed_msg.params.size() == 1)
		{
			mode = "+";
			if (_srvAPI.doesChannelHaveLimit(channelId))
				mode += "l";
			if (_srvAPI.isChannelInviteOnly(channelId))
				mode += "i";
			if (_srvAPI.isChannelTopicProtected(channelId))
				mode += "t";
			if (_srvAPI.isChannelPasswordProtected(channelId))
				mode += "k";
			if (_srvAPI.doesChannelHaveLimit(channelId))
			{
				std::stringstream ss;
    			ss << _srvAPI.getChannelLimit(channelId);
				mode += " " + ss.str();
			}
			if (_srvAPI.isChannelPasswordProtected(channelId))
				mode += " " + _srvAPI.getChannelPassword(channelId);
			reply_message = build_reply(SERVER_NAME, RPL_CHANNELMODEIS, user_nickname, channelName, mode);
			_srvAPI.send_reply(reply_message);
			return ;
		}
		if (!_srvAPI.isUserChannelOperator(channelId))
		{
			reply_message = build_reply(SERVER_NAME, ERR_CHANOPRIVSNEEDED, user_nickname, channelName, "You're not channel operator");
			_srvAPI.send_reply(reply_message);
			return ;
		}
		param = parsed_msg.params.at(1);
		t_modes modes;
		std::vector<std::string>::iterator paramIt = parsed_msg.params.begin() + 2;
		modes.i = _srvAPI.isChannelInviteOnly(channelId);
		modes.k = _srvAPI.isChannelPasswordProtected(channelId);
		modes.l = _srvAPI.doesChannelHaveLimit(channelId);
		modes.t = _srvAPI.isChannelTopicProtected(channelId);
		modes.passChanged = false;

		for (std::string::iterator it = param.begin(); it != param.end(); it++)
		{
			if (*it == '+')
				status = true;
			else if (*it == '-')
				status = false;
			else if (*it == 'i')
				_srvAPI.setChannelInviteOnly(channelId, status);
			else if (*it == 'l')
			{
				if (!status)
				{
					_srvAPI.setChannelLimit(channelId, 0);
					_srvAPI.setChannelHasLimit(channelId, false);
				}
				else
				{
					if (paramIt == parsed_msg.params.end())
						continue;
					modes.limitString = *paramIt;
					const char *num = (*paramIt).c_str();
					paramIt++;					
					modes.limit = atoi(num);
					if (modes.limit == 0)
					{
						_srvAPI.setChannelLimit(channelId, 0);
						_srvAPI.setChannelHasLimit(channelId, false);
						continue;
					}
					_srvAPI.setChannelLimit(channelId, modes.limit);
				}
			}
			else if (*it == 'o')
			{
				if (paramIt == parsed_msg.params.end())
					continue;
				std::string targetNickname = *paramIt;
				paramIt++;
				
				if (_srvAPI.isTargetChannelMember(channelId, targetNickname) && status && !_srvAPI.isTargetChannelOperator(channelId, targetNickname))
				{
					Logger::log(INFO,  "upgrading "+ targetNickname);
					_srvAPI.promoteChannelMember(channelId, targetNickname);
					modes.opSigns.push_back("+o");
					modes.targetNicknames.push_back(targetNickname);
				}
				else if (_srvAPI.isTargetChannelOperator(channelId, targetNickname) && !status)
				{
					Logger::log(INFO,  "demoting "+ targetNickname);
					_srvAPI.demoteChannelOperator(channelId, targetNickname);
					modes.opSigns.push_back("-o");
					modes.targetNicknames.push_back(targetNickname);
				}
			}
			else if (*it == 'k')
			{
				if (status == _srvAPI.isChannelPasswordProtected(channelId)  || modes.passChanged)
					continue;
				modes.passChanged = true;
				if (paramIt == parsed_msg.params.end() && status)
					continue;
				if (status)
				{
					std::string newPassword = *paramIt;
					paramIt++;
					_srvAPI.setChannelPassword(channelId, newPassword);
				}
				else
				{
					modes.password = _srvAPI.getChannelPassword(channelId);
					_srvAPI.clearChannelPassword(channelId);
				}
			}
			else if (*it == 't')
			{
				_srvAPI.setChannelTopicRestricted(channelId, status);
			}
			else
			{
				reply_message = build_reply(SERVER_NAME, ERR_UMODEUNKNOWNFLAG, user_nickname, "Unknown MODE flag");
				_srvAPI.send_reply(reply_message);
			}
			(void) paramIt;
		}
		std::string pluses = "+";
		std::string minuses = "-";
		std::vector<std::string> params;
		bool	plus = true;
		
		if (modes.i != _srvAPI.isChannelInviteOnly(channelId))
		{
			if (_srvAPI.isChannelInviteOnly(channelId))
				pluses += "i";
			else
				minuses += "i";
		}
		if (modes.t != _srvAPI.isChannelTopicProtected(channelId))
		{
			if (_srvAPI.isChannelTopicProtected(channelId))
				pluses += "t";
			else
				minuses += "t";
		}
		if (modes.l != _srvAPI.getChannelLimit(channelId))
		{
			if (_srvAPI.doesChannelHaveLimit(channelId))
			{
				pluses += "l";
				params.push_back(modes.limitString);
			}
			else
				minuses += "l";
		}

		std::string message = "";
		if (pluses.size() > 1)
			message += pluses;
		if (minuses.size() > 1)
		{
			message += minuses;
			plus = false;
		}
		if (modes.opSigns.size() > 0 && modes.opSigns[modes.opSigns.size() - 1].at(0) == '-')
			plus = false;
		if (message.size() > 0 && modes.opSigns.size() > 0 && ((plus && modes.opSigns.at(0).at(0) == '+')
		|| (!plus && modes.opSigns.at(0).at(0) == '-')))
			modes.opSigns.at(0).erase(0, 1);
		// params.push_back(join_strings(modes.opSigns, ""));
		message = join_strings(message, modes.opSigns, "");
		if (modes.opSigns.size() > 0)
		{
			params.push_back(join_strings(modes.targetNicknames, " "));
		}
		if (modes.passChanged)
		{
			if (modes.k != _srvAPI.isChannelPasswordProtected(channelId))
			{
				if (modes.k == true)
				{
					if (plus)
						message += "-";
					message += "k";
					params.push_back(modes.password);
				}
				else
				{
					if (!plus || message.size() == 0)
						message += "+";
					message += "k";
					params.push_back(_srvAPI.getChannelPassword(channelId));
				}
			}
		}
		if (params.size() > 0)
			message = join_strings(message, params, " ");
		std::cout << message << std::endl;
		reply_message = build_reply(user_identifier, command, channelName, message);
		if (message.size() > 0)
		{
			_srvAPI.sendMessageToChannel(channelId, reply_message);
			_srvAPI.send_reply(reply_message);
		}
	}
	else
	{
		reply_message = build_reply(SERVER_NAME, ERR_NOSUCHNICK, user_nickname, param, ":No such nick/channel");
		_srvAPI.send_reply(reply_message);
	}

	
}