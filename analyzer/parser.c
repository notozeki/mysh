#include <stdio.h>
#include <stdlib.h>
#include "analyzer.h"
#include "parser.h"
#include "lexer.h"
#include "token.h"
#include "node.h"

#define MSGBUFSIZE 32
static ParserState st_parser_state;
static const char* st_error_messages[MSGBUFSIZE];
static int st_message_index;
const ParserState parser_state() { return st_parser_state; }

/* prototypes */
void parser_init();
void parser_print_error();
void parse_error(const char* message);
//void parser_continue(const char* prompt, Node* next);

Node* parse_args();
Node* parse_solo_command();
Node* parse_complex_command();
Node* parse_redsign();
Node* parse_src_dst();
Node* parse_append_hear();
Node* parse_redirect();
Node* parse_command();
Node* parse_pipeline();
Node* parse_list();
Node* parse_acceptable();


void parser_init()
{
	st_parser_state = PS_ACCEPT;
	st_message_index = 0;
}

void parser_print_error()
{
	int i;
	for ( i = 0; i < st_message_index; i++ ) {
		fprintf(stderr, "parse error: %s\n", st_error_messages[i]);
	}
}

void parse_error(const char* message)
{
	st_parser_state = PS_ERROR;
	if ( st_message_index == MSGBUFSIZE - 1 ) {
		st_error_messages[st_message_index] = "Too many errors!";
		return;
	}
	st_error_messages[st_message_index] = message;
	st_message_index++;
}

/*
  args ::= arg <args>
*/
Node* parse_args()
{
	//printf("parsing <args> ...\n");
	Token* arg;
	Node* args;
	Node* ret;

	arg = get_token();
	if ( arg->id != T_STRING ) {
		unget_token(arg);
		return NOMATCH;
	}

	ret = new_node();
	ret->token = arg;
	args = parse_args();
	if ( ISMATCH(args) ) {
		ret->left = args;
	}
	else {
		ret->left = NULL;
	}

	return ret;
}

/*
  solo_command ::= file
                 | file <args>
*/
Node* parse_solo_command()
{
	//printf("parsing <solo_command> ...\n");
	Token* file;
	Node* args;
	Node* ret;

	file = get_token();
	if ( file->id != T_STRING ) {
		unget_token(file);
		return NOMATCH;
	}
	
	ret = new_node();
	ret->token = file;
	args = parse_args();
	if ( ISMATCH(args) ) {
		ret->left = args;
	}
	else {
		ret->left = NULL;
	}

	return ret;
}

/*
  complex_command ::= ( <list> )
                    | (and more but not implemented)
*/
Node* parse_complex_command()
{
	//printf("parsing <complex_command> ...\n");
	Token* lparen;
	Node* list;
	Token* rparen;

	lparen = get_token();
	if ( lparen->id != T_LPAREN ) {
		unget_token(lparen);
		return NOMATCH;
	}

	list = parse_list();
	if ( !ISMATCH(list) ) {
		parse_error("Missing right parenthesis `)'");
		return NOMATCH;
	}

	rparen = get_token();
	if ( rparen->id != T_RPAREN ) {
		parse_error("Missing right parenthesis `)'");
		unget_token(rparen);
		return NOMATCH;
	}

	return list;
}

/*
  redsign ::= < | >
*/
Node* parse_redsign()
{
	//printf("parsing <redsign> ...\n");
	Token* in_out;
	Node* ret;

	in_out = get_token();
	if ( !(in_out->id == T_OUTRED || in_out->id == T_INRED) ) {
		unget_token(in_out);
		return NOMATCH;
	}

	ret = new_node();
	ret->token = in_out;
	return ret;
}

/*
  src_dst ::= file | ref
*/
Node* parse_src_dst()
{
	//printf("parsing <src_dst> ...\n");
	Token* file_ref;
	Node* ret;
	
	file_ref = get_token();
	if ( !(file_ref->id == T_STRING || file_ref->id == T_REF) ) {
		unget_token(file_ref);
		parse_error("Missing redirect argument");
		return NOMATCH;
	}

	ret = new_node();
	ret->token = file_ref;
	return ret;
}

/*
  append_hear ::= >> file
                | << eot
*/
Node* parse_append_hear()
{
	//printf("parsing <append_hear> ...\n");
	Token* sign;
	Token* file_eot;
	Node* ret;

	sign = get_token();
	if ( !(sign->id == T_APPEND || sign->id == T_HEAR) ) {
		unget_token(sign);
		return NOMATCH;
	}

	file_eot = get_token();
	if ( file_eot->id != T_STRING ) {
		unget_token(file_eot);
		if ( sign->id == T_APPEND ) parse_error("Missing file name");
		if ( sign->id == T_HEAR ) parse_error("Missing end of token");
		return NOMATCH;
	}

	ret = new_node();
	ret->token = sign;
	ret->left = new_node();
	ret->left->token = file_eot;
	return ret;
}

