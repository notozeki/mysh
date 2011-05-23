#ifndef PARSER_H
#define PARSER_H

#include "node.h"

typedef enum {
	PS_ACCEPT,
	PS_CONTINUE,
	PS_ERROR,
} ParserState;

const ParserState parser_state();

void parser_init();
void parser_print_error();

Node* parse_list();
Node* parse_acceptable();

#endif // PARSER_H
