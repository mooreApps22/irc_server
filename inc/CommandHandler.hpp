#pragma once
# include <iostream>
# include "Parser.hpp"
# include <map>
# include <string>

class CommandHandler
{
	typedef std::map<std::string, void (CommandHandler::*)()> commands;
    private:
		commands	_commands;
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
		CommandHandler();
        ~CommandHandler();
		void				execute(void);
		void				execute(parsed_message parsed_message);
};