/*
  redirect ::= <redsign> <src_dst>
             | <append_hear>
             | <redirect> <redirect>
*/
Node* parse_redirect()
{
	//printf("parsing <redirect> ...\n");
	Node* redsign;
	Node* src_dst;
	Node* append_hear;
	Node* redirect;
	Node* ret;

	redsign = parse_redsign();
	if ( ISMATCH(redsign) ) {
		src_dst = parse_src_dst();
		if ( ISMATCH(src_dst) ) {
			redsign->left = src_dst;
			ret = redsign;
			goto next;
		}
		else {
			return NOMATCH;
		}
	}

	append_hear = parse_append_hear();
	if ( ISMATCH(append_hear) ) {
		ret = append_hear;
		goto next;
	}
	
	return NOMATCH;
 next:
	redirect = parse_redirect();
	if ( ISMATCH(redirect) ) {
		ret->center = redirect;
		return ret;
	}
	else {
		return ret;
	}
}

/*
  command ::= <solo_command>
            | <solo_command> <redirect>
            | <complex_command>
	    | <complex_command> <redirect>
*/
Node* parse_command()
{
	//printf("parsing <command> ...\n");
	Node* solo_command;
	Node* complex_command;
	Node* redirect;

	solo_command = parse_solo_command();
	if ( ISMATCH(solo_command) ) {
		redirect = parse_redirect();
		if ( ISMATCH(redirect) ) { // <solo_command> <redirect> にマッチ
			solo_command->center = redirect;
		}
		return solo_command;
	}

	complex_command = parse_complex_command();
	if ( ISMATCH(complex_command) ) {
		redirect = parse_redirect();
		if ( ISMATCH(redirect) ) { // <complex_command> <redirect> にマッチ
			complex_command->center = redirect;
		}
		return complex_command;
	}

	return NOMATCH;
}

/*
  pipeline ::= <command>
             | <command> | <pipeline>
 */
Node* parse_pipeline()
{
	//printf("parsing <pipeline> ...\n");
	Node* command;
	Token* pipe;
	Node* pipeline;
	Node* ret;

	command = parse_command();
	if ( !ISMATCH(command) ) {
		return NOMATCH;
	}

	pipe = get_token();
	if ( pipe->id != T_PIPE ) {
		unget_token(pipe);
		return command;
	}

	pipeline = parse_pipeline();
	if ( !ISMATCH(pipeline) ) {
		parse_error("Missing right side of |");
		return NOMATCH;
	}

	ret = new_node();
	ret->token = pipe;
	ret->left = command;
	ret->right = pipeline;

	return ret;
}

/*
  list ::= <pipeline1>
	 | <pipeline1> && <pipeline2>
	 | <pipeline1> || <pipeline2>
         | <pipeline1> &
	 | <pipeline1> & <pipeline2>
*/
Node* parse_list()
{
	//printf("parsing <list> ...\n");
	Node* pipeline1;
	Token* op;
	Node* pipeline2;
	Node* ret;

	pipeline1 = parse_pipeline();
	if ( !ISMATCH(pipeline1) ) {
		return NOMATCH;
	}
	
	op = get_token();
	switch ( op->id ) {
	case T_AND:
	case T_OR:
	case T_CONTINUE:
		pipeline2 = parse_pipeline();
		if ( ISMATCH(pipeline2) ) {
			ret = new_node();
			ret->token = op;
			ret->left = pipeline1;
			ret->right = pipeline2;
			return ret;
		}
		else {
			if ( op->id == T_CONTINUE ) { // <pipeline1> & にマッチ
				ret = new_node();
				ret->token = op;
				ret->left = pipeline1;
				return ret;
			}
			else {
				if ( op->id == T_AND ) parse_error("Missing right side of &&");
				if ( op->id == T_OR ) parse_error("Missing right side of ||");
				return NOMATCH;
			}
		}
		break;
	default: // <pipeline1> にマッチ
		unget_token(op);
		return pipeline1;
		break;
	}
}

/* 
   acceptable ::= <list> eot
*/
Node* parse_acceptable()
{
	//printf("parsing <acceptable> ...\n");
	Node* list;
	Token* eol;

	list = parse_list();
	if ( !ISMATCH(list) ) {
		return NOMATCH;
	}

	eol = get_token();
	if ( eol->id != T_EOL ) {
		unget_token(eol);
		return NOMATCH;
	}

	return list;
}
