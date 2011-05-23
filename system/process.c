#include <unistd.h>
#include <sys/wait.h>
#include "../common/error.h"

typedef struct {
	pid_t pid;
	int pipe[2]; // [0] : in, [1] : out
} Process;

//Process* new_process(const char* file, char* const argv[])
void invoke_process(const char* file, char* const argv[], int wait)
{
	pid_t child_pid;
	//Process* ps;
	int p_p2c[2];
	int p_c2p[2];

	if ( pipe(p_p2c) < 0) {
		cannot_continue("pipe");
	}
	if ( pipe(p_c2p) < 0 ) {
		close(p_p2c[0]);
		close(p_p2c[1]);
		cannot_continue("pipe");
	}

	child_pid = fork();
	if ( child_pid > 0 ) { // 親
		/*
		ps = malloc(sizeof(Process));
		if ( ps == NULL ) {
			cannot_continue("malloc");
		}
		ps->pid = child_pid;
		return ps;
		*/
		if ( wait ) waitpid(child_pid, NULL, 0);
	}
	else if ( child_pid == 0 ) { // 子
		//process_setup();
		//pause(); // すぐ実行を開始しない
		if ( execvp(file, argv) < 0) { // パス名展開は勝手にやってほしい
			// こけたら死ぬ
			cannot_continue("execvp");
		}
	}
	else { // エラー
		print_last_error("fork");
		//return -1;
	}
}

void invoke_processes_with_pipe(const char* file1, char* const argv1[],
				const char* file2, char* const argv2[])
{
	
}
