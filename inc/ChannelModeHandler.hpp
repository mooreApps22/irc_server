#pragma once
# include "IServerAPI.hpp"
# include "CommandHandler.hpp"
# include "Parser.hpp"
# include <iostream>
# include <map>
# include <string>

class ChannelModeHandler
{
    public:
        ChannelModeHandler(IServerAPI& srvAPI, const parsedMessage& parsed_msg);
        ~ChannelModeHandler();
        void    handle();

    private:
        std::string                 channelModeString();
        struct                      ChannelMode;
        void                        handleLimitChange();
        void                        handleOperatorChange();
        void                        handleKeyChange();
        std::string                 itoa(int);
        void                        parseInput();
        void                        getResultParams();
        std::string                 buildResultString();
        
        IServerAPI&                 api;    
        std::string                 param;
        const parsedMessage&        parsedMsg;
        std::string                 channelName;
        std::string                 channelId;
        bool		                inviteMode;
        bool		                keyMode;
        size_t		                limit;
        bool		                operatorMode;
        bool		                topicMode;
        bool		                passChanged;
        std::string	                password;
        std::vector<std::string>    opSigns;
        std::vector<std::string>    targetNicknames;
        std::string                 userNickname;
        std::string                 userID;
        bool                        status;
        std::string                 mode;
        parsedMessage::paramsIt     paramIt;
        std::vector<std::string>    outputParams;
        std::string                 pluses;
        std::string                 minuses;
};