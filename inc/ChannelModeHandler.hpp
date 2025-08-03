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
        struct ChannelMode {
            bool                    isActive;
            char                    flag;
            std::string             param;
                                    ChannelMode(bool, char, const std::string&);
        };
        struct                      ResultStrings {
            std::map<std::string, bool>     opChanges;
            std::vector<std::string>        params;
            std::string             minuses;
            std::string             pluses;
                                    ResultStrings();
            std::string&            operator[](int);
            const std::string&      operator[](int) const;
        };
        ResultStrings               resultStrings;
        std::string                 itoa(int);
        void                        parseInput();
        void                        updateStatuses();
        void                        getResultParams();
        void                        handleKeyChange();
        std::string                 buildResultString();
        void                        handleLimitChange();
        void                        handleOperatorChange();
        
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
};