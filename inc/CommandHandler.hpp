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
		int			_fd;
		
		// Function pointers
		void		_inviteFp(parsed_message& parsed_msg);
		void		_joinFp(parsed_message& parsed_msg);
		void		_kickFp(parsed_message& parsed_msg);
		void		_modeFp(parsed_message& parsed_msg);
		void		_nickFp(parsed_message& parsed_msg);
		void		_passFp(parsed_message& parsed_msg);
		void		_pingFp(parsed_message& parsed_msg);
		void		_privMsgFp(parsed_message& parsed_msg);
		void		_realFp(parsed_message& parsed_msg);
		void		_topicFp(parsed_message& parsed_msg);
		void		_userFp(parsed_message& parsed_msg);

		// Utils
		bool				_isNickUnique(const std::string nick);
		const std::string	build_reply(const std::string& prefix, const std::string& command, const std::string message);
		const std::string	build_reply(const std::string& prefix, const std::string& code, const std::string& dest, const std::string message);
		const std::string	build_reply(const std::string& prefix, const std::string& code, const std::string& dest, const std::string arg, const std::string message);

		//Join Utils
		bool				processJoinParams(User* user, std::string chanParams, std::string keyParams);

    public:
		CommandHandler(IServerAPI& srvAPI);
        ~CommandHandler();
		void				execute(void);
		void				execute(parsed_message& parsed_msg, int fd);
		void				setClientFd(int fd);
		int					getClientFd(void) const;
		bool				validChannelName(const std::string& name);
};
