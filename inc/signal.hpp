#pragma once

#include <csignal>

namespace sig {
	extern bool stop;
	void signalHandler(int signal);
	void setSignals();
}
