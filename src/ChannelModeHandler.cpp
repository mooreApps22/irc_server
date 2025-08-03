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
        password(api.getChannelKey(channelId)),
        userNickname(api.getUserNickname()),
        userID(api.getUserIdentifier()),   
        status(true)
        {
    Logger::log(INFO, "ChannelModeHandler constructed.");
	Logger::log(INFO,  msg.command + " received.");
}

ChannelModeHandler::~ChannelModeHandler() {};
 
ChannelModeHandler::ChannelMode::ChannelMode(bool active, char f, const std::string& p = "") 
        : isActive(active), flag(f), param(p) {};

ChannelModeHandler::ResultStrings::ResultStrings() : minuses("-"), pluses("+") {}

std::string&    ChannelModeHandler::ResultStrings::operator[](int index) {
    return (index == 0) ? minuses : pluses;
}

const std::string&  ChannelModeHandler::ResultStrings::operator[](int index) const {
    return (index == 0) ? minuses : pluses;
}

std::string ChannelModeHandler::itoa(int value) {
    std::ostringstream oss;
    oss << value;
    return oss.str();
}

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
    // if (limit != (size_t)(api.getChannelLimit(channelId)))
    //     return;
    if (!status)
        limit = 0;
    else if (paramIt != parsedMsg.getParamsEnd()) {
            limit = (size_t)atoi((*paramIt).c_str());
            paramIt++;					
        }
}

void    ChannelModeHandler::handleOperatorChange() {
    if (paramIt == parsedMsg.getParamsEnd())
        return ;
    
    std::string targetNickname = *paramIt;
    paramIt++;
    
    if (!api.doesNicknameExist(targetNickname) 
    || (status && !api.isChannelUser(channelId, targetNickname)))
        return ;
    
    if (resultStrings.opChanges.find(targetNickname) == resultStrings.opChanges.end())
        resultStrings.opChanges[targetNickname] = api.isChannelOperator(channelId, targetNickname);
    
    if (resultStrings.opChanges[targetNickname] == status)
        return;

    if (status)
        opSigns.push_back("+o");
    else
        opSigns.push_back("-o");

    resultStrings.opChanges[targetNickname] = status;

    targetNicknames.push_back(targetNickname);
}

void    ChannelModeHandler::handleKeyChange() {
    std::string passParam = "";

    if (passChanged)
        return ;

    if (paramIt == parsedMsg.getParamsEnd() && status)
        return;

    if (paramIt != parsedMsg.getParamsEnd()) {
        passParam = *paramIt;
        paramIt++;
    }

    if (status && api.isChannelKeyMode(channelId) &&
        passParam != "" &&
        passParam == api.getChannelKey(channelId))
        return;

    passChanged = true;

    keyMode = status;
    password = status ? passParam : api.getChannelKey(channelId);
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
                it++) {
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

std::string ChannelModeHandler::buildResultString() {
    bool    plus = true;
    std::string message = "";

    if (inviteMode != api.isChannelInviteMode(channelId))
        resultStrings[inviteMode] += "i";
    if (topicMode != api.isChannelTopicMode(channelId))
        resultStrings[topicMode] += "t";
    if (limit != api.getChannelLimit(channelId)) {
        resultStrings[limit > 0] += "l";
        if (limit > 0)
            resultStrings.params.push_back(itoa(limit));
        api.setChannelLimit(channelId, limit);
    }
    if (resultStrings.pluses.size() > 1)
        message += resultStrings.pluses;
    if (resultStrings.minuses.size() > 1) {
        message += resultStrings.minuses;
        plus = false;
    }
    if (message.size() > 0 && opSigns.size() > 0 && ((plus && opSigns.at(0).at(0) == '+')
        || (!plus && opSigns.at(0).at(0) == '-')))
        opSigns.at(0).erase(0, 1);
    message = joinStrings(opSigns, "", message);
    if (opSigns.size() > 0)
        resultStrings.params.push_back(joinStrings(targetNicknames, " "));
    if (keyMode != api.isChannelKeyMode(channelId)
        || (password != api.getChannelKey(channelId))) {
        if (!keyMode){
            if (plus)
                message += "-";
            message += "k";
        } else {
            if (!plus || message.size() == 0)
                message += "+";
            message += "k";
        }
        resultStrings.params.push_back(password);
    }
    return message;
}

void    ChannelModeHandler::updateStatuses() {
    if (inviteMode != api.isChannelInviteMode(channelId))
        api.setChannelInviteMode(channelId, inviteMode);
    if (topicMode != api.isChannelTopicMode(channelId))
        api.setChannelTopicMode(channelId, topicMode);
    if (limit != api.getChannelLimit(channelId))
        api.setChannelLimit(channelId, limit);
    if (keyMode != api.isChannelKeyMode(channelId)
        || (password != api.getChannelKey(channelId))) {
        if (keyMode)
            api.setChannelKey(channelId, password);
        else
            api.clearChannelKey(channelId);
    }
    for (std::map<std::string, bool>::const_iterator it = resultStrings.opChanges.begin();
            it != resultStrings.opChanges.end(); it++) {
        if (it->second)
            api.promoteChannelMember(channelId, it->first);
        else
            api.demoteChannelOperator(channelId, it->first);
    }
}

void    ChannelModeHandler::handle() { 
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
    std::string resultString = buildResultString();
    updateStatuses();
   
    if (resultStrings.params.size() > 0)
        resultString = joinStrings(resultStrings.params, " ", resultString);
    if (resultString.size() > 0)
    {
        api.sendMessageToChannel(
            CMODE_RPL(userID, channelName, resultString), channelId);
        api.sendReply(
            CMODE_RPL(userID, channelName, resultString));
    }
}
