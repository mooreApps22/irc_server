#pragma once
# include "IServerAPI.hpp"
# include "Parser.hpp"
# include <map>
# include <string>
class CommandHandler
{
	typedef std::map<std::string, void (CommandHandler::*)(const parsedMessage&) const> commands;
	private:
		IServerAPI&	_srvAPI;
		commands	_commands;
		
		// Function pointers
		void	_inviteFp(const parsedMessage& parsedMsg) const;
		void	_joinFp(const parsedMessage& parsedMsg) const;
		void	_kickFp(const parsedMessage& parsedMsg) const;
		void	_modeFp(const parsedMessage& parsedMsg) const;
		void	_nickFp(const parsedMessage& parsedMsg) const;
		void	_passFp(const parsedMessage& parsedMsg) const;
		void	_pingFp(const parsedMessage& parsedMsg) const;
		void	_privMsgFp(const parsedMessage& parsedMsg) const;
		void	_topicFp(const parsedMessage& parsedMsg) const;
		void	_userFp(const parsedMessage& parsedMsg) const;
		void	_partFp(const parsedMessage& parsedMsg) const;
		void	_quitFp(const parsedMessage& parsedMsg) const;

		// Utils
		// const std::string 	buildReply(const std::string& arg1, const std::string& arg2, const std::string& arg3 = "", const std::string arg4 = "", const std::string arg5 = "", const std::string arg6 = "") const;

	public:
		CommandHandler(IServerAPI& srvAPI);
		~CommandHandler();
		void	execute(const parsedMessage& parsedMsg) const;
		void 	disconnectClient() const;
		void 	disconnectServer() const;
};
