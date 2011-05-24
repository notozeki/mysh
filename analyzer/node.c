#include <stdlib.h>
#include "node.h"
#include "token.h"
#include "../common/error.h"

Node* new_node()
{
	Node* node;
	node = malloc(sizeof(Node));
	if ( node == NULL ) {
		cannot_continue("malloc");
	}
	node->token = NULL;
	node->left = NULL;
	node->center = NULL;
	node->right = NULL;
	return node;
}

void delete_node(Node* node)
{
	delete_token(node->token);
	free(node);
}

void node_traverse(Node* node, void (*action)(Node* node), TravOrder order)
{
	if ( order == TO_PRE ) {
		action(node);
	}
	if ( node->left != NULL ) node_traverse(node->left, action, order);
	if ( order == TO_IN1 ) {
		action(node);
	}
	if ( node->center != NULL ) node_traverse(node->center, action, order);
	if ( order == TO_IN2 ) {
		action(node);
	}
	if ( node->right != NULL ) node_traverse(node->right, action, order);
	if ( order == TO_POST ) {
		action(node);
	}
}

void delete_tree(Node* root)
{
	node_traverse(root, delete_node, TO_POST);
}
