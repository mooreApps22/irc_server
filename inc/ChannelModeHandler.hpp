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
        ChannelModeHandler(IServerAPI& srvAPI, parsed_message& parsed_msg);
        ~ChannelModeHandler();
        void    handle();

    private:
        IServerAPI& api;    

        std::string param;

        std::string channelName;
        std::string channelId;
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
        parsed_message& parsed_msg;
        std::string user_nickname;
        std::string user_identifier;
        std::string reply_message;
        bool        status;
        std::string mode;
        std::string command;

        
};