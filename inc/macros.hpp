#pragma once

// Command responses
#define RPL_WELCOME				"001"	// "Welcome to the Internet Relay Network
										// <nick>!<user>@<host>"

#define ERR_UNKNOWNCOMMAND 		"421" // "<command> :Unknown command"
#define ERR_ALREADYREGISTRED 	"462" // ":Unauthorized command (already registered)"
#define ERR_PASSWDMISMATCH 		"464" //": Password incorrect"