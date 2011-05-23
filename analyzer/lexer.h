#ifndef LEX_H
#define LEX_H

#include "../common/string.h"
#include "token.h"

void lexer_init(String* line);
Token* lex_get_token();

#endif // LEX_H
