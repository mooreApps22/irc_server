#pragma once

#define SERVER_PEFIX			":irc.serverirc.42.fr"
#define SPACE					" "

// Command replies
#define RPL_WELCOME				"001"	// "Welcome to the Internet Relay Network
										// <nick>!<user>@<host>"

// Error replies
#define ERR_UNKNOWNCOMMAND 		"421" 	// "<command> :Unknown command"
#define ERR_NONICKNAMEGIVEN		"431"	// ":No nickname given"
#define ERR_ERRONEUSNICKNAME	"432"	// "<nick> :Erroneous nickname"
#define ERR_NICKNAMEINUSE		"433"	// "<nick> :Nickname is already in use"
#define ERR_NEEDMOREPARAMS		"461"	// "<command> :Not enough parameters"
#define ERR_ALREADYREGISTRED 	"462" 	// ":Unauthorized command (already registered)"
#define ERR_PASSWDMISMATCH 		"464" 	// ": Password incorrect"
