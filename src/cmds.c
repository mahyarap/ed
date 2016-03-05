#include "ed.h"
#include <string.h>

function find_function(Command *command)
{
	function func;
	switch (command->cmd) {
	case 'a':
		func = append;
		break;
	case 'q':
		func = quit;
		break;
	case 'e':
		func = read_in;
		break;
	case 'w':
		func = write_out;
		break;
	default:
		func = unknown;
		break;
	}
	return func;
}

void *parse_command(Command *command, char *cmdstr)
{
	if (cmdstr == NULL || strlen(cmdstr) == 0) {
		return command;
	}

	strstrip(cmdstr);
	if (strlen(cmdstr) == 1) {
		command->cmd = cmdstr[0];
		return command;
	}
	/* This is temporary. I just for the moment, allow one command
	 * and one argument, without any range. In the future, I'll add
	 * support for parsing a full command with all its components. */
	char *cmd, *arg;

	cmd = strtok(cmdstr, " ");
	command->cmd = cmd != NULL ? cmd[0] : '\0';
	arg = strtok(NULL, " ");
	if (arg != NULL && strlen(arg) != 0) {
		strcpy(command->arg, arg);
	}
}

Command *new_command(char *cmdstr)
{
	Command *command;

	command = malloc(sizeof(Command));
	command->range.beg = 0;
	command->range.end = 0;
	command->cmd = '\0';
	command->arg = charalloc(BUFFSIZE);

	parse_command(command, cmdstr);
	return command;
}

void delete_cmd(Command *command)
{
	free(command->arg);
	free(command);
}

void append(Command *command)
{
	int ch;
	char *charbuf;
	int line_no = 0;

	/* line_no = curbuf->last_line->line_no; */
	/* Use fgetc */
	while ((charbuf = read_line(stdin)) != NULL) {
		Line *line;

		if (strcmp(charbuf, ".\n") == 0) {
			return;
		}
		line = new_line(charbuf, ++line_no);
		push_back_line(curbuf, line);
	}
}

void read_in(Command *command)
{
	ssize_t retval;

	if (strlen(curbuf->path) == 0) {
		if (command->arg != NULL && strlen(command->arg) != 0) {
			retval = read_file(command->arg);
			if (retval < 0) {
				unknown(command);
				return;
			}

			strcpy(curbuf->path, command->arg);
		}
		else {
			unknown(command);
			return;
		}
	}
	else {
		read_file(curbuf->path);
	}
}

void write_out(Command *command)
{
	int retval;

	if (strlen(curbuf->path) == 0) {
		if (command->arg != NULL && strlen(command->arg) != 0) {
			retval = write_buffer(command->arg);
			if (retval != 0) {
				unknown(command);
				return;
			}
		}
		else {
			unknown(command);
			return;
		}
	}
	else {
		write_buffer(curbuf->path);
	}
}

void quit(Command *command)
{
	exit(EXIT_SUCCESS);
}

void unknown(Command *command)
{
	puts("?");
	return;
}
