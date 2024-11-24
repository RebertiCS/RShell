#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>
#include <sys/types.h>

#include "parser.h"
#include "jobs.h"

job_t *job_list;
pid_t job_rn;

void signal_handler(int status)
{
	switch (status) {
	case SIGINT:
		kill(job_rn, SIGCONT);
		kill(job_rn, SIGINT);
		break;
	case SIGTSTP:
		kill(job_rn, SIGSTOP);
		break;
	case SIGSEGV:
		fprintf(stderr, "RShell: Sigment Fault\n");
		break;
	}
}

int job_bg(shell_node_t *shell)
{
	for (job_t *ptr = job_list; ptr; ptr = ptr->next) {
		if (ptr->pid != 0)
			printf("[ %s - %d ]\n", ptr->name, ptr->pid);
	}

	return 0;
}

int job_fg(shell_node_t *shell)
{
	char *job_name;
	pid_t job_pid;

	if (shell->argc > 1) {
		job_pid = atoi(shell->argv[1]);
		for (job_t *job = job_list;; job = job->next) {
			if (job_pid == job->pid) {
				job_name = job->name;
				break;
			}
		}
	} else {
		job_name = job_list->name;
		job_pid = job_list->pid;
	}

	if (kill(job_pid, SIGCONT) == 0)
		tcsetpgrp(STDIN_FILENO, job_pid);

	job_chk(job_pid, job_name);
	tcsetpgrp(STDIN_FILENO, getpid());

	return 0;
}

int job_chk(pid_t pid, char *name)
{
	job_rn = pid;

	int status = 0;

	while (!WIFSIGNALED(status)) {
		waitpid(pid, &status, WUNTRACED);

		if (WIFSTOPPED(status)) {
			printf("[ Stopped: %s - %d ]\n", name, pid);
			job_add(pid, name, SIGSTOP);
			break;
		}

		if (WIFEXITED(status)) {
			job_rem(pid);
			break;
		}
	}

	job_rn = 0;
	return 0;
}

int job_add(pid_t pid, char *name, int status)
{
	if (!job_list)
		job_list = calloc(1, sizeof(job_t));

	job_t *old = NULL;
	for (job_t *ptr = job_list; ptr; ptr = ptr->next) {
		// Used when a new item in the linked list should be created.
		old = ptr;

		if (ptr->pid == pid) {
			ptr->status = status;
			return 0;
		}

		if (ptr->pid == 0) {
			*ptr = (job_t){
				.name = strdup(name),
				.status = status,
				.pid = pid,
				.next = ptr->next,
			};

			return 0;
		}
	}

	old->next = malloc(sizeof(job_t));
	*old->next = (job_t){
		.name = strdup(name),
		.pid = pid,
		.status = status,
	};

	return 0;
}

int job_rem(pid_t pid)
{
	for (job_t *ptr = job_list; ptr; ptr = ptr->next) {
		if (ptr->pid == pid) {
			printf("[ Exited: %s - %d ]\n", ptr->name, ptr->pid);

			ptr->status = 0;
			ptr->pid = 0;
			free(ptr->name);

			// Remove the first element if it's empty
			if (ptr == job_list) {
				job_t *old = job_list;
				job_list = job_list->next;

				free(old);
			}
			return 0;
		}
	}
	return 0;
}

int job_free_list(void)
{
	if (!job_list)
		return -1;

	job_t *ptr = job_list;

	while (ptr) {
		job_t *old = ptr;

		ptr = ptr->next;

		if (old->pid != 0)
			free(old->name);

		free(old);
	}

	return 0;
}
