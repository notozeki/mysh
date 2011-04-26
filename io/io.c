#include <stdio.h>
#include <string.h>
#include "io.h"
#include "../common/string.h"

#define BUF_SIZE 2

void print_prompt()
{
	printf("> ");
}

void get_line(String* str)
{
	char buf[BUF_SIZE];

	string_renew(str, "");

	do {
		fgets(buf, BUF_SIZE, stdin);
		string_append(str, buf);
	} while ( strchr(buf, '\n') == NULL );
}

