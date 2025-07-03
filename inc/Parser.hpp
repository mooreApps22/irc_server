#pragma once
# include <string>
# include <vector>

typedef std::vector<std::string>::const_iterator paramsIt;

typedef struct parsedMessage
{
	// std::string					prefix;
	std::string					command;
	std::vector<std::string>	params;
	//	std::string					trailing;
	
	// paramsIt getParamsBegin() const
	// {
	// 	return params.begin();
	// }

	// paramsIt getParamsEnd()
	// {
	// 	return params.end();
	// }
}	parsedMessage;

class Parser
{
	private:
		std::string::iterator		_it;

		// Message grammar
		bool		isMessage(parsedMessage& parsedMsg);
		bool		isColon();
		bool		isPrefix(parsedMessage& parsedMsg);
		// bool		isServername();
		// bool		isNickname();
		bool		isSpace();
		bool		isCommand(parsedMessage& parsedMsg);
		bool		isParams(parsedMessage& parsedMsg);
		bool		isMiddle(parsedMessage& parsedMsg);
		bool		isNospcrlfcl();
		bool		isNull();
		bool		isCR();
		bool		isLF();
		bool		isCRLF();
		bool		isTrailing(parsedMessage& parsedMsg);

		// Helpers
		static bool	isSpecial(std::string::iterator it);
		static bool	isChstring(std::string::iterator it);

	public:
		Parser();
		~Parser();

		bool								parseMessage(std::string& message, parsedMessage& parsedMsg);

		// Helpers
		static bool							isPartial(std::string& message);
		static int							getMessageLength(std::string& message);
		static bool							isNickname(std::string& nickname);
		static bool							isChannel(std::string& channel);
		static std::vector<std::string>		splitParam(const std::string& msgtarget, char c);
		static const std::string 			toLower(const std::string& string);
};
