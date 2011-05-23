/*
  analyzer.c -- lexer と parser の糊付け
*/
#include <stdlib.h>
#include "node.h"
#include "lexer.h"
#include "parser.h"
#include "../common/error.h"

static Token* st_token_buf = NULL;

Token* get_token()
{
	Token* ret;

	if ( st_token_buf == NULL ) {
		ret = lex_get_token();
	}
	else {
		ret = st_token_buf;
		st_token_buf = NULL;
	}

	return ret;
}

void unget_token(Token* tk)
{
	if ( st_token_buf == NULL ) {
		st_token_buf = tk;
	}
	else {
		die_of_bug("st_token_buf overflow");
	}
}

Node* analyze_line(String* str)
{
	Node* node;

	st_token_buf = NULL;
	lexer_init(str);
	parser_init();

	node = parse_acceptable();
	switch ( parser_state() ) {
	case PS_ACCEPT:
		break;
	case PS_ERROR:
		parser_print_error();
		if ( node != NULL ) {
			node = NULL;
		}
		break;
	}
	
	if ( st_token_buf != NULL) {
		delete_token(st_token_buf);
		st_token_buf = NULL;
	}

	return node;
}
