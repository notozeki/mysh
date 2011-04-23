#ifndef TOKEN_H
#define TOKEN_H

typedef enum {
	T_STRING,

	// sign
	T_INRED,	// <
	T_OUTRED,	// >
	T_APPEND,	// >>
	T_HEAR,		// <<
	T_PIPE,		// |
	T_OR,		// ||
	T_CONTINUE,	// &
	T_AND,		// &&
	T_DST,		// &n
	T_LPAREN,	// (
	T_RPAREN,	// )
	T_ESCAPE,	// ＼
	T_EOL,		// End Of Line(\n)

	// keyword
	T_FOR,
	T_EXIT,
} TokenId;

typedef union {
	char* string;	// with T_STRING
	int number;	// with T_INRED, T_OUTRED, T_DST
} TokenValue;

typedef struct {
	TokenId id;
	TokenValue value;
} Token;

Token* new_token();
void print_token(Token* tk); // debug用
void delete_token(Token* tk);

#endif // TOKEN_H
