#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include "meta.h"

void print_last_error(const char* place)
{
#ifdef DEBUG
	fprintf(stderr, "%s: %s: %s\n", PROGRAM_NAME, place, strerror(errno));
#else
	// リリース時の表示には関数名は含めない
	fprintf(stderr, "%s: %s\n", PROGRAM_NAME, strerror(errno));
#endif
}

void _cannot_continue(const char* place, const char* reason)
{
	fprintf(stderr, "ABORT!: at %s: %s() failed\n", place, reason);
	perror(reason);
	exit(1);
}

void _die_of_bug(const char* file, int line, const char* reason)
{
#ifdef DEBUG
	fprintf(stderr, "BUG!: [%s:%d] %s\n", file, line, reason);
#else
	// logファイルにエラーを書き出すとかするとそれっぽい
	fprintf(stderr, "Congratulations! You found %s's bug. Please report me.\n", PROGRAM_NAME);
#endif
	exit(1);
}
