#pragma once

#define BACKLOG		10
#define MAX_EVENTS	10
#define BUFFER_SIZE	512

// Server data
#define SRV				"marvin@serverirc.42.fr"	// Server name
#define VER				"v0.0"						// Version
#define DATE			"21/06/2025"				// Creation date
#define UMOD			"o"							// User modes
#define	CMOD			"iklot"						// Channel modes

// Syntax
#define CRLF		"\r\n"


#define SPC					" "
#define COL					":"


// Commands
#define CMODE_RPL(uI,c,m)			(std::string (COL) + uI + SPC + "MODE" + SPC + c + SPC + COL + m)
#define ERROR_RPL(n,h)				(std::string ("ERROR :Closing Link: ") + n + "[" + h + "] (Quit: " + n + ")")
#define EXIT_RPL(uI)				(std::string (COL) + uI + SPC + "QUIT :Client exited")
#define INVITE_RPL(uI,t,c)			(std::string (COL) + uI + SPC + "INVITE" + SPC + t + SPC + COL + c)
#define JOIN_RPL(uI,c)				(std::string (COL) + uI + SPC + "JOIN" + SPC + COL + c)
#define KICK_RPL(uI,c,t,r)			(std::string (COL) + uI + SPC + "KICK" + SPC + c + SPC + t + COL + r)
#define NICK_RPL(uI,n)				(std::string (COL) + uI + SPC + "NICK" + SPC + COL + n)
#define PART_RPL(uI,c,m)			(std::string (COL) + uI + SPC + "PART" + SPC + (m.empty()? COL + c : c + SPC + COL + m))
#define PONG_RPL(m)					(std::string (COL) + SRV + SPC + "PONG" + SPC + SRV + SPC + COL + m)
#define PRIVMSG_RPL(uI,t,m)			(std::string (COL) + uI + SPC + "PRIVMSG" + SPC + t + SPC + COL + m)
#define QUIT_RPL(uI,n, m)			(std::string (COL) + uI + SPC + "QUIT" + SPC + (m.empty()? COL + n : COL + m))
#define SERVER_ERROR_RPL			"ERROR :Closing Link: (Server disconnected)"
#define TOPIC_RPL(uI,c,m)			(std::string (COL) + uI + SPC + "TOPIC" + SPC + c + SPC + COL + m)
#define UMODE_RPL(n,m)				(std::string (COL) + n + SPC + "MODE" + SPC + n + SPC + COL + m)


// Command replies
#define RPL_WELCOME(n)				(std::string (COL) + SRV + SPC + "001" + SPC + n + " :Welcome to the Internet Relay Network")
#define RPL_YOURHOST(n)				(std::string (COL) + SRV + SPC + "002" + SPC + n + " :Your host is " + SRV + ", running VER " + VER)
#define	RPL_CREATED(n)				(std::string (COL) + SRV + SPC + "003" + SPC + n + " :This server was created "  +  DATE)
#define RPL_MYINFO(n)				(std::string (COL) + SRV + SPC + "004" + SPC + n + " :" + SRV + SPC + VER + SPC + UMOD + SPC + CMOD)
#define	RPL_UMODEIS(n,m)			(std::string (COL) + SRV + SPC + "221" + SPC + n + SPC + COL + m)
#define RPL_CHANNELMODEIS(n,c,m)	(std::string (COL) + SRV + " 324" + SPC + n + SPC + c + SPC + COL + (m))
#define	RPL_NOTOPIC(n,c)			(std::string (COL) + SRV + SPC + "331" + SPC + n + SPC + c + " :No topic is set")
#define RPL_TOPIC(n,c,t)			(std::string (COL) + SRV + SPC + "332" + SPC + n + SPC + c + " :" + t)
#define RPL_INVITING(n,t,c)			(std::string (COL) + SRV + SPC + "341" + SPC + n + SPC + t + " :" + c)
#define	RPL_NAMREPLY(n,c,ul)		(std::string (COL) + SRV + SPC + "353" + SPC + n + " = " + c + " :" + ul)
#define	RPL_ENDOFNAMES(n,c)			(std::string (COL) + SRV + SPC + "366" + SPC + n + SPC + c + " :End of NAMES list")


