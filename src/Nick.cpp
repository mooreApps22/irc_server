#include "CommandHandler.hpp"
#include "IServerAPI.hpp"
#include "macros.hpp"
#include "Logger.hpp"
#include <vector>
#include <string>

void	CommandHandler::_nickFp(parsed_message& parsed_msg)
{
	Logger::log(INFO,  parsed_msg.command + " received.");
	
	std::string	user_nickname = _srvAPI.getUserNick();
	std::string	command = parsed_msg.command;
	std::string	new_nickname;
	std::string	reply_message;
	std::string user_identifier;

	if (!_srvAPI.getUserPasswordState())
	{
		reply_message = build_reply(SERVER_NAME, ERR_PASSWDMISMATCH, user_nickname, "Password incorrect");
		_srvAPI.send_reply(reply_message);
		_srvAPI.disconnectUser();
		return ;
	}
	
	if(parsed_msg.params.size() != 1)
	{
		reply_message = build_reply(SERVER_NAME, ERR_NONICKNAMEGIVEN, user_nickname, command, "No nickname given");
		_srvAPI.send_reply(reply_message);
		if( !_srvAPI.isUserRegistered())
			_srvAPI.disconnectUser();
		return ;
	}

	new_nickname = *parsed_msg.getParamsBegin();
	Logger::log(DEBUG, "Checking nick correctness");

	if (!Parser::is_nickname(new_nickname))
	{
		reply_message = build_reply(SERVER_NAME, ERR_ERRONEUSNICKNAME, user_nickname, new_nickname, "Erroneous nickname");
		_srvAPI.send_reply(reply_message);
		_srvAPI.disconnectUser();
		return ;
	}

	Logger::log(DEBUG, "Checking nick uniqueness");
	if (!isNickUnique(new_nickname))
	{
		if (new_nickname != user_nickname)
		{
			reply_message = build_reply(SERVER_NAME, ERR_NICKNAMEINUSE, user_nickname, new_nickname, "Nickname is already in use");
			_srvAPI.send_reply(reply_message);
		}	
		if( !_srvAPI.isUserRegistered())
			_srvAPI.disconnectUser();
		return ;
	}

	if(_srvAPI.isUserRegistered())
	{
		user_identifier = _srvAPI.getUserIdentifier(); 
		reply_message = build_reply(user_identifier, "NICK", new_nickname);
		if (user_nickname != "*")
			_srvAPI.sendToAll(reply_message);
	}
	_srvAPI.setUserNick(new_nickname);
}