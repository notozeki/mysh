#include <signal.h>
#include <unistd.h>
#include <sys/wait.h>
#include "../env/jobs.h"

void sigint_handler(int signum)
{
	int pid;
	pid = primary_job_pid();

	if ( pid == getpid() ) return; // プライマリジョブが自分自身なら何もしない
	kill((pid_t)pid, SIGINT);
	waitpid(pid, NULL, 0);

	pop_job(pid);
}

void sigquit_handler(int signum)
{
	int pid;
	pid = primary_job_pid();

	if ( pid == getpid() ) return; // プライマリジョブが自分自身なら何もしない
	kill((pid_t)pid, SIGQUIT);
	waitpid(pid, NULL, 0);

	pop_job(pid);
}

void sigtstp_handler(int signum)
{
	int pid;
	pid = primary_job_pid();

	if ( pid == getpid() ) return; // プライマリジョブが自分自身なら何もしない
	kill((pid_t)pid, SIGTSTP);
	//waitpid(pid, NULL, WUNTRACED);

	primary_job_suspend();
}
