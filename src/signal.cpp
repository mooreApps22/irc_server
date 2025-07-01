#include "signal.hpp"
#include <cstring>

bool sig::stop = false;

void sig::signalHandler(int signal)
{
	if (signal == SIGINT)
		sig::stop = true;
}

void sig::setSignals()
{
	struct sigaction act;

	std::memset(&act, 0, sizeof act);
	act.sa_handler = sig::signalHandler;
	sigaction(SIGINT, &act, NULL);
}
