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
		
		// Function pointers
		void	_inviteFp(parsed_message& parsed_msg);
		void	_joinFp(parsed_message& parsed_msg);
		void	_kickFp(parsed_message& parsed_msg);
		void	_modeFp(parsed_message& parsed_msg);
		void	_nickFp(parsed_message& parsed_msg);
		void	_passFp(parsed_message& parsed_msg);
		void	_pingFp(parsed_message& parsed_msg);
		void	_privMsgFp(parsed_message& parsed_msg);
		void	_realFp(parsed_message& parsed_msg);
		void	_topicFp(parsed_message& parsed_msg);
		void	_userFp(parsed_message& parsed_msg);
		void	_partFp(parsed_message& parsed_msg);

		// Utils
		bool				isNickUnique(const std::string nick);
		const std::string 	build_reply(const std::string& arg1, const std::string& arg2, const std::string& arg3 = "", const std::string arg4 = "", const std::string arg5 = "", const std::string arg6 = "");

    public:
		CommandHandler(IServerAPI& srvAPI);
        ~CommandHandler();
		void				execute(parsed_message& parsed_msg);
};
