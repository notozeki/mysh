#include <stdlib.h>
#include "node.h"
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
	free(node);
}
/*
void print_node(Node* root)
{
	printf();
	print_token(root->token);
	
}
*/
