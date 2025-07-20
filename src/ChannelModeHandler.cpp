#include "ChannelModeHandler.hpp"
#include "IServerAPI.hpp"
#include "macros.hpp"
#include "Logger.hpp"
#include <vector>
#include <string>
#include <cstdlib>


ChannelModeHandler::ChannelModeHandler(IServerAPI& srvAPI, parsed_message& msg) 
    :   api(srvAPI),
        param(msg.params.at(0)),
        channelName(param),
        channelId(Parser::toLower(channelName)),
        i(api.isChannelInviteOnly(channelId)),
        k(api.isChannelPasswordProtected(channelId)),
        l(api.doesChannelHaveLimit(channelId)),
        o(false),
        t(api.isChannelTopicProtected(channelId)),
        passChanged(false),
        limit(0),
        parsed_msg(msg),
        reply_message("")
{
    Logger::log(INFO, "ChannelModeHandler constructed.");
    oldKey = "";
	Logger::log(INFO,  msg.command + " received.");
	user_nickname = api.getUserNick();
	command = msg.command;
	status = true;
	user_identifier = api.getUserIdentifier();
}

ChannelModeHandler::~ChannelModeHandler() {};

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

void ChannelModeHandler::handle(){ 
    if (!api.doesChannelExist(channelId))
    {
        reply_message = CommandHandler::build_reply(SERVER_NAME, ERR_NOSUCHCHANNEL, user_nickname, channelName, "No such channel");
        api.send_reply(reply_message);
        return ;
    }

    channelName = api.getChannelName(channelId);
    
    if (parsed_msg.params.size() == 1)
    {
        mode = "+";
        if (api.doesChannelHaveLimit(channelId))
            mode += "l";
        if (api.isChannelInviteOnly(channelId))
            mode += "i";
        if (api.isChannelTopicProtected(channelId))
            mode += "t";
        if (api.isChannelPasswordProtected(channelId))
            mode += "k";
        if (api.doesChannelHaveLimit(channelId))
        {
            std::stringstream ss;
            ss << api.getChannelLimit(channelId);
            mode += " " + ss.str();
        }
        if (api.isChannelPasswordProtected(channelId))
            mode += " " + api.getChannelPassword(channelId);
        reply_message = CommandHandler::build_reply(SERVER_NAME, RPL_CHANNELMODEIS, user_nickname, channelName, mode);
        api.send_reply(reply_message);
        return ;
    }
    if (!api.isUserChannelOperator(channelId))
    {
        reply_message = CommandHandler::build_reply(SERVER_NAME, ERR_CHANOPRIVSNEEDED, user_nickname, channelName, "You're not channel operator");
        api.send_reply(reply_message);
        return ;
    }
    param = parsed_msg.params.at(1);
    std::vector<std::string>::iterator paramIt = parsed_msg.params.begin() + 2;


    for (std::string::iterator it = param.begin(); it != param.end(); it++)
    {
        if (*it == '+')
            status = true;
        else if (*it == '-')
            status = false;
        else if (*it == 'i')
            api.setChannelInviteOnly(channelId, status);
        else if (*it == 'l')
        {
            if (!status)
            {
                api.setChannelLimit(channelId, 0);
                api.setChannelHasLimit(channelId, false);
            }
            else
            {
                if (paramIt == parsed_msg.params.end())
                    continue;
                limitString = *paramIt;
                const char *num = (*paramIt).c_str();
                paramIt++;					
                limit = atoi(num);
                if (limit == 0)
                {
                    api.setChannelLimit(channelId, 0);
                    api.setChannelHasLimit(channelId, false);
                    continue;
                }
                api.setChannelLimit(channelId, limit);
            }
        }
        else if (*it == 'o')
        {
            if (paramIt == parsed_msg.params.end())
                continue;
            std::string targetNickname = *paramIt;
            paramIt++;
            
            if (api.isTargetChannelMember(channelId, targetNickname) && status && !api.isTargetChannelOperator(channelId, targetNickname))
            {
                Logger::log(INFO,  "upgrading "+ targetNickname);
                api.promoteChannelMember(channelId, targetNickname);
                opSigns.push_back("+o");
                targetNicknames.push_back(targetNickname);
            }
            else if (api.isTargetChannelOperator(channelId, targetNickname) && !status)
            {
                Logger::log(INFO,  "demoting "+ targetNickname);
                api.demoteChannelOperator(channelId, targetNickname);
                opSigns.push_back("-o");
                targetNicknames.push_back(targetNickname);
            }
        }
        else if (*it == 'k')
        {
            if (status == api.isChannelPasswordProtected(channelId)  || passChanged)
                continue;
            passChanged = true;
            if (paramIt == parsed_msg.params.end() && status)
                continue;
            if (status)
            {
                std::string newPassword = *paramIt;
                paramIt++;
                api.setChannelPassword(channelId, newPassword);
            }
            else
            {
                password = api.getChannelPassword(channelId);
                api.clearChannelPassword(channelId);
            }
        }
        else if (*it == 't')
        {
            api.setChannelTopicRestricted(channelId, status);
        }
        else
        {
            reply_message = CommandHandler::build_reply(SERVER_NAME, ERR_UMODEUNKNOWNFLAG, user_nickname, "Unknown MODE flag");
            api.send_reply(reply_message);
        }
        (void) paramIt;
    }
    std::string pluses = "+";
    std::string minuses = "-";
    std::vector<std::string> params;
    bool	plus = true;
    
    if (i != api.isChannelInviteOnly(channelId))
    {
        if (api.isChannelInviteOnly(channelId))
            pluses += "i";
        else
            minuses += "i";
    }
    if (t != api.isChannelTopicProtected(channelId))
    {
        if (api.isChannelTopicProtected(channelId))
            pluses += "t";
        else
            minuses += "t";
    }
    if (l != api.getChannelLimit(channelId))
    {
        if (api.doesChannelHaveLimit(channelId))
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
    // params.push_back(join_strings(opSigns, ""));
    message = join_strings(message, opSigns, "");
    if (opSigns.size() > 0)
    {
        params.push_back(join_strings(targetNicknames, " "));
    }
    if (passChanged)
    {
        if (k != api.isChannelPasswordProtected(channelId))
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
                params.push_back(api.getChannelPassword(channelId));
            }
        }
    }
    if (params.size() > 0)
        message = join_strings(message, params, " ");
    std::cout << message << std::endl;
    reply_message = CommandHandler::build_reply(user_identifier, command, channelName, message);
    if (message.size() > 0)
    {
        api.sendMessageToChannel(channelId, reply_message);
        api.send_reply(reply_message);
    }
}
