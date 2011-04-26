#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include "error.h"
#include "meta.h"

void print_last_error(const char* place)
{
#ifdef DEBUG
	fprintf(stderr, "%s: %s: %s", PROGRAM_NAME, place, strerror(errno));
#else
	// リリース時の表示には関数名は含めない
	fprintf(stderr, "%s: %s", PROGRAM_NAME, strerror(errno));
#endif
}

void _cannot_continue(const char* place, const char* reason)
{
	fprintf(stderr, "ABORT!: %s: %s\n", place, reason);
	exit(1);
}

void _die_of_bug(const char* place, const char* reason)
{
	fprintf(stderr, "BUG!: %s: %s\n", place, reason);
	exit(1);
}
