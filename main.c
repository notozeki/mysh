#include <stdio.h>
#include "common/string.h"
#include "io/io.h"
#include "analyzer/analyzer.h"
#include "analyzer/node.h"
#include "system/signal.h"
#include "system/executer.h"
#include "env/jobs.h"
#include "env/flags.h"

int main(int argc, char* argv[])
{
	String* buf = new_string("");

	init_signal_handler();
	init_jobs();

	print_welcome();
	while ( !g_exit_flag ) { // main loop
		print_prompt();
		get_line(buf);
		Node* node = analyze_line(buf);
		if ( node ) {
			if ( node->token->id == T_EOL ) {
				delete_tree(node);
				continue;
			}
			execute(node, NULL);
			delete_tree(node);
		}
	}
	print_goodbye();
	delete_string(buf);

	return 0;
}

