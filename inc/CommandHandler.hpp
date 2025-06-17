#pragma once
# include <iostream>
# include "Server.hpp"
# include "Parser.hpp"

class Server;

class CommandHandler
{
    private:
		Server&	_server;
		Parser&	_parser;
		void	_passFp();
		void	_nickFp();
		void	_userFp();
		void	_realFp();
		void	_joinFp();
		void	_privMsgFp();
		void	_kickFp();
		void	_inviteFp();
		void	_modeFp();
		void	_topicFp();
    public:
		CommandHandler(Server& server, Parser& parser);
        ~CommandHandler();
		void	execute(void);
};
