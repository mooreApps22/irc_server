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
        parsedMsg(msg),
        channelName(msg.params.at(0)),
        channelId(Parser::toLower(channelName)),
        inviteMode(api.isChannelInviteMode(channelId)),
        keyMode(api.isChannelKeyMode(channelId)),
        limit(api.getChannelLimit(channelId)),
        operatorMode(false),
        topicMode(api.isChannelTopicMode(channelId)),
        passChanged(false),
        userNickname(api.getUserNickname()),
        userID(api.getUserIdentifier()),   
        status(true)
        {
    Logger::log(INFO, "ChannelModeHandler constructed.");
	Logger::log(INFO,  msg.command + " received.");
}

ChannelModeHandler::~ChannelModeHandler() {};

std::string ChannelModeHandler::itoa(int value) {
    std::ostringstream oss;
    oss << value;
    return oss.str();
}

struct ChannelModeHandler::ChannelMode {
    bool                isActive;
    char                flag;
    std::string         param;
        
    ChannelMode(bool active, char f, const std::string& p = "") 
        : isActive(active), flag(f), param(p) {}
};

std::string joinStrings(const std::vector<std::string>& elements, 
                        const std::string& separator = " ", 
                        const std::string& prefix = "") {
                            std::ostringstream result;

    if (!prefix.empty()) {
        result << prefix;
    }
    
    for (std::vector<std::string>::const_iterator it = elements.begin(); 
            it != elements.end(); 
            ++it) {
        if (it != elements.begin() || !prefix.empty())
            result << separator;
        result << *it;
    }
        
    return result.str();
}

void    ChannelModeHandler::handleLimitChange() {
    if (!status)
        limit = 0;
    else if (paramIt != parsedMsg.getParamsEnd()) {
            const char *num = (*paramIt).c_str();
            paramIt++;					
            limit = (size_t)atoi(num);
        }
}

void    ChannelModeHandler::handleOperatorChange() {
    if (paramIt == parsedMsg.getParamsEnd())
        return ;
    
    std::string targetNickname = *paramIt;
    paramIt++;
    
    if (!api.doesNicknameExist(targetNickname) 
    || (status && !api.isChannelUser(channelId, targetNickname))
    || (status == api.isChannelOperator(channelId, targetNickname)))
        return ;
    
    if (!api.isChannelOperator(channelId, targetNickname)) {
        opSigns.push_back("+o");
        targetNicknames.push_back(targetNickname);
    }else {
        opSigns.push_back("-o");
        targetNicknames.push_back(targetNickname);
    }
}

void    ChannelModeHandler::handleKeyChange() {
    if (status == api.isChannelKeyMode(channelId) || passChanged)
        return;

    passChanged = true;
    if (paramIt == parsedMsg.getParamsEnd() && status)
        return;

    if (status) {
        keyMode = true;
        password = *paramIt;
        paramIt++;
    } else {
        password = api.getChannelKey(channelId);
        keyMode = false;
    }
}

std::string ChannelModeHandler::channelModeString() {
    std::vector<ChannelMode>    modes;
    std::vector<std::string>    params;
    std::string                 result = "+";

    modes.push_back(ChannelMode(api.isChannelInviteMode(channelId), 'i'));
    modes.push_back(ChannelMode(limit > 0, 'l', itoa(api.getChannelLimit(channelId))));
    modes.push_back(ChannelMode(api.isChannelKeyMode(channelId), 'k', 
        api.isChannelUser(channelId) ? api.getChannelKey(channelId) : "<key>"));
    modes.push_back(ChannelMode(api.isChannelTopicMode(channelId), 't'));
    
    for (std::vector<ChannelMode>::const_iterator it = modes.begin(); 
                it != modes.end(); 
                ++it) {
        if (it->isActive) {
            result += it->flag;
            if (!it->param.empty())
                params.push_back(it->param);
        }
    }
    
    return joinStrings(params, " ", result);
}

void    ChannelModeHandler::parseInput() {
    param = parsedMsg.params.at(1);
    paramIt = parsedMsg.getParamsBegin() + 2;

    for (std::string::iterator stringIt = param.begin(); 
                stringIt != param.end(); 
                stringIt++) {
        switch (*stringIt) {
            case '+': status = true; break;
            case '-': status = false; break;
            case 'i': inviteMode = status; break;
            case 'l': handleLimitChange(); break;
            case 'o': handleOperatorChange(); break;
            case 'k': handleKeyChange(); break;
            case 't': topicMode = status; break;
            default: api.sendReply(
                ERR_UMODEUNKNOWNFLAG(userNickname)); break;
        }
    }
}

void    ChannelModeHandler::getResultParams() {
    pluses = "+";
    minuses = "-";
    
    if (inviteMode != api.isChannelInviteMode(channelId)) {
        if (api.isChannelInviteMode(channelId))
            pluses += "i";
        else
            minuses += "i";
    }
    if (topicMode != api.isChannelTopicMode(channelId)) {
        if (api.isChannelTopicMode(channelId))
            pluses += "t";
        else
            minuses += "t";
    }
    if (limit != api.getChannelLimit(channelId)) {
        if (limit > 0){
            pluses += "l";
            outputParams.push_back(itoa(limit));
        }
        else
            minuses += "l";
        api.setChannelLimit(channelId, limit);
    }
}

std::string ChannelModeHandler::buildResultString() {
    bool    plus = true;

    std::string message = "";
    if (pluses.size() > 1)
        message += pluses;
    if (minuses.size() > 1) {
        message += minuses;
        plus = false;
    }
    if (opSigns.size() > 0 && opSigns[opSigns.size() - 1].at(0) == '-')
        plus = false;
    if (message.size() > 0 && opSigns.size() > 0 && ((plus && opSigns.at(0).at(0) == '+')
    || (!plus && opSigns.at(0).at(0) == '-')))
        opSigns.at(0).erase(0, 1);
    message = joinStrings(opSigns, "", message);
    if (opSigns.size() > 0)
        outputParams.push_back(joinStrings(targetNicknames, " "));
    if (passChanged)
        if (keyMode != api.isChannelKeyMode(channelId)) {
            if (keyMode == true){
                if (plus)
                    message += "-";
                message += "k";
                outputParams.push_back(password);
            } else {
                if (!plus || message.size() == 0)
                    message += "+";
                message += "k";
                outputParams.push_back(api.getChannelKey(channelId));
            }
        }
    return message;
}

void ChannelModeHandler::handle() { 
    if (!api.doesChannelExist(channelId))
        return (api.sendReply(
            ERR_NOSUCHCHANNEL(userNickname, channelName)));
    
    if (parsedMsg.params.size() == 1)
        return (api.sendReply(
            RPL_CHANNELMODEIS(userNickname, channelName, channelModeString())));

    if (!api.isChannelOperator(channelId))
		return (api.sendReply(
            ERR_CHANOPRIVSNEEDED(userNickname, channelName)));

    parseInput();
    getResultParams();
    std::string resultString = buildResultString();
   
    if (outputParams.size() > 0)
        resultString = joinStrings(outputParams, " ", resultString);
    if (resultString.size() > 0)
    {
        api.sendMessageToChannel(
            CMODE_RPL(userID, channelName, resultString), channelId);
        api.sendReply(
            CMODE_RPL(userID, channelName, resultString));
    }
}
