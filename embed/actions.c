#include <stdio.h>
#include <stdlib.h>
#include <limits.h>
#include <unistd.h>
#include <signal.h>
#include <sys/wait.h>
#include "../env/flags.h"
#include "../env/jobs.h"
#include "../common/error.h"

int act_cd(char* const argv[])
{
	int argc;
	for ( argc = 0; argv[argc] != NULL; argc++ ) {
		/* nothing to do */;
	}
	if ( argc == 1 ) {
		char* homepath;
		homepath = getenv("HOME");
		if ( homepath == NULL ) {
			print_last_error("getenv");
			return -1;
		}
		if ( chdir(homepath) < 0 ) {
			print_last_error("chdir");
			return -1;
		}
	}
	else if ( argc == 2 ) {
		char res_path[PATH_MAX];
		if ( realpath(argv[1], res_path) == NULL ) {
			print_last_error("realpath");
			return -1;
		}
		if ( chdir(res_path) < 0 ) {
			print_last_error("chdir");
			return -1;
		}
	}
	else {
		fprintf(stderr, "cd: Too many arguments\n");
		return -1;
	}
	return 0;
}

int act_exit(char* const argv[])
{
	g_exit_flag = 1;

	if ( argv[1] != NULL ) {
		return atoi(argv[1]);
	}
	return 0;
}

int act_fg(char* const argv[])
{
	pid_t pid;
	pid = primary_suspended_job_pid();

	if ( pid == getpid() ) {
		fprintf(stderr, "no suspended jobs\n");
		return -1;
	}
	kill(pid, SIGCONT);
	primary_suspended_job_continue();

	int status;
	waitpid(pid, &status, WUNTRACED);
	
	return ( WIFEXITED(status) )? WEXITSTATUS(status) : -1;
}
