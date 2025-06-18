#pragma once
# include <iostream>
# include "Server.hpp"
# include "Parser.hpp"
# include <map>
# include <string>

class Server;

class CommandHandler
{
	typedef std::map<std::string, void (CommandHandler::*)()> commands;
    private:
		commands	_commands;
		Server&		_server;
		Parser&		_parser;
		void		_passFp();
		void		_nickFp();
		void		_userFp();
		void		_realFp();
		void		_joinFp();
		void		_privMsgFp();
		void		_kickFp();
		void		_inviteFp();
		void		_modeFp();
		void		_topicFp();
    public:
		CommandHandler(Server& server, Parser& parser);
        ~CommandHandler();
		void				execute(void);
};
