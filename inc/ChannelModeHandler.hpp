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
        std::string channelModeString();
        struct      ChannelMode;

        IServerAPI& api;    

        std::string param;

        std::string channelName;
        std::string channelId;
        bool		i;
        bool		k;
        size_t			l;
        bool		o;
        bool		t;
        bool		passChanged;
        std::string	password;
        int			limit;
        std::string	limitString;
        std::vector<std::string> opSigns;
        std::vector<std::string> targetNicknames;
        std::string	oldKey;
        const parsedMessage& parsedMsg;
        std::string userNickname;
        std::string userID;
        std::string reply_message;
        bool        status;
        std::string mode;
        std::string command;

        
};