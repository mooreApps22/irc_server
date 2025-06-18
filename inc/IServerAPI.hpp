#pragma once

#include <string>

class IServerAPI
{
public:
	virtual ~IServerAPI() {}

	// Server
	virtual void send_reply(const std::string& message) = 0;

	// User
	virtual bool is_user_registered() = 0;
};