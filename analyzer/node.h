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

typedef enum {
	TO_PRE,
	TO_IN1,
	TO_IN2,
	TO_POST,
} TravOrder;

Node* new_node();
void delete_node(Node* node);
void node_traverse(Node* node, void (*action)(Node* node), TravOrder order);
void delete_tree(Node* root);

#endif // NODE_H
