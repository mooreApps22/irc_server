#include "CommandHandler.hpp"
#include "Parser.hpp"
#include "macros.hpp"
#include "Logger.hpp"
#include <vector>
#include <string>
#include <cstdlib>
#include <sstream>

typedef struct s_modes
{
	bool						i;
	bool						k;
	size_t						l;
	bool						o;
	bool						t;
	bool						passChanged;
	std::string					password;
	size_t						limit;
	std::string					limitString;
	std::vector<std::string>	opSigns;
	std::vector<std::string>	targetNicknames;
	std::string					oldKey;
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
	return (os.str());
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
	return (os.str());
}

void	CommandHandler::_modeFp(const parsedMessage& parsedMsg) const
{
	// Logger::log(INFO,  parsedMsg.command + " received.");
	std::string userNickname = _srvAPI.getUserNickname();
	std::string	command = parsedMsg.command;
	std::string userID = _srvAPI.getUserIdentifier(); 
	std::string mode;
	bool		status = true;


	if (!_srvAPI.isUserRegistered())
	{
		_srvAPI.sendReply(ERR_NOTREGISTERED(userNickname, command));
		return ;
	}

	if (parsedMsg.params.size() == 0)
	{
		_srvAPI.sendReply(ERR_NEEDMOREPARAMS(userNickname, command));
		return ;
	}

	std::string	param = parsedMsg.params.at(0);
	if (Parser::isNickname(param))
	{
		std::string nickname = param;
		if (!_srvAPI.doesNicknameExist(nickname))
		{
			_srvAPI.sendReply(ERR_NOSUCHNICK(userNickname, nickname));
			return ;
		}
		if (parsedMsg.params.size() == 1)
		{
			if (Parser::toLower(nickname) != Parser::toLower(userNickname))
				return ;
			mode ="+";
			if (_srvAPI.isUserInvisibleMode())
				mode += "i";
			_srvAPI.sendReply(RPL_UMODEIS(userNickname, mode));
			return ;
		}
		
		param = parsedMsg.params.at(1);
		if (Parser::toLower(nickname) != Parser::toLower(userNickname))
		{
			_srvAPI.sendReply(ERR_USERSDONTMATCH(userNickname));
			return ;
		}
		bool iMode = _srvAPI.isUserInvisibleMode();
		for (std::string::iterator it = param.begin(); it != param.end(); it++)	// TODO struct iterator
		{
			if (*it == '+')
				status = true;
			else if (*it == '-')
				status = false;
			else if (*it == 'i')
				iMode = status;
			else
				_srvAPI.sendReply(ERR_UMODEUNKNOWNFLAG(userNickname));
		}

		if (iMode != _srvAPI.isUserInvisibleMode())
		{
			_srvAPI.setUserInvisibleMode(iMode);
			if (iMode == true)
				mode = "+i";
			else
				mode = "-i";
			_srvAPI.sendReply(UMODE_RPL(userNickname, mode));
		}		
	}
	else if (Parser::isChannel(param))
	{
		std::string channelName = param;
		
		if (!_srvAPI.doesChannelExist(channelName))
		{
			_srvAPI.sendReply(ERR_NOSUCHCHANNEL(userNickname, channelName));
			return ;
		}
		std::string channelId = Parser::toLower(channelName);
		channelName = _srvAPI.getChannelName(channelId);
		
		if (parsedMsg.params.size() == 1)
		{
			mode = "+";
			if (_srvAPI.isChannelLimitMode(channelId))
				mode += "l";
			if (_srvAPI.isChannelInviteMode(channelId))
				mode += "i";
			if (_srvAPI.isChannelTopicMode(channelId))
				mode += "t";
			if (_srvAPI.isChannelKeyMode(channelId))
				mode += "k";
			if (_srvAPI.isChannelLimitMode(channelId))
			{
				std::stringstream ss;
    			ss << _srvAPI.getChannelLimit(channelId);
				mode += " " + ss.str();
			}
			if (_srvAPI.isChannelKeyMode(channelId))
				mode += " " + _srvAPI.getChannelKey(channelId);
			_srvAPI.sendReply(RPL_CHANNELMODEIS(userNickname, channelName, mode));
			return ;
		}
		if (!_srvAPI.isChannelOperator(channelId))
		{
			_srvAPI.sendReply(ERR_CHANOPRIVSNEEDED(userNickname, channelName));
			return ;
		}
		param = parsedMsg.params.at(1);
		t_modes modes;
		parsedMessage::paramsIt modesParamsIt = parsedMsg.getParamsBegin() + 2;
		modes.i = _srvAPI.isChannelInviteMode(channelId);
		modes.k = _srvAPI.isChannelKeyMode(channelId);
		modes.l = _srvAPI.getChannelLimit(channelId);
		modes.t = _srvAPI.isChannelTopicMode(channelId);
		modes.passChanged = false;

		for (std::string::iterator it = param.begin(); it != param.end(); it++)
		{
			if (*it == '+')
				status = true;
			else if (*it == '-')
				status = false;
			else if (*it == 'i')
				_srvAPI.setChannelInviteMode(channelId, status);
			else if (*it == 'l')
			{
				if (!status)
				{
					_srvAPI.setChannelLimit(channelId, 0);
// 					_srvAPI.setChannelHasLimit(channelId, false);
				}
				else
				{
					if (modesParamsIt == parsedMsg.getParamsEnd())
						continue;
					modes.limitString = *modesParamsIt;
					const char *num = (*modesParamsIt).c_str();
					modesParamsIt++;					
					modes.limit = atoi(num);
					if (modes.limit == 0)
					{
						_srvAPI.setChannelLimit(channelId, 0);
						// _srvAPI.setChannelHasLimit(channelId, false);
						continue;
					}
					_srvAPI.setChannelLimit(channelId, modes.limit);
				}
			}
			else if (*it == 'o')
			{
				if (modesParamsIt == parsedMsg.params.end())
					continue;
				std::string targetNickname = *modesParamsIt;
				modesParamsIt++;

				if (_srvAPI.isChannelMember(channelId, targetNickname) && status && !_srvAPI.isChannelOperator(channelId, targetNickname))
				{
					// Logger::log(INFO,  "upgrading "+ targetNickname);
					_srvAPI.promoteChannelMember(channelId, targetNickname);
					modes.opSigns.push_back("+o");
					modes.targetNicknames.push_back(targetNickname);
				}
				else if (_srvAPI.isChannelOperator(channelId, targetNickname) && !status)
				{
					// Logger::log(INFO,  "demoting "+ targetNickname);
					_srvAPI.demoteChannelOperator(channelId, targetNickname);
					modes.opSigns.push_back("-o");
					modes.targetNicknames.push_back(targetNickname);
				}
			}
			else if (*it == 'k')
			{
				if (status == _srvAPI.isChannelKeyMode(channelId)  || modes.passChanged)
					continue;
				modes.passChanged = true;
				if (modesParamsIt == parsedMsg.params.end() && status)
					continue;
				if (status)
				{
					std::string newPassword = *modesParamsIt;
					modesParamsIt++;
					_srvAPI.setChannelKey(channelId, newPassword);
				}
				else
				{
					modes.password = _srvAPI.getChannelKey(channelId);
					_srvAPI.clearChannelKey(channelId);
				}
			}
			else if (*it == 't')
				_srvAPI.setChannelTopicMode(channelId, status);
			else
			{
				_srvAPI.sendReply(ERR_UMODEUNKNOWNFLAG(userNickname));	// TODO only show pass value and limit value to channel members
			}
		}
		std::string pluses = "+";
		std::string minuses = "-";
		std::vector<std::string> params;
		bool	plus = true;
		
		if (modes.i != _srvAPI.isChannelInviteMode(channelId))
		{
			if (_srvAPI.isChannelInviteMode(channelId))
				pluses += "i";
			else
				minuses += "i";
		}
		if (modes.t != _srvAPI.isChannelTopicMode(channelId))
		{
			if (_srvAPI.isChannelTopicMode(channelId))
				pluses += "t";
			else
				minuses += "t";
		}
		if (modes.l != _srvAPI.getChannelLimit(channelId))
		{
			if (_srvAPI.isChannelLimitMode(channelId))
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
		message = join_strings(message, modes.opSigns, "");
		if (modes.opSigns.size() > 0)
		{
			params.push_back(join_strings(modes.targetNicknames, " "));
		}
		if (modes.passChanged)
		{
			if (modes.k != _srvAPI.isChannelKeyMode(channelId))
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
					params.push_back(_srvAPI.getChannelKey(channelId));
				}
			}
		}
		if (params.size() > 0)
			message = join_strings(message, params, " ");

		if (message.size() > 0)
		{
			_srvAPI.sendMessageToChannel(CMODE_RPL(userID, channelName, message), channelId);
			_srvAPI.sendReply(CMODE_RPL(userID, channelName, message));
		}
	}
	else
	{
		_srvAPI.sendReply(ERR_NOSUCHNICK(userNickname, param));
	}	
}