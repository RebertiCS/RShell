#ifndef PARSER_H
#define PARSER_H

#include <stddef.h>
#include <sys/types.h>

#define CMD_ARRAY_SIZE 8
#define CMD_STRING_SIZE 1024

typedef struct shell_node shell_node_t;

/** Structure that contains information about the command inputed by the user */
struct shell_node {
	size_t argc; /** Ammount of command arguments */
	char **argv; /** String of command arguments */

	pid_t  pid; /** To check the command proccess state(see jobs.h) */

	shell_node_t *next; /** Used when pipes and more complex commands are inputed */
};

/** Takes the user input and parses it
 * @param user_input The string to be parsed.
 * @see shell_free_node(shell_node_t *shell_node);
 * @return a new shell struct that can be executed and shold be mannualy freed.
 */
shell_node_t *parse_user_input(char *user_input);
/** Frees a shell node
 * @param shell_node The node that will be freed.
 * @return Returns 0 for success, olther value for errors.
 */
int shell_free_node(shell_node_t *shell_node);

#endif /* PARSER_H */