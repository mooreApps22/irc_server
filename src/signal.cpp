#include "signal.hpp"
#include <iostream>
#include <cstring>

bool sig::stop = false;

void sig::signal_handler(int signal)
{
	if (signal == SIGINT){
		std::cout << ": SIGINT catched" << std::endl;
		sig::stop = true;
	}
}

void sig::set_signals()
{
	struct sigaction act;

	std::cout << "Setting signals..." << std::endl;

	std::memset(&act, 0, sizeof act);
	act.sa_handler = sig::signal_handler;
	sigaction(SIGINT, &act, NULL);
}
