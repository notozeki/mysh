#ifndef ANALYZER_H
#define ANALYZER_H

#include "token.h"
#include "node.h"
#include "../common/string.h"

Token* get_token();
void unget_token(Token* tk);
Node* analyze_line(String* str);

#endif // ANALYZER_H
