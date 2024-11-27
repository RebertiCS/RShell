#include <stdio.h>
#include <error.h>
#include <errno.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <limits.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <linux/limits.h>
#include <readline/readline.h>
#include <readline/history.h>

#include "jobs.h"
#include "shell.h"
#include "parser.h"

struct {
	char *str;
	int (*func)(shell_node_t *);
} shell_shell[] = {
	{ "q", shell_quit }, { "cd", shell_cd }, { "bg", job_bg },
	{ "fg", job_fg },    { NULL, NULL },
};

char shell_hist[256];

char *shell_prompt(void)
{
	char *ps1 = getenv("PS1");

	if (ps1 == NULL)
		return readline(" % ");

	char *prompt = calloc(1, PATH_MAX);
	prompt[0] = '\0';

	for (char *prompt_ptr = prompt; *ps1 != '\0'; ps1++) {
		prompt_ptr = strrchr(prompt_ptr, '\0');

		if (*ps1 == '\\') {
			switch (ps1[1]) {
			case 'h': {
				gethostname(prompt_ptr, HOST_NAME_MAX);
				break;
			}
			case 'u':
				strcpy(prompt_ptr, getenv("USER"));
				break;

			case 'w': {
				char *home = getenv("HOME");
				char *pwd = getenv("PWD");

				if (pwd[7] == home[7] && pwd[8] == home[8])
					sprintf(prompt_ptr, "~%s",
						pwd + strlen(home));
				else
					sprintf(prompt_ptr, "%s", pwd);
				break;
			}
			case 'W': {
				char *pwd = strrchr(getenv("PWD"), '/');
				strcpy(prompt_ptr,
				       (pwd[1] != '\0') ? ++pwd : pwd);
				break;
			}
			// XXX: Removes colors from PS1.
			case '[':
			case ']':
				break;
			case '0':
				ps1 = strchr(ps1, 'm');
				ps1--;
				break;

			default:
				prompt_ptr[0] = ps1[0];
				break;
			}
			ps1++;
		} else
			prompt_ptr[0] = ps1[0];
	}

	char *shell = readline(prompt);
	free(prompt);

	return shell;
}

int shell_quit(shell_node_t *shell)
{
	shell_free_node(shell);
	job_free_list();
	write_history(shell_hist);
	exit(0);
}

int shell_cd(shell_node_t *shell)
{
	char *new_pwd = (shell->argc < 2) ? getenv("HOME") : shell->argv[1];

	if (chdir(new_pwd) == 0) {
		char pwd[PATH_MAX + 1];
		getcwd(pwd, PATH_MAX);

		setenv("PWD", pwd, 1);
		return 0;
	}

	return -1;
}

int shell_exec(shell_node_t *shell)
{
	for (int i = 0; shell_shell[i].str; ++i) {
		if (strcmp(shell->argv[0], shell_shell[i].str) == 0) {
			shell_shell[i].func(shell);
			return 0;
		}
	}

	switch (shell->pid = fork()) {
	case 0:
		setpgid(getpid(), 0);
		tcsetpgrp(STDIN_FILENO, getpgrp());
		execvp(shell->argv[0], shell->argv);

		if (errno != 0)
			perror("RShell");

		exit(1);

	default:
		job_chk(shell->pid, shell->argv[0]);
	}
	return 0;
}