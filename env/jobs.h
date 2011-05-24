#ifndef JOBS_H
#define JOBS_H

#include <unistd.h>

void init_jobs();
void push_job(pid_t pid, const char* name);
void push_background_job(pid_t pid, const char* name);
void pop_job(pid_t pid);
pid_t primary_job_pid();
void primary_job_suspend();
pid_t primary_suspended_job_pid();

#endif // JOBS_H
