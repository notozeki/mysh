#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>
#include <string.h>
#include <fcntl.h>
#include "redv.h"
#include "../analyzer/node.h"
#include "../common/error.h"

#define SUCCESS(status) ((status) == 0)

Redv* construct_redv(Node* command_node, Redv* global_redv)
{
	Redv* redv;
	Node* p;

	redv = ( global_redv != NULL )? redv_copy(global_redv) : new_redv();
	p = command_node;
	while ( (p = p->center) ) {
		int fd;
		switch ( p->token->id ) {
		case T_OUTRED:
			fd = open(p->left->token->value.string, O_WRONLY|O_CREAT, 0664);
			if ( fd < 0 ) {
				print_last_error("open");
				break;
			}
			redv_append_entry(redv, p->token->value.number, fd);
			break;
		case T_INRED:
			fd = open(p->left->token->value.string, O_RDONLY);
			if ( fd < 0 ) {
				print_last_error("open");
				break;
			}
			redv_append_entry(redv, p->token->value.number, fd);
			break;
		case T_APPEND:
			fd = open(p->left->token->value.string, O_WRONLY|O_CREAT|O_APPEND, 0664);
			if ( fd < 0 ) {
				print_last_error("open");
				break;
			}
			redv_append_entry(redv, p->token->value.number, fd);
			break;
		case T_HEAR:
			fprintf(stderr, "sorry, but here document is not implemented...\n");
			break;
		default:
			die_of_bug("unexpected token type");
			break;
		}
	}
	if ( redv->ep == 0 ) {
		delete_redv(redv);
		redv = NULL;
	}
	return redv;
}

#define BUFSIZE 256
char** construct_argv(Node* command_node)
{
	Node* p;
	char** buf;
	int index = 0;

	buf = malloc(sizeof(char*) * BUFSIZE);
	if ( buf == NULL ) {
		cannot_continue("malloc");
	}

	p = command_node;
	buf[index] = strdup(p->token->value.string); // コマンド名自体を入れる
	index++;
	while ( (p = p->left) ) {
		if ( p->token->id != T_STRING ) {
			free(buf);
			die_of_bug("unexpected token type");
		}
		if ( index == BUFSIZE ) {
			buf = realloc(buf, BUFSIZE * (index / BUFSIZE + 1));
			if ( buf == NULL ) {
				cannot_continue("realloc");
			}
		}
		buf[index] = strdup(p->token->value.string);
		index++;
	}
	buf[index] = NULL; // NULL tarminate
	return buf;
}

void animate(const char* file, char* const argv[], Redv* redv)
{
        int i;

	if ( redv != NULL ) {
		for (i = 0; i < redv->ep; i++) {
			close(redv->entries[i][0]);
			dup2(redv->entries[i][1], redv->entries[i][0]);
			close(redv->entries[i][1]);
		}
	}

	int status;
	status = execvp(file, argv); // パス名展開は勝手にやってね
	/* もしエラーがあったら復帰してくる */
	print_last_error("execvp");
	exit(-1); // 生きててもらっても困るのでここで楽にしてあげる
}

int execute_command(Node* command_node, Redv* global_redv)
{
	int status, ret;
	pid_t child;
	child = fork();
	if ( child ) { // parent
		waitpid(child, &status, 0);
		ret = ( WIFEXITED(status) )? WEXITSTATUS(status) : -1;
	}
	else if ( child == 0 ) { // child
		char** argv = construct_argv(command_node);
		Redv* redv = construct_redv(command_node, global_redv);

		animate(argv[0], argv, redv);
	}
	else { // error
		print_last_error("fork");
		ret = -1;
	}
	return ret;
}

int execute_and(Node* and_node, Redv* global_redv)
{
	Redv* redv = construct_redv(and_node, global_redv);

	int ret;
	int status = execute_command(and_node->left, redv);
	if ( SUCCESS(status) ) {
		ret = execute_command(and_node->right, redv);
	}
	else {
		ret = status;
	}
	delete_redv(redv);
	return ret;
}

int execute_or(Node* or_node, Redv* global_redv)
{
	Redv* redv = construct_redv(or_node, global_redv);

	int ret;
	int status = execute_command(or_node->left, redv);
	if ( !SUCCESS(status) ) {
		ret = execute_command(or_node->right, redv);
	}
	else {
		ret = status;
	}
	delete_redv(redv);
	return ret;
}

int execute_pipe(Node* pipe_node, Redv* global_redv)
{
	int pipefd[2];

	if ( pipe(pipefd) < 0 ) {
		print_last_error("pipe");
		return -1;
	}

	pid_t child;
	child = fork();
	if ( child < 0 ) {
		print_last_error("fork");
		return -1;
	}
	if ( child == 0 ) { // パイプの左辺側を実行
		close(pipefd[0]);

		char** argv = construct_argv(pipe_node->left);
		Redv* redv = redv_copy(global_redv);
		redv_append_entry(redv, 1, pipefd[1]);
		redv = construct_redv(pipe_node->left, redv);
		animate(argv[0], argv, redv);
	}
	close(pipefd[1]);
	waitpid(child, NULL, 0);

	child = fork();
	if ( child < 0 ) {
		print_last_error("fork");
		return -1;
	}
	if ( child == 0 ) { // パイプの右辺側を実行
		char** argv = construct_argv(pipe_node->right);
		Redv* redv = redv_copy(global_redv);
		redv_append_entry(redv, 0, pipefd[0]);
		redv = construct_redv(pipe_node->right, redv);
		animate(argv[0], argv, redv);
	}
	int status;
	waitpid(child, &status, 0);

	return ( WIFEXITED(status) )? WEXITSTATUS(status) : -1;
}

int execute(Node* root, Redv* global_redv)
{
	switch ( root->token->id ) {
	case T_STRING:
		return execute_command(root, global_redv);
		break;
	case T_AND:
		return execute_and(root, global_redv);
		break;
	case T_OR:
		return execute_or(root, global_redv);
		break;
	case T_PIPE:
		return execute_pipe(root, global_redv);
		break;
	default:
		die_of_bug("unexpected token type");
		break;
	}
}
