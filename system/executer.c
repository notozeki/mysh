#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>
#include <string.h>
#include <fcntl.h>
#include "redv.h"
#include "../analyzer/node.h"
#include "../common/error.h"
#include "../env/jobs.h"
#include "../embed/embed.h"

#define SUCCESS(status) ((status) == 0)

int execute(Node* root, Redv* global_redv);

Redv* construct_redv(Node* command_node, Redv* global_redv)
{
	Redv* redv;
	Node* p;

	redv = ( global_redv != NULL )? redv_copy(global_redv) : new_redv();
	p = command_node;
	while ( (p = p->center) ) {
		int fd = -1;
		switch ( p->token->id ) {
		case T_OUTRED:
			if ( p->left->token->id == T_STRING ) {
				fd = open(p->left->token->value.string, O_WRONLY|O_CREAT, 0664);
			}
			else { // T_DST
				int i;
				for ( i = 0; i < redv->ep; i++ ) {
					if ( redv->entries[i][0] == p->left->token->value.number ) {
						fd = redv->entries[i][1];
					}
				}
			}
			if ( fd < 0 ) {
				print_last_error("open");
				break;
			}
			redv_append_entry(redv, p->token->value.number, fd);
			break;
		case T_INRED:
			if ( p->left->token->id == T_STRING ) {
				fd = open(p->left->token->value.string, O_RDONLY);
			}
			else { // T_DST
				int i;
				for ( i = 0; i < redv->ep; i++ ) {
					if ( redv->entries[i][0] == p->left->token->value.number ) {
						fd = redv->entries[i][1];
					}
				}
			}
			if ( fd < 0 ) {
				print_last_error("open");
				break;
			}
			redv_append_entry(redv, p->token->value.number, fd);
			break;
		case T_APPEND:
			if ( p->left->token->id == T_STRING ) {
				fd = open(p->left->token->value.string, 
					  O_WRONLY|O_CREAT|O_APPEND, 0664);
			}
			else { // T_DST
				int i;
				for ( i = 0; i < redv->ep; i++ ) {
					if ( redv->entries[i][0] == p->left->token->value.number ) {
						fd = redv->entries[i][1];
					}
				}
			}
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
	if ( embed_command_match(command_node->token->value.string) ) {
		char** argv = construct_argv(command_node);
		return embed_command_call(command_node->token->value.string, argv);
	}

	int status, ret;
	pid_t child;
	child = fork();
	if ( child ) { // parent
		push_job(child, command_node->token->value.string);
		waitpid(child, &status, WUNTRACED);
		if ( !WIFSTOPPED(status) ) { // 停止しただけではない（終了した）とき
			pop_job(child);
		}
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
	int status = execute(and_node->left, redv);
	if ( SUCCESS(status) ) {
		ret = execute(and_node->right, redv);
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
	int status = execute(or_node->left, redv);
	if ( !SUCCESS(status) ) {
		ret = execute(or_node->right, redv);
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

	Redv* redv_left = redv_copy(global_redv);
	redv_append_entry(redv_left, pipefd[0], pipefd[0]); // close(pipefd[0])に相当
	redv_append_entry(redv_left, 1, pipefd[1]);
	redv_left = construct_redv(pipe_node, redv_left);
	execute(pipe_node->left, redv_left);

	close(pipefd[1]);
	int status;
	Redv* redv_right = redv_copy(global_redv);
	redv_append_entry(redv_right, 0, pipefd[0]);
	redv_right = construct_redv(pipe_node, redv_right);
	status = execute(pipe_node->right, redv_right);

	return status;
}

int execute_continue(Node* continue_node, Redv* global_redv)
{
	pid_t child;
	child = fork();
	if ( child ) { // parent
		push_job(child, "subshell");
	}
	else if ( child == 0 ) { // child
		Redv* redv = construct_redv(continue_node, global_redv);

		execute(continue_node->left, redv);
		exit(0); // 返ってきたら死んでください
	}
	else { // error
		print_last_error("fork");
		return -1;
	}
	return 0;
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
	case T_CONTINUE:
		return execute_continue(root, global_redv);
		break;
	default:
		die_of_bug("unexpected token type");
		break;
	}
}
