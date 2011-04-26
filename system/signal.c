#include <stdlib.h>
#include <string.h>
#include <signal.h>
#include "handlers.h"
#include "../common/error.h"

struct {int signum; void (*handler)(int);} handler_table[] = {
	{SIGINT, sigint_handler},
};

void regist_signal_handler(int signum, void (*handler)(int));

void init_signal_handler()
{
	int n = sizeof(handler_table) / sizeof (handler_table[0]);
	int i;

	for ( i = 0; i < n; i++ ) {
		regist_signal_handler(handler_table[i].signum, handler_table[i].handler);
	}
}

void regist_signal_handler(int signum, void (*handler)(int))
{
	struct sigaction sa;

	memset(&sa, 0, sizeof(sa));
	sa.sa_handler = handler;
	sa.sa_flags = SA_RESTART;

	if ( sigaction(signum, &sa, NULL) < 0 ) {
		cannot_continue("failed sigaction()");
	}
}
