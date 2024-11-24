#ifndef JOBS_H
#define JOBS_H

#include <sys/types.h>

#include "parser.h"

typedef struct job job_t;

/** Linked list used to keep track of proccess jobs. */
struct job {
	char *name; /** The proccess name. */
	int status; /** Last recorded job status. */
	pid_t pid;  /** Used to send signals and change the terminal IOCTL. */

	job_t *next; /** Next job in the linked list. */
};

void signal_handler(int status);

/**
 * @brief Shell built-in used to show the background jobs.
 * 
 * @param shell Not used in this funcion.
 * @return int 
 */
int job_bg(shell_node_t *shell);

/**
 * @brief Send SIGCONT to background proccess and set the foreground group.
 * 
 * @param shell Used to get the wanted proccess PID, oltherwise the first proccess in the job_list will be used.
 * @return int 
 */
int job_fg(shell_node_t *shell);

/**
 * @brief Checks the status of a proccess, if the job is stopped it's put in the job_list.
 * 
 * @param pid The PID of the job.
 * @param name The name of the job.
 * @return int 
 */
int job_chk(pid_t pid, char *name);
int job_add(pid_t pid, char *name, int status);
int job_rem(pid_t pid);
int job_free_list(void);

#endif // JOBS_H