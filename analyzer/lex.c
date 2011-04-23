#include <stdlib.h>
#include <ctype.h>
#include <string.h>
#include "token.h"
#include "../common/string.h"

typedef enum {
	LS_INITIAL,
	LS_STRING,
	LS_NUMBER,
	LS_SPECIAL,
	LS_SPECIAL_LEX,
	LS_SPACE,
} LexState;

static const char* st_line;
static int st_size;
static int st_index;
static LexState st_state;

static struct {char* keyword; TokenId id;} st_keywords[] = {
	{"exit", T_EXIT},
};


// 先行宣言
void lex_begin_special(String* str);
Token* lex_get_special_token();
void lex_end_special();

int isspecial(int c)
{
	switch ( c ) {
	case '<':
	case '>':
	case '|':
	case '&':
	case '(':
	case ')':
	case '\\':
	case '\n':
		return 1;
		break;
	default:
		return 0;
		break;
	}
}

int get_keyword_id(String* str)
{
	int n = sizeof(st_keywords) / sizeof(st_keywords[0]);
	int i;
	for ( i = 0; i < n; i++ ) {
		if ( string_cmp2(str, st_keywords[i].keyword) == 0 ) {
			return (int)st_keywords[i].id;
		}
	}
	return -1;
}


void lex_set_line(String* line)
{
	st_line = string_c_str(line);
	st_size = string_size(line);
	st_index = 0;
	st_state = LS_INITIAL;
}

Token* lex_get_token()
{
	String* buf = new_string("");
	Token* token = new_token();
	Token* tmp;

	while ( st_index < st_size ) {
		switch ( st_state ) {
		case LS_INITIAL:
			if ( isspace(st_line[st_index]) ) {
				st_state = LS_SPACE;
			}
			else if ( isdigit(st_line[st_index]) ) {
				string_append_char(buf, st_line[st_index]);
				st_state = LS_NUMBER;
			}
			else if ( isspecial(st_line[st_index]) ) {
				string_append_char(buf, st_line[st_index]);
				st_state = LS_SPECIAL;
			}
			else {
				string_append_char(buf, st_line[st_index]);
				st_state = LS_STRING;
			}
			st_index++;
			break;

		case LS_STRING:
			if ( isspace(st_line[st_index]) || isspecial(st_line[st_index]) ) {
				if ( get_keyword_id(buf) >= 0 ) { // keywordかどうかの判定
					token->id = (TokenId)get_keyword_id(buf);
				}
				else {
					token->id = T_STRING;
					token->value.string = strdup(string_c_str(buf));
				}
				st_state = LS_INITIAL;
				goto end;
			}
			else {
				string_append_char(buf, st_line[st_index]);
				st_index++;
			}
			break;

		case LS_NUMBER:
			if ( isspace(st_line[st_index]) ) {
				token->id = T_STRING;
				token->value.string = strdup(string_c_str(buf));
				st_state = LS_INITIAL;
				goto end;
			}
			else if ( isspecial(st_line[st_index]) ) {
				string_append_char(buf, st_line[st_index]);
				st_index++;
				st_state = LS_SPECIAL;
			}
			else if ( !isdigit(st_line[st_index]) ) {
				string_append_char(buf, st_line[st_index]);
				st_index++;
				st_state = LS_STRING;
			}
			else {
				string_append_char(buf, st_line[st_index]);
				st_index++;
			}
			break;

		case LS_SPECIAL:
			if ( isspace(st_line[st_index]) || 
			     !(isdigit(st_line[st_index]) || isspecial(st_line[st_index])) ) {
				lex_begin_special(buf);
				st_state = LS_SPECIAL_LEX;
			}
			else {
				string_append_char(buf, st_line[st_index]);
				st_index++;
			}
			break;

		case LS_SPECIAL_LEX:
			if ( (tmp = lex_get_special_token()) != NULL ) {
				delete_token(token);
				token = tmp;
				goto end;
			}
			else {
				lex_end_special();
				st_state = LS_INITIAL;
			}
			break;

		case LS_SPACE:
			if ( !isspace(st_line[st_index]) ) {
				st_state = LS_INITIAL;
			}
			else {
				st_index++;
			}
			break;

		default:
			// bug!
			break;
		}
	}
	// ここにはこないはず。でも一応。
	token->id = T_EOL;
 end:
	delete_string(buf);
	return token;
}

/* == Special string analyzer ==
 *   >> や 2> や &1 といった複合した形の記号は、ここである程度詳しく解析する。
 */
typedef enum {
	LS_SP_INITIAL,
	LS_SP_NUMBER,
	LS_SP_GT,
	LS_SP_LT,
	LS_SP_AMP,
	LS_SP_VERTICAL,
	LS_SP_OTHER,
} LexSpState;

static char* st_sp_string;
static int st_sp_index;
static int st_sp_size;
static LexSpState st_sp_state;

void lex_begin_special(String* str)
{
	st_sp_string = strdup(string_c_str(str));
	st_sp_index = 0;
	st_sp_size = string_size(str);
	st_sp_state = LS_SP_INITIAL;
}

Token* lex_get_special_token()
{
	String* numbuf = new_string("");
	Token* token = new_token();

	while ( 1 ) {
		char c = st_sp_string[st_sp_index]; // /!\ '\0'が入るかも

		switch ( st_sp_state ) {
		case LS_SP_INITIAL:
			if ( isdigit(c) ) {
				string_append_char(numbuf, c);
				st_sp_index++;
				st_sp_state = LS_SP_NUMBER;
			}
			else if ( c == '>' ) {
				st_sp_index++;
				st_sp_state = LS_SP_GT;
			}
			else if ( c == '<' ) {
				st_sp_index++;
				st_sp_state = LS_SP_LT;
			}
			else if ( c == '&' ) {
				st_sp_index++;
				st_sp_state = LS_SP_AMP;
			}
			else if ( c == '|' ) {
				st_sp_index++;
				st_sp_state = LS_SP_VERTICAL;
			}
			else if ( c == '\0') {
				delete_token(token);
				return NULL;
			}
			else {
				st_sp_state = LS_SP_OTHER;
			}
			break;

		case LS_SP_NUMBER:
			if ( c == '>' ) {
				token->id = T_OUTRED;
				token->value.number = atoi(string_c_str(numbuf));
				st_sp_index++;
				st_sp_state = LS_SP_INITIAL;
				goto end;
			}
			else if ( c == '<' ) {
				token->id = T_INRED;
				token->value.number = atoi(string_c_str(numbuf));
				st_sp_index++;
				st_sp_state = LS_SP_INITIAL;
				goto end;
			}
			else if ( !isdigit(c) ) {
				token->id = T_STRING;
				token->value.string = strdup(string_c_str(numbuf));
				st_sp_state = LS_SP_INITIAL;
				goto end;
			}
			else {
				string_append_char(numbuf, c);
				st_sp_index++;
			}
			break;

		case LS_SP_GT:
			if ( c == '>' ) {
				token->id = T_APPEND;
				st_sp_index++;
			}
			else {
				token->id = T_OUTRED;
				token->value.number = 0; // デフォルトでのリダイレクト先は0
			}
			st_sp_state = LS_SP_INITIAL;
			goto end;
			break;

		case LS_SP_LT:
			if ( c == '<' ) {
				token->id = T_HEAR;
				st_sp_index++;
			}
			else {
				token->id = T_INRED;
				token->value.number = 0; // デフォルトでのリダイレクト元は0
			}
			st_sp_state = LS_SP_INITIAL;
			goto end;
			break;

		case LS_SP_AMP:
			if ( c == '&' ) {
				token->id = T_AND;
				st_sp_index++;
			}
			else if ( isdigit(c) ) {
				do {
					string_append_char(numbuf, c);
					st_sp_index++;
					c = st_sp_string[st_sp_index];
				} while ( isdigit(c) );
				token->id = T_DST;
				token->value.number = atoi(string_c_str(numbuf));
			}
			else {
				token->id = T_CONTINUE;
			}
			st_sp_state = LS_SP_INITIAL;
			goto end;
			break;

		case LS_SP_VERTICAL:
			if ( c == '|' ) {
				token->id = T_OR;
				st_sp_index++;
			}
			else {
				token->id = T_PIPE;
			}
			st_sp_state = LS_SP_INITIAL;
			goto end;
			break;

		case LS_SP_OTHER:
			if ( c == '(' ) {
				token->id = T_LPAREN;
			}
			else if ( c == ')' ) {
				token->id = T_RPAREN;
			}
			else if ( c == '\n' ) {
				token->id = T_EOL;
			}
			else {
				// bug!
			}
			st_sp_index++;
			st_sp_state = LS_SP_INITIAL;
			goto end;
			break;

		default:
			// bug!
			break;
		}
	}
 end:
	delete_string(numbuf);
	return token;
}

void lex_end_special()
{
	free(st_sp_string);
}
