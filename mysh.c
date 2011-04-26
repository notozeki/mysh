#include <stdio.h>
#include "common/string.h"
#include "io/io.h"
#include "analyzer/token.h"
#include "analyzer/lex.h"
#include "system/signal.h"

extern char ***environ;

int main(int argc, char* argv[])
{
	String* buf = new_string("");
	Token* t;

	// init signal handler
	init_signal_handler();

	while ( 1 ) { // main loop
		print_prompt();
		get_line(buf);
		// analyze
		// run command (if necessary)
		lex_set_line(buf);
		while ( 1 ) {
			t = lex_get_token();
			if ( t->id == T_EOL ) {
				delete_token(t);
				break;
			}
			else if ( t->id == T_EXIT ) {
				delete_token(t);
				goto loop_end;
			}
			print_token(t);
			delete_token(t);
		}
	}
 loop_end:

	delete_string(buf);

	return 0;
}

