#include <stdio.h>
#include "common/string.h"
#include "io/io.h"
#include "analyzer/token.h"
#include "analyzer/lex.h"
#include "system/signal.h"
#include "system/process.h"

int main(int argc, char* argv[])
{
	String* buf = new_string("");
	Token* t;
	char* args[2];

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
			else if ( t->id == T_STRING ) {
				args[0] = t->value.string;
				args[1] = NULL;
				invoke_process(t->value.string, args, 1);
			}
			//print_token(t);
			delete_token(t);
		}
	}
 loop_end:

	delete_string(buf);

	return 0;
}

