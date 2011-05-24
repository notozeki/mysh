#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include "../common/error.h"

#define STACKSIZE 256

typedef enum {
	JS_RUNNING,
	JS_BACKGROUND,
	JS_SUSPENDED,
	JS_TERMINATED,
} JobState;

typedef struct {
	pid_t pid;
	char* name;
	JobState state;
	int priority;
} Job;

static Job* st_job_stack[STACKSIZE];
static int st_sp;

Job* new_job()
{
	Job* job;
	job = malloc(sizeof(Job));
	if ( job == NULL ) {
		cannot_continue("malloc");
	}
	job->pid = 0;
	job->name = NULL;
	job->state = JS_TERMINATED;
	job->priority = 0;
	return job;
}

void delete_job(Job* job)
{
	free(job->name);
	free(job);
}

void init_jobs()
{
	st_sp = 0;

	int i;
	for ( i = 0; i < STACKSIZE; i++ ) {
		st_job_stack[i] = NULL;
	}
	Job* thisjob;
	thisjob = new_job();
	thisjob->pid = getpid();
	thisjob->name = "shell";
	thisjob->state = JS_RUNNING;
	st_job_stack[st_sp] = thisjob;
	st_sp++;
}

void cleanup_job_stack()
{
	int i, j;
	for ( i = 0; i < st_sp; i++ ) {
		if ( st_job_stack[i] == NULL) {
			for ( j = i; i < st_sp; j++ ) {
				st_job_stack[j] = st_job_stack[j + 1];
			}
			st_sp--;
		}
		else if ( st_job_stack[i]->state == JS_TERMINATED ) {
			int pri = st_job_stack[i]->priority;
			delete_job(st_job_stack[i]);
			for ( j = i; j < st_sp - 1; j++ ) {
				st_job_stack[j] = st_job_stack[j + 1];
			}
			st_sp--;
			for ( j = 0; j < st_sp; j++ ) { // priority書き直し
				if ( pri < st_job_stack[j]->priority ) {
					st_job_stack[j]->priority--;
				}
			}
		}
	}
}

void inc_jobs_priority()
{
	int i;
	for ( i = 0; i < st_sp; i++ ) {
		st_job_stack[i]->priority++;
	}
}

void dec_jobs_priority()
{
	int i;
	for ( i = 0; i < st_sp; i++ ) {
		st_job_stack[i]->priority--;
	}
}

void push_job(pid_t pid, const char* name)
{
	cleanup_job_stack();

	if ( st_sp == STACKSIZE ) {
		fprintf(stderr, "Too many jobs\n");
	}
	Job* job;
	job = new_job();
	job->pid = pid;
	job->name = strdup(name);
	job->state = JS_RUNNING;
	job->priority = 0;

	inc_jobs_priority();
	st_job_stack[st_sp] = job;
	st_sp++;
}

void push_background_job(pid_t pid, const char* name)
{
	cleanup_job_stack();

	if ( st_sp == STACKSIZE ) {
		fprintf(stderr, "Too many jobs\n");
	}
	Job* job;
	job = new_job();
	job->pid = pid;
	job->name = strdup(name);
	job->state = JS_BACKGROUND;
	job->priority = 0;

	inc_jobs_priority();
	st_job_stack[st_sp] = job;
	st_sp++;
}

void pop_job(pid_t pid)
{
	int i;
	for ( i = 0; i < st_sp; i++ ) {
		if ( pid == st_job_stack[i]->pid ) {
			st_job_stack[i]->state = JS_TERMINATED;
		}
	}
}

pid_t primary_job_pid()
{
	int i;
	Job* min = st_job_stack[0];
	for ( i = 1; i < st_sp; i++ ) {
		if ( st_job_stack[i]->state == JS_RUNNING &&
		     st_job_stack[i]->priority < min->priority ) {
			min = st_job_stack[i];
		}
	}
	return min->pid;
}

void primary_job_suspend()
{
	int i;
	Job* min = st_job_stack[0];
	for ( i = 1; i < st_sp; i++ ) {
		if ( st_job_stack[i]->state == JS_RUNNING &&
		     st_job_stack[i]->priority < min->priority ) {
			min = st_job_stack[i];
		}
	}
	if ( min != st_job_stack[0] ) {
		min->state = JS_SUSPENDED;
	}
}

pid_t primary_suspended_job_pid()
{
	int i;
	Job* min = st_job_stack[0];
	for ( i = 1; i < st_sp; i++ ) {
		if ( st_job_stack[i]->state == JS_SUSPENDED &&
		     st_job_stack[i]->priority < min->priority ) {
			min = st_job_stack[i];
		}
	}
	return min->pid;
}
