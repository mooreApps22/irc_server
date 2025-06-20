#pragma once

#define SERVER_PEFIX			":irc.serverirc.42.fr"
#define SPACE					" "

// Command replies
#define RPL_WELCOME				"001"	// "Welcome to the Internet Relay Network
										// <nick>!<user>@<host>"

// Error replies
#define ERR_UNKNOWNCOMMAND 		"421" // "<command> :Unknown command"
#define ERR_NEEDMOREPARAMS		"461"	// "<command> :Not enough parameters"
#define ERR_ALREADYREGISTRED 	"462" // ":Unauthorized command (already registered)"
#define ERR_PASSWDMISMATCH 		"464" //": Password incorrect"