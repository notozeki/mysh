#include <stdlib.h>
#include "../env/flags.h"

int act_cd(char* const argv[])
{
	return 0;
}

int act_exit(char* const argv[])
{
	g_exit_flag = 1;

	if ( argv[1] != NULL ) {
		return atoi(argv[1]);
	}
	return 0;
}

int act_fg(char* const argv[])
{
	return 0;
}
