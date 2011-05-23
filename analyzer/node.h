#ifndef NODE_H
#define NODE_H

#include "token.h"

#define NOMATCH NULL
#define ISMATCH(p) ((p) != NOMATCH)

typedef struct tagNode {
	Token* token;
	struct tagNode* left;
	struct tagNode* center;
	struct tagNode* right;
} Node;

Node* new_node();
void delete_node(Node* node);

#endif // NODE_H
