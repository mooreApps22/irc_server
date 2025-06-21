#pragma once

#define SERVER_PEFIX			":irc.serverirc.42.fr"
#define SERVER_NAME				"marvin@serverirc.42.fr"
#define VERSION					"v0.0"
#define SPACE					" "
#define CREATION_DATE			"21/06/2025"
#define USER_MODES				"o"
#define	CHANNEL_MODES			"iklot"

// Command replies
#define RPL_WELCOME				"001"	// "Welcome to the Internet Relay Network
										// <nick>!<user>@<host>"

#define RPL_YOURHOST			"002"	// "Your host is <servername>, running version <ver>"
#define	RPL_CREATED				"003"	// "This server was created <date>"
#define RPL_MYINFO				"004"	// "<servername> <version> <available user modes> <available channel modes>"

// Error replies
#define ERR_UNKNOWNCOMMAND 		"421" 	// "<command> :Unknown command"
#define ERR_NONICKNAMEGIVEN		"431"	// ":No nickname given"
#define ERR_ERRONEUSNICKNAME	"432"	// "<nick> :Erroneous nickname"
#define ERR_NICKNAMEINUSE		"433"	// "<nick> :Nickname is already in use"
#define ERR_NEEDMOREPARAMS		"461"	// "<command> :Not enough parameters"
#define ERR_ALREADYREGISTRED 	"462" 	// ":Unauthorized command (already registered)"
#define ERR_PASSWDMISMATCH 		"464" 	// ": Password incorrect"