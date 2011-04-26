#include <unistd.h>
#include <errno.h>
#include "../common/error.h"

void invoke_process(const char* path, char* const argv[], int wait)
// NOTE: 引数waitには、子プロセスの終了を待つかどうかの真偽値を渡す（待つ場合は真）。
{
	pid_t child_pid;

	child_pid = fork();
	if ( child_pid > 0 ) { // 親
		if ( wait ) waitpid(child_pid, NULL, 0);
	}
	else if ( child_pid == 0 ) { // 子
		if ( execv(path, argv) < 0) { // パス名展開は勝手にやってほしい
			print_last_error("execv");
		}
	}
	else { // エラー
		print_last_error("fork");
	}
}
