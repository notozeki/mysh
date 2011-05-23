#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "io.h"
#include "../common/string.h"
#include "../common/meta.h"

#define BUF_SIZE 2

static const char* st_username;
static const char* st_hostname;

void print_welcome()
{
	// ついでにプロンプトの初期化処理とかもしちゃう
	st_username = getenv("USER");
	st_hostname = getenv("HOSTNAME");
	if ( st_username == NULL ) st_username = "";
	if ( st_hostname == NULL ) st_hostname = "";

	printf("Welcome to %s!\n", PROGRAM_NAME);
	printf("To terminate this program, type `exit'\n");
}

void print_goodbye()
{
	printf("bye!\n");
}

void print_prompt()
{
	fprintf(stderr, "[%s]%s@%s $ ",PROGRAM_NAME, st_username, st_hostname);
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

