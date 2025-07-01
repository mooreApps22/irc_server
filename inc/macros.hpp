#pragma once

#define BACKLOG		10
#define MAX_EVENTS	10
#define BUFFER_SIZE	512

// Server data
#define SRV				"marvin@serverirc.42.fr"	// Server name
#define VER				"v0.0"
#define DATE			"21/06/2025"				// Creation date
#define UMOD			"o"							// User modes
#define	CMOD			"iklot"						// Channel modes

// Syntax
#define CRLF		"\r\n"


#define SPC					" "
#define COL					":"

// Command replies
// #define RPL_WELCOME(n)				(std::string (COL) + SRV + " 001" + SPC + (n) + SPC + " :Welcome to the Internet Relay Network")
// #define RPL_YOURHOST(n)				(std::string (COL) + SRV + " 002" + SPC + (n) + SPC + " :Your host is " + SRV + ", running VER " + VER)
// #define	RPL_CREATED(n)				(std::string (COL) + SRV + " 003" + SPC + (n) + SPC + " :This server was created "  +  DATE)
// #define RPL_MYINFO(n)				(std::string (COL) + SRV + " 004" + SPC + (n) + SPC + " :" + SRV + SPC + VER + SPC + UMOD + SPC + CMOD)
// #define RPL_CHANNELMODEIS(n, c, m)	(std::string (COL) + SRV + " 324" + SPC + (n) + SPC + (c) + SPC + COL + (m))
// #define	RPL_NOTOPIC(n, c)			(std::string (COL) + SRV + " 331" + SPC + (n) + SPC + (c) + " :No topic is set")
// #define RPL_TOPIC(n, c, t)			(std::string (COL) + SRV + " 332" + SPC + (n) + SPC + (c) + " :" + (t))
// #define RPL_INVITING(n, t, c)		(std::string (COL) + SRV + " 341" + SPC + (n) + SPC + (t) + " :" + (c))
// #define	RPL_NAMREPLY(n, c, ul)		(std::string (COL) + SRV + " 353" + SPC + (n) + " = " + (c) + " :"(ul))
// #define	RPL_ENDOFNAMES(n, c)		(std::string (COL) + SRV + " 366" + SPC + (n) + SPC + (c) + " :End of NAMES list")

// Error replies
// #define ERR_NOSUCHNICK(n, t)			(std::string (COL) + SRV + " 401" + SPC + (n) + SPC + (t) + " :No such nick/channel")
// #define ERR_NOSUCHCHANNEL(n, c) 		(std::string (COL) + SRV + " 403" + SPC + (n) + SPC + (c) + " :No such channel")
// #define ERR_CANNOTSENDTOCHAN(n, c)		(std::string (COL) + SRV + " 404" + SPC + (n) + SPC + (c) + " :<channel name> :Cannot send to channel")
// #define ERR_TOOMANYCHANNELS(n, c) 		(std::string (COL) + SRV + " 405" + SPC + (n) + SPC + (c) + " :<channel name> :You have joined too many channels")
// #define ERR_TOOMANYTARGETS(n, c)		(std::string (COL) + SRV + " 407" + SPC + (n) + SPC + (c) + " :Duplicate recipients. No message delivered")
// #define ERR_NOORIGIN(n)					(std::string (COL) + SRV + " 409" + SPC + (n) + " :No origin specified")
// #define ERR_NORECIPIENT(n, c)			(std::string (COL) + SRV + " 411" + SPC + (n) + " ::No recipient given ("(c)")")
// #define ERR_NOTEXTTOSEND(n)				(std::string (COL) + SRV + " 412" + SPC + (n) + " ::No text to send")
#define ERR_UNKNOWNCOMMAND(n, c) 		(std::string (COL) + SRV + " 421" + SPC + (n) + SPC + (c) + " :Unknown command")
// #define ERR_NONICKNAMEGIVEN(n, c)		(std::string (COL) + SRV + " 431" + SPC + (n) + SPC + (c) + " :No nickname given")
// #define ERR_ERRONEUSNICKNAME(n, t)		(std::string (COL) + SRV + " 432" + SPC + (n) + SPC + (t) + " :Erroneous nickname")
// #define ERR_NICKNAMEINUSE(n, t)			(std::string (COL) + SRV + " 433" + SPC + (n) + SPC + (c) + " :Nickname is already in use")
// #define ERR_NOTONCHANNEL(n, c)			(std::string (COL) + SRV + " 442" + SPC + (n) + SPC + (c) + " :You're not on that channel")
// #define ERR_USERONCHANNEL(t, c)			(std::string (COL) + SRV + " 443" + SPC + (t) + SPC + (c) + " :is already on channel")
// #define ERR_NOTREGISTERED(n, c)			(std::string (COL) + SRV + " 451" + SPC + (n) + SPC + (c) + " :You have not registered")
// #define ERR_NEEDMOREPARAMS(n, c)		(std::string (COL) + SRV + " 461" + SPC + (n) + SPC + (c) + " :Not enough parameters")
#define ERR_ALREADYREGISTRED(n) 		(std::string (COL) + SRV + " 462" + SPC + (n) + " :Unauthorized command (already registered)")
#define ERR_PASSWDMISMATCH(n) 			(std::string (COL) + SRV + " 464" + SPC + (n) + " :Password incorrect")
// #define ERR_CHANNELISFULL(n, c) 		(std::string (COL) + SRV + " 471" + SPC + (n) + SPC + (c) + " :Cannot join channel ( + l)")
// #define ERR_INVITEONLYCHAN(n, c) 		(std::string (COL) + SRV + " 473" + SPC + (n) + SPC + (c) + " :Cannot join channel ( + i)")
// #define ERR_BADCHANNELKEY(n, c) 		(std::string (COL) + SRV + " 475" + SPC + (n) + SPC + (c) + " :Cannot join channel ( + k)")
// #define ERR_CHANOPRIVSNEEDED(n, c)		(std::string (COL) + SRV + " 482" + SPC + (n) + SPC + (c) + " :You're not channel operator")
// #define ERR_UMODEUNKNOWNFLAG(n)			(std::string (COL) + SRV + " 501" + SPC + (n) + " ::Unknown MODE flag")
// #define ERR_USERSDONTMATCH(n) 			(std::string (COL) + SRV + " 502" + SPC + (n) + " :Cannot change mode for other users")