// Error replies
#define ERR_NOSUCHNICK(n,t)			(std::string (COL) + SRV + SPC + "401" + SPC + n + SPC + t + " :No such nick/channel")
#define ERR_NOSUCHCHANNEL(n,c) 		(std::string (COL) + SRV + SPC + "403" + SPC + n + SPC + c + " :No such channel")
// #define ERR_CANNOTSENDTOCHAN(n,c)	(std::string (COL) + SRV + SPC + "404" + SPC + n + SPC + c + " :<channel name> :Cannot send to channel")
// #define ERR_TOOMANYCHANNELS(n,c) 	(std::string (COL) + SRV + SPC + "405" + SPC + n + SPC + c + " :<channel name> :You have joined too many channels")
#define ERR_TOOMANYTARGETS(n,c)		(std::string (COL) + SRV + SPC + "407" + SPC + n + SPC + c + " :Duplicate recipients. No message delivered")
#define ERR_NOORIGIN(n)				(std::string (COL) + SRV + SPC + "409" + SPC + n + " :No origin specified")
#define ERR_NORECIPIENT(n,c)		(std::string (COL) + SRV + SPC + "411" + SPC + n + " :No recipient given (" + c + ")")
#define ERR_NOTEXTTOSEND(n)			(std::string (COL) + SRV + SPC + "412" + SPC + n + " :No text to send")
#define ERR_UNKNOWNCOMMAND(n,c) 	(std::string (COL) + SRV + SPC + "421" + SPC + n + SPC + c + " :Unknown command")
#define ERR_NONICKNAMEGIVEN(n,c)	(std::string (COL) + SRV + SPC + "431" + SPC + n + SPC + c + " :No nickname given")
#define ERR_ERRONEUSNICKNAME(n,t)	(std::string (COL) + SRV + SPC + "432" + SPC + n + SPC + t + " :Erroneous nickname")
#define ERR_NICKNAMEINUSE(n,t)		(std::string (COL) + SRV + SPC + "433" + SPC + n + SPC + t + " :Nickname is already in use")
#define ERR_USERNOTINCHANNEL(t,c)	(std::string (COL) + SRV + SPC + "441" + SPC + t + SPC + c + " :They aren't on that channel")
#define ERR_NOTONCHANNEL(n,c)		(std::string (COL) + SRV + SPC + "442" + SPC + n + SPC + c + " :You're not on that channel")
#define ERR_USERONCHANNEL(t,c)		(std::string (COL) + SRV + SPC + "443" + SPC + t + SPC + c + " :is already on channel")
#define ERR_NOTREGISTERED(n,c)		(std::string (COL) + SRV + SPC + "451" + SPC + n + SPC + c + " :You have not registered")
#define ERR_NEEDMOREPARAMS(n,c)		(std::string (COL) + SRV + SPC + "461" + SPC + n + SPC + c + " :Not enough parameters")
#define ERR_ALREADYREGISTRED(n) 	(std::string (COL) + SRV + SPC + "462" + SPC + n + " :Unauthorized command (already registered)")
#define ERR_PASSWDMISMATCH(n) 		(std::string (COL) + SRV + SPC + "464" + SPC + n + " :Password incorrect")
#define ERR_CHANNELISFULL(n,c) 		(std::string (COL) + SRV + SPC + "471" + SPC + n + SPC + c + " :Cannot join channel ( + l)")
#define ERR_INVITEONLYCHAN(n,c) 	(std::string (COL) + SRV + SPC + "473" + SPC + n + SPC + c + " :Cannot join channel ( + i)")
#define ERR_BADCHANNELKEY(n,c) 		(std::string (COL) + SRV + SPC + "475" + SPC + n + SPC + c + " :Cannot join channel ( + k)")
#define ERR_CHANOPRIVSNEEDED(n,c)	(std::string (COL) + SRV + SPC + "482" + SPC + n + SPC + c + " :You're not channel operator")
#define ERR_UMODEUNKNOWNFLAG(n)		(std::string (COL) + SRV + SPC + "501" + SPC + n + " ::Unknown MODE flag")
#define ERR_USERSDONTMATCH(n) 		(std::string (COL) + SRV + SPC + "502" + SPC + n + " :Cannot change mode for other users")