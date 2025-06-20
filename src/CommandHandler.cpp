#include "CommandHandler.hpp"
# include "IServerAPI.hpp"
#include "macros.hpp"
#include "Logger.hpp"
#include <vector>
#include <string>


CommandHandler::CommandHandler(IServerAPI& srvAPI)
	:	_srvAPI(srvAPI)
{
	Logger::log(INFO, "CommandHandler constructed.");

	_commands["INVITE"]		= &CommandHandler::_inviteFp;
	_commands["JOIN"]		= &CommandHandler::_joinFp;
	_commands["KICK"]		= &CommandHandler::_kickFp;
	_commands["MODE"]		= &CommandHandler::_modeFp;
	_commands["NICK"]		= &CommandHandler::_nickFp;
	_commands["PASS"]		= &CommandHandler::_passFp;
	_commands["PRIVMSG"]	= &CommandHandler::_privMsgFp;
	_commands["REAL"] 		= &CommandHandler::_realFp;
	_commands["TOPIC"] 		= &CommandHandler::_topicFp;
	_commands["USER"] 		= &CommandHandler::_userFp;
}

CommandHandler::~CommandHandler()
{
	Logger::log(INFO, "CommandHandler destructed.");
}

void	CommandHandler::execute(parsed_message& parsed_msg)
{
	const std::string command =  parsed_msg.command;
	commands::iterator it = _commands.find(command);

	if (it != _commands.end())
	{
		(this->*(it->second))(parsed_msg);
	}
	else
		Logger::log(INFO, "Unknown Command.", command);
}

void	CommandHandler::_inviteFp(parsed_message& parsed_msg)
{
	Logger::log(INFO,  parsed_msg.command + " received.");
	_srvAPI.send_reply("You've sent a" +  parsed_msg.command + "request!");
}

void	CommandHandler::_joinFp(parsed_message& parsed_msg)
{
	Logger::log(INFO,  parsed_msg.command + " received.");
	_srvAPI.send_reply("You've sent a" +  parsed_msg.command + "request!");
}

void	CommandHandler::_kickFp(parsed_message& parsed_msg)
{
	Logger::log(INFO,  parsed_msg.command + " received.");
	_srvAPI.send_reply("You've sent a" +  parsed_msg.command + "request!");
}

void	CommandHandler::_modeFp(parsed_message& parsed_msg)
{
	Logger::log(INFO,  parsed_msg.command + " received.");
	_srvAPI.send_reply("You've sent a" +  parsed_msg.command + "request!");
}

void	CommandHandler::_nickFp(parsed_message& parsed_msg)
{
	Logger::log(INFO,  parsed_msg.command + " received.");
	
	// TODO check if there are arguments!!!
	
	if (_srvAPI.getUserPasswordState())
	{
		Logger::log(DEBUG, "Checking uniqueness");

		std::string nickname = *parsed_msg.getParamsBegin();

		if (!_isNickUnique(nickname))
		{
			_srvAPI.send_reply("The nickname provided is already in use!"); // TODO send proper reply
			_srvAPI.disconnectUser();
			return ;
		}
		
		_srvAPI.send_reply("Nice nickname election, congrats!"); // TODO send proper reply
		
		_srvAPI.setUserNick(nickname);
		if(_srvAPI.isUserRegistered())
			(void) parsed_msg;	// TODO change UserNIck In all channels keys
	}

	else
	{
		_srvAPI.send_reply("You have not provided any Password!"); // TODO send proper reply
		_srvAPI.disconnectUser();
	}

}

void	CommandHandler::_passFp(parsed_message& parsed_msg)
{
	Logger::log(INFO,  parsed_msg.command + " received.");
	if(!_srvAPI.isUserRegistered())
	{
		if(_srvAPI.isPasswordValid( parsed_msg.params.at(0)))
		{
			_srvAPI.setUserPasswordState(true);
			_srvAPI.send_reply("You got it, mate!");

		}
		else
		{
			_srvAPI.send_reply("Wrong Password!"); // Kick out (message?)
			_srvAPI.disconnectUser();
		}
	}
	else
		_srvAPI.send_reply("You're already registered, mate!");
}

void	CommandHandler::_privMsgFp(parsed_message& parsed_msg)
{
	Logger::log(INFO,  parsed_msg.command + " received.");
	_srvAPI.send_reply("You've sent a" +  parsed_msg.command + "request!");
}

void	CommandHandler::_realFp(parsed_message& parsed_msg)
{
	Logger::log(INFO,  parsed_msg.command + " received.");
	_srvAPI.send_reply("You've sent a" +  parsed_msg.command + "request!");
}

void	CommandHandler::_topicFp(parsed_message& parsed_msg)
{
	Logger::log(INFO,  parsed_msg.command + " received.");
	_srvAPI.send_reply("You've sent a" +  parsed_msg.command + "request!");
}

void	CommandHandler::_userFp(parsed_message& parsed_msg)
{
	Logger::log(INFO,  parsed_msg.command + " received.");
	// TODO check number of params!!!!
	paramsIt it = parsed_msg.getParamsBegin();

	_srvAPI.setUserRegisteredStatus(true);

	std::string message = RPL_WELCOME;
	message.append(":Welcome to the Internet Relay Network\n");
	message.append(_srvAPI.getUserNick());
	message.append("!");
	message.append(*++it);
	message.append("@");
	message.append(*++it);
	_srvAPI.send_reply(message);
}

bool	CommandHandler::_isNickUnique(const std::string nick)
{
	for (usrsIt it = _srvAPI.getUsersBegin(); it != _srvAPI.getUsersEnd(); it++)
	{
		if(it->second->getNickname() == nick)
			return false;
	}
	return true;
}