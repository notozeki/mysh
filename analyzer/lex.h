#ifndef LEX_H
#define LEX_H

#include "token.h"

void lex_set_line(String* line);
Token* lex_get_token();

#endif // LEX_H
