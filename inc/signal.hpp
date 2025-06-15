#ifndef SIGNAL_HPP
#define SIGNAL_HPP

#include <csignal>

namespace sig {
	extern bool stop;
	void signal_handler(int signal);
	void set_signals();
}

#endif