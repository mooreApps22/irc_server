#pragma once

// Server data
#define SERVER_NAME				"marvin@serverirc.42.fr"
#define VERSION					"v0.0"
#define CREATION_DATE			"21/06/2025"
#define USER_MODES				"o"
#define	CHANNEL_MODES			"iklot"

// Syntax
#define SPACE					" "
#define COLON					":"

// Command replies
#define RPL_WELCOME				"001"	// "Welcome to the Internet Relay Network
										// <nick>!<user>@<host>"

#define RPL_YOURHOST			"002"	// "Your host is <servername>, running version <ver>"
#define	RPL_CREATED				"003"	// "This server was created <date>"
#define RPL_MYINFO				"004"	// "<servername> <version> <available user modes> <available channel modes>"
#define RPL_UMODEIS				"221"	// "<user mode string>"
#define RPL_CHANNELMODEIS		"324"	// "<channel> <mode> <mode params>"
#define	RPL_NOTOPIC				"331"	// "<channel> :No topic is set"
#define RPL_TOPIC				"332"	// "<channel> :<topic>"
#define RPL_INVITING			"341"	// "<channel> <nick>"
#define	RPL_NAMREPLY			"353"	// "( "=" / "*" / "@" ) <channel>
										// :[ "@" / "+" ] <nick> *( " " [ "@" / "+" ] <nick> )
										// - "@" is used for secret channels, "*" for private
										// channels, and "=" for others (public channels).
#define	RPL_ENDOFNAMES			"366"	// "<channel> :End of NAMES list"

// Error replies
#define ERR_NOSUCHNICK			"401"	// "<nickname> :No such nick/channel"
#define ERR_NOSUCHCHANNEL 		"403"	// "<channel name> :No such channel"
#define ERR_CANNOTSENDTOCHAN	"404"	// "<channel name> :Cannot send to channel"
                        
#define ERR_TOOMANYCHANNELS 	"405"	// "<channel name> :You have joined too many channels"
#define ERR_TOOMANYTARGETS		"407"	// "<target> :Duplicate recipients. No message delivered"
#define ERR_NOORIGIN			"409"	// ":No origin specified"
#define ERR_NORECIPIENT			"411"	// ":No recipient given (<command>)"
#define ERR_NOTEXTTOSEND		"412"	// ":No text to send"
#define ERR_UNKNOWNCOMMAND 		"421" 	// "<command> :Unknown command"
#define ERR_NONICKNAMEGIVEN		"431"	// ":No nickname given"
#define ERR_ERRONEUSNICKNAME	"432"	// "<nick> :Erroneous nickname"
#define ERR_NICKNAMEINUSE		"433"	// "<nick> :Nickname is already in use"
#define ERR_NOTONCHANNEL		"442"	// "<channel> :You're not on that channel"
#define ERR_USERONCHANNEL		"443"	// "<user> <channel> :is already on channel"
#define ERR_NOTREGISTERED		"451"	// ":You have not registered"
#define ERR_NEEDMOREPARAMS		"461"	// "<command> :Not enough parameters"
#define ERR_ALREADYREGISTRED 	"462" 	// ":Unauthorized command (already registered)"
#define ERR_PASSWDMISMATCH 		"464" 	// ": Password incorrect"
#define ERR_CHANNELISFULL 		"471"	// "<channel> :Cannot join channel (+l)"
#define ERR_INVITEONLYCHAN 		"473"	// "<channel> :Cannot join channel (+i)"
#define ERR_BADCHANNELKEY 		"475"	// "<channel> :Cannot join channel (+k)"
#define ERR_CHANOPRIVSNEEDED	"482"	// "<channel> :You're not channel operator"
#define ERR_UMODEUNKNOWNFLAG	"501"	// ":Unknown MODE flag"
#define ERR_USERSDONTMATCH 		"502"	// ":Cannot change mode for other users"
