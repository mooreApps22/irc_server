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
		std::string channel = param;
		if (!_srvAPI.doesChannelExist(channel))
		{
			reply_message = build_reply(SERVER_NAME, ERR_NOSUCHCHANNEL, user_nickname, channel, "No such channel");
			_srvAPI.send_reply(reply_message);
			return ;
		}
		if (parsed_msg.params.size() == 1)
		{
			mode = "+";
			if (_srvAPI.doesChannelHaveLimit(channel))
				mode += "l";
			if (_srvAPI.isChannelInviteOnly(channel))
				mode += "i";
			if (_srvAPI.isChannelTopicProtected(channel))
				mode += "t";
			if (_srvAPI.isChannelPasswordProtected(channel))
				mode += "k";
			if (_srvAPI.doesChannelHaveLimit(channel))
			{
				std::stringstream ss;
    			ss << _srvAPI.getChannelLimit(channel);
				mode += " " + ss.str();
			}
			if (_srvAPI.isChannelPasswordProtected(channel))
				mode += " " + _srvAPI.getChannelPassword(channel);
			reply_message = build_reply(SERVER_NAME, RPL_CHANNELMODEIS, user_nickname, channel, mode);
			_srvAPI.send_reply(reply_message);
			return ;
		}
		if (!_srvAPI.isUserChannelOperator(channel))
		{
			reply_message = build_reply(SERVER_NAME, ERR_CHANOPRIVSNEEDED, user_nickname, channel, "You're not channel operator");
			_srvAPI.send_reply(reply_message);
			return ;
		}
		param = parsed_msg.params.at(1);
		t_modes modes;
		std::vector<std::string>::iterator paramIt = parsed_msg.params.begin() + 2;
		modes.i = _srvAPI.isChannelInviteOnly(channel);
		modes.k = _srvAPI.isChannelPasswordProtected(channel);
		modes.l = _srvAPI.getChannelLimit(channel);
		modes.t = _srvAPI.isChannelTopicProtected(channel);
		modes.passChanged = false;

		for (std::string::iterator it = param.begin(); it != param.end(); it++)
		{
			if (*it == '+')
				status = true;
			else if (*it == '-')
				status = false;
			else if (*it == 'i')
				_srvAPI.setChannelInviteOnly(channel, status);
			else if (*it == 'l')
			{
				if (!status)
				{
					_srvAPI.setChannelLimit(channel, 0);
					_srvAPI.setChannelHasLimit(channel, false);
				}
				else
				{
					if (paramIt == parsed_msg.params.end())
						continue;
					modes.limitString = *paramIt;
					const char *num = (*paramIt).c_str();
					modes.limit = atoi(num);
					if (modes.limit == 0)
					{
						_srvAPI.setChannelLimit(channel, 0);
						_srvAPI.setChannelHasLimit(channel, false);
						continue;
					}
					paramIt++;					
					_srvAPI.setChannelLimit(channel, modes.limit);
				}
			}
			else if (*it == 'o')
			{
				if (paramIt == parsed_msg.params.end())
					continue;
				std::string targetNickname = *paramIt;
				paramIt++;
				if (_srvAPI.isTargetChannelMember(channel, targetNickname) && status && !_srvAPI.isTargetChannelOperator(channel, targetNickname))
				{
					Logger::log(INFO,  "upgrading "+ targetNickname);
					_srvAPI.promoteChannelMember(channel, targetNickname);
					modes.opSigns.push_back("+o");
					modes.targetNicknames.push_back(targetNickname);
				}
				else if (_srvAPI.isTargetChannelOperator(channel, targetNickname) && !status)
				{
					Logger::log(INFO,  "demoting "+ targetNickname);
					_srvAPI.demoteChannelOperator(channel, targetNickname);
					modes.opSigns.push_back("-o");
					modes.targetNicknames.push_back(targetNickname);
				}
			}
			else if (*it == 'k')
			{
				if (status == _srvAPI.isChannelPasswordProtected(channel))
					continue;
				if ((paramIt == parsed_msg.params.end() && status) || modes.passChanged)
				{
					modes.passChanged = true;
					continue;
				}
				modes.passChanged = true;
				if (status)
				{
					std::string newPassword = *paramIt;
					paramIt++;
					_srvAPI.setChannelPassword(channel, newPassword);
				}
				else
				{
					modes.password = _srvAPI.getChannelPassword(channel);
					_srvAPI.clearChannelPassword(channel);
				}
			}
			else if (*it == 't')
			{
				_srvAPI.setChannelTopicRestricted(channel, status);
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
		
		if (modes.i != _srvAPI.isChannelInviteOnly(channel))
		{
			if (_srvAPI.isChannelInviteOnly(channel))
				pluses += "i";
			else
				minuses += "i";
		}
		if (modes.t != _srvAPI.isChannelTopicProtected(channel))
		{
			if (_srvAPI.isChannelTopicProtected(channel))
				pluses += "t";
			else
				minuses += "t";
		}
		if (modes.l != _srvAPI.getChannelLimit(channel))
		{
			if (_srvAPI.doesChannelHaveLimit(channel))
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
			if (modes.k != _srvAPI.isChannelPasswordProtected(channel))
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
					params.push_back(_srvAPI.getChannelPassword(channel));
				}
			}
		}
		if (params.size() > 0)
			message = join_strings(message, params, " ");
		// std::cout << message << std::endl;
		reply_message = build_reply(user_identifier, command, channel, message);
		// std::string targets = join_strings(modes.targetNicknames, " ");
		if (message.size() > 0)
		{
			_srvAPI.sendMessageToChannel(channel, reply_message);
			_srvAPI.send_reply(reply_message);
		}
	}
	else
	{
		std::cout << "MODE none" << std::endl;
		// :Rubicon.GeekShed.net 401 norbac no :No such nick/channel
	}

	
}