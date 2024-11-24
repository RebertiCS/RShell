#include <stdio.h>
#include <stddef.h>
#include <stdlib.h>
#include <string.h>
#include <wordexp.h>
#include <readline/history.h>

#include "parser.h"

shell_node_t *parse_user_input(char *user_input)
{
	if (user_input[0] == '\0')
		return NULL;

	add_history(user_input);

	wordexp_t wexp;
	shell_node_t *new_node = malloc(sizeof(shell_node_t));

	char *buffer = strndup(user_input, strlen(user_input) * 2);

	free(user_input);

	switch (wordexp(buffer, &wexp, 0)) {
	case WRDE_BADCHAR:
		printf("RShell: Illegal occurrence of <, >, (, ), {, }\n");
		free(buffer);
		free(new_node);

		return NULL;

	case WRDE_SYNTAX:
		printf("RShell: Unbalanced parentheses or unmatched quotes\n");
		free(buffer);
		free(new_node);

		return NULL;

	default: {
		shell_node_t *ptr = new_node;

		*ptr = (shell_node_t){
			.argv = calloc(CMD_ARRAY_SIZE, sizeof(char **)),
			.next = NULL,
		};

		for (ptr->argc = 0; ptr->argc < wexp.we_wordc; ++ptr->argc)
			ptr->argv[ptr->argc] = strdup(wexp.we_wordv[ptr->argc]);
	}
	}

	free(buffer);
	wordfree(&wexp);

	return new_node;
}

int shell_free_node(shell_node_t *shell_node)
{
	shell_node_t *ptr = shell_node;

	while (ptr) {
		for (int i = 0; i < ptr->argc; ++i)
			free(ptr->argv[i]);

		free(ptr->argv);

		shell_node_t *old = ptr;
		ptr = ptr->next;

		free(old);
	}

	return 0;
}
