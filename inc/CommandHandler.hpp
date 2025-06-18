#pragma once
# include "IServerAPI.hpp"
# include "Parser.hpp"
# include <iostream>
# include <map>
# include <string>

class CommandHandler
{
	typedef std::map<std::string, void (CommandHandler::*)(parsed_message&)> commands;
    private:
		IServerAPI&	_srvAPI;
		commands	_commands;
		// void		_inviteFp(parsed_message& parsed_message);
		// void		_joinFp(parsed_message& parsed_message);
		// void		_kickFp(parsed_message& parsed_message);
		// void		_modeFp(parsed_message& parsed_message);
		void		_nickFp(parsed_message& parsed_message);
		void		_passFp(parsed_message& parsed_message);
		// void		_privMsgFp(parsed_message& parsed_message);
		// void		_realFp(parsed_message& parsed_message);
		// void		_topicFp(parsed_message& parsed_message);
		void		_userFp(parsed_message& parsed_message);
		
    public:
		CommandHandler(IServerAPI& srvAPI);
        ~CommandHandler();
		void				execute(void);
		void				execute(parsed_message& parsed_message);
};
