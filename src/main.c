#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <readline/readline.h>
#include <readline/history.h>

#include "jobs.h"
#include "parser.h"
#include "shell.h"

int main(void)
{
	signal(SIGINT, &signal_handler);
	signal(SIGTSTP, &signal_handler);
	signal(SIGCHLD, &signal_handler);
	signal(SIGSEGV, &signal_handler);
	signal(SIGTTOU, SIG_IGN);

	setpgid(getpid(), 0);
	tcsetpgrp(STDIN_FILENO, getpid());

	using_history();
	stifle_history(1000);

	sprintf(shell_hist, "%s/.local/share/rshell_history", getenv("HOME"));
	read_history(shell_hist);

	while (1) {
		shell_node_t *shell = parse_user_input(shell_prompt());

		if (shell) {
			shell_exec(shell);
			shell_free_node(shell);

			setpgid(getpid(), 0);
			tcsetpgrp(STDIN_FILENO, getpid());
		}
	}
	return 0;
}
