#include <stdio.h>
#include <stdlib.h>
#include "token.h"

Token* new_token()
{
	Token* tk = malloc(sizeof(Token));
	if ( tk == NULL ) {
		// abort
	}
	tk->value.string = NULL;
	return tk;
}

void delete_token(Token* tk)
{
	if ( tk->id == T_STRING ) {
		free(tk->value.string);
	}
	free(tk);
}

void print_token(Token* tk)
{
	switch ( tk->id ) {
	case T_STRING:
		printf("Type: string\n");
		printf("Value: %s\n", tk->value.string);
		break;

	case T_INRED:
		printf("Type: input redirect (<)\n");
		printf("Value: %d\n", tk->value.number);
		break;

	case T_OUTRED:
		printf("Type: output redirect (>)\n");
		printf("Value: %d\n", tk->value.number);
		break;

	case T_APPEND:
		printf("Type: append (>>)\n");
		break;

	case T_HEAR:
		printf("Type: hear document (<<)\n");
		break;

	case T_PIPE:
		printf("Type: pipe (|)\n");
		break;

	case T_OR:
		printf("Type: or (||)\n");
		break;

	case T_CONTINUE:
		printf("Type: continue (&)\n");
		break;

	case T_AND:
		printf("Type: and (&&)\n");
		break;

	case T_DST:
		printf("Type: destination (&)\n");
		printf("Value: %d\n", tk->value.number);
		break;

	case T_LPAREN:
		printf("Type: left parenthesis (()\n");
		break;

	case T_RPAREN:
		printf("Type: right parenthesis ())\n");
		break;

	case T_ESCAPE:

		break;

	case T_EOL:
		printf("Type: end of line (\\n)\n");
		break;

	case T_FOR:
		break;

	case T_EXIT:
		break;

	default:
		printf("Type: unknown\n");
		break;
	}
	printf("\n");
}
