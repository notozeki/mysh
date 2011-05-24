#include <string.h>
#include "actions.h"

struct {char* cmd; int (*action)(char* const argv[]);} actions[] = {
	{"cd", act_cd},
	{"exit", act_exit},
	{"fg", act_fg}
};

int embed_command_match(const char* name)
{
	int n = sizeof(actions) / sizeof(actions[0]);
	int i;
	for ( i = 0; i < n; i++ ) {
		if ( strcmp(actions[i].cmd, name) == 0 ) {
			return 1;
		}
	}
	return 0;
}

int embed_command_call(const char* name, char* const argv[])
{
	int n = sizeof(actions) / sizeof(actions[0]);
	int i;
	for ( i = 0; i < n; i++ ) {
		if ( strcmp(actions[i].cmd, name) == 0 ) {
			return actions[i].action(argv);
		}
	}
	return 0;
}
