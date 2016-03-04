#include "ed.h"
#include <string.h>

Command *parse_cmd(char *cmdstr)
{
	Command *command;

	command = new_cmd();
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
	strcpy(command->arg, arg != NULL ? arg : "");

	return command;
}

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
	case 'w':
		func = write_out;
		break;
	default:
		func = unknown;
		break;
	}
	return func;
}

Command *new_cmd()
{
	Command *command;

	command = malloc(sizeof(Command));
	command->range.beg = 0;
	command->range.end = 0;
	command->cmd = '\0';
	command->arg = charalloc(BUFFSIZE);
	return command;
}

void delete_cmd(Command *command)
{
	free(command->arg);
}

void append(Command *command)
{
	int ch;
	char *charbuf;
	int line_no = 0;

	if (curbuf == NULL) {
		curbuf = new_buffer("");
	}
	else {
		line_no = curbuf->last_line->line_no;
	}

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

void write_out(Command *command)
{
	if (curbuf == NULL) {
		/* pass */
	}
	else if (strcmp(curbuf->path, "") == 0) {
		/* Ask the path */
		write_buffer(command->arg);
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
