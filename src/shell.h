#ifndef SHELL_H
#define SHELL_H

#include "parser.h"

#ifndef HOST_NAME_MAX
#define HOST_NAME_MAX 256
#endif

char *shell_prompt(void);
int shell_quit(shell_node_t *shell);
int shell_cd(shell_node_t *shell);
int shell_exec(shell_node_t *shell);

extern char shell_hist[256];

#endif /* SHELL_H */