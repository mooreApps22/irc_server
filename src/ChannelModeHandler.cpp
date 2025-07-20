#include "ChannelModeHandler.hpp"
#include "CommandHandler.hpp"
#include "IServerAPI.hpp"
#include "macros.hpp"
#include "Logger.hpp"
#include "Parser.hpp"
#include <vector>
#include <string>
#include <cstdlib>
#include <sstream>


ChannelModeHandler::ChannelModeHandler(IServerAPI& srvAPI, const parsedMessage& msg) 
    :   api(srvAPI),
        param(msg.params.at(0)),
        channelName(param),
        channelId(Parser::toLower(channelName)),
        i(api.isChannelInviteMode(channelId)),
        k(api.isChannelKeyMode(channelId)),
        l(api.isChannelLimitMode(channelId)),
        o(false),
        t(api.isChannelTopicMode(channelId)),
        passChanged(false),
        limit(0),
        parsedMsg(msg),
        reply_message("")
{
    Logger::log(INFO, "ChannelModeHandler constructed.");
    oldKey = "";
	Logger::log(INFO,  msg.command + " received.");
	userNickname = api.getUserNickname();
	command = msg.command;
	// status = true;
	userID = api.getUserIdentifier();
}

ChannelModeHandler::~ChannelModeHandler() {};

std::string joinStrings(const std::vector<std::string>& elements, const std::string& separator = " ", const std::string& prefix = "")
{
    if (elements.empty())
        return prefix;
    
    std::ostringstream os;
    if (!prefix.empty())
        os << prefix;
    
    for (std::vector<std::string>::const_iterator it = elements.begin(); it != elements.end(); ++it)
    {
        if (it != elements.begin() || !prefix.empty())
            os << separator;
        os << *it;
    }
    return os.str();
}

struct ChannelModeHandler::ChannelMode {
        bool isActive;
        char flag;
        std::string param;
        
        ChannelMode(bool active, char f, const std::string& p = "") 
            : isActive(active), flag(f), param(p) {}
    };

std::string ChannelModeHandler::channelModeString() {
    std::vector<ChannelMode> channelModes;
    std::string modes = "+";
    std::vector<std::string> params;

    channelModes.push_back(ChannelMode(api.isChannelInviteMode(channelId), 'i'));
    channelModes.push_back(ChannelMode(api.isChannelKeyMode(channelId), 'k', 
        api.isChannelUser(channelId) ? api.getChannelKey(channelId) : "<key>"));
    channelModes.push_back(ChannelMode(api.isChannelLimitMode(channelId), 'l'));
    channelModes.push_back(ChannelMode(api.isChannelTopicMode(channelId), 't'));
    
    for (std::vector<ChannelMode>::const_iterator it = channelModes.begin(); 
            it != channelModes.end(); ++it) {
        if (it->isActive) {
            modes += it->flag;
            if (!it->param.empty())
                params.push_back(it->param);
        }
    }
    
    return modes + (params.empty() ? "" : " " + joinStrings(params, " "));
}

void ChannelModeHandler::handle(){ 
    if (!api.doesChannelExist(channelId))
        return (api.sendReply(ERR_NOSUCHCHANNEL(userNickname, channelName)));
    
    if (parsedMsg.params.size() == 1)
        return (api.sendReply(RPL_CHANNELMODEIS(userNickname, channelName, channelModeString())));

    if (!api.isChannelOperator(channelId))
		return (api.sendReply(ERR_CHANOPRIVSNEEDED(userNickname, channelName)));

    param = parsedMsg.params.at(1);
    parsedMessage::paramsIt paramIt = parsedMsg.getParamsBegin() + 2;

    for (std::string::iterator it = param.begin(); it != param.end(); it++)
    {
        if (*it == '+')
            status = true;
        else if (*it == '-')
            status = false;
        else if (*it == 'i')
            api.setChannelInviteMode(channelId, status);
        else if (*it == 'l')
        {
            if (!status)
                api.setChannelLimit(channelId, 0);
            else
            {
                if (paramIt == parsedMsg.params.end())
                    continue;
                limitString = *paramIt;
                const char *num = (*paramIt).c_str();
                paramIt++;					
                limit = atoi(num);
                if (limit == 0)
                {
                    api.setChannelLimit(channelId, 0);
                    continue;
                }
                api.setChannelLimit(channelId, limit);
            }
        }
        else if (*it == 'o')
        {
            if (paramIt == parsedMsg.params.end())
                continue;
            std::string targetNickname = *paramIt;
            paramIt++;
            
				if (api.isChannelMember(channelId, targetNickname) && status && !api.isChannelOperator(channelId, targetNickname))
				{
					// Logger::log(INFO,  "upgrading "+ targetNickname);
					api.promoteChannelMember(channelId, targetNickname);
					opSigns.push_back("+o");
					targetNicknames.push_back(targetNickname);
				}
				else if (api.isChannelOperator(channelId, targetNickname) && !status)
				{
					// Logger::log(INFO,  "demoting "+ targetNickname);
					api.demoteChannelOperator(channelId, targetNickname);
					opSigns.push_back("-o");
					targetNicknames.push_back(targetNickname);
				}
				else
				{
					if (!api.doesNicknameExist(targetNickname))
						api.sendReply(ERR_NOSUCHNICK(userNickname, targetNickname));
					else
						api.sendReply(ERR_USERNOTINCHANNEL(targetNickname, channelName));
				}
        }
        else if (*it == 'k')
			{
				if (status == api.isChannelKeyMode(channelId)  || passChanged)
					continue;
				passChanged = true;
				if (paramIt == parsedMsg.params.end() && status)
					continue;
				if (status)
				{
					std::string newPassword = *paramIt;
					paramIt++;
					api.setChannelKey(channelId, newPassword);
				}
				else
				{
					password = api.getChannelKey(channelId);
					api.clearChannelKey(channelId);
				}
			}
			else if (*it == 't')
				api.setChannelTopicMode(channelId, status);
			else
				api.sendReply(ERR_UMODEUNKNOWNFLAG(userNickname));
    }
    std::string pluses = "+";
    std::string minuses = "-";
    std::vector<std::string> params;
    bool	plus = true;
    
    if (i != api.isChannelInviteMode(channelId))
    {
        if (api.isChannelInviteMode(channelId))
            pluses += "i";
        else
            minuses += "i";
    }
    if (t != api.isChannelTopicMode(channelId))
    {
        if (api.isChannelTopicMode(channelId))
            pluses += "t";
        else
            minuses += "t";
    }
    if (l != api.getChannelLimit(channelId))
    {
        if (api.isChannelLimitMode(channelId))
        {
            pluses += "l";
            params.push_back(limitString);
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
    if (opSigns.size() > 0 && opSigns[opSigns.size() - 1].at(0) == '-')
        plus = false;
    if (message.size() > 0 && opSigns.size() > 0 && ((plus && opSigns.at(0).at(0) == '+')
    || (!plus && opSigns.at(0).at(0) == '-')))
        opSigns.at(0).erase(0, 1);
    // params.push_back(joinStrings(opSigns, ""));
    message = joinStrings(opSigns, "", message);
    if (opSigns.size() > 0)
    {
        params.push_back(joinStrings(targetNicknames, " "));
    }
    if (passChanged)
    {
        if (k != api.isChannelKeyMode(channelId))
        {
            if (k == true)
            {
                if (plus)
                    message += "-";
                message += "k";
                params.push_back(password);
            }
            else
            {
                if (!plus || message.size() == 0)
                    message += "+";
                message += "k";
                params.push_back(api.getChannelKey(channelId));
            }
        }
    }
    if (params.size() > 0)
        message = joinStrings(params, " ", message);
		if (message.size() > 0)
		{
			api.sendMessageToChannel(CMODE_RPL(userID, channelName, message), channelId);
			api.sendReply(CMODE_RPL(userID, channelName, message));
		}
}
