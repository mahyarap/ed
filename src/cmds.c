#include "ed.h"
#include <string.h>

Command *parse_cmd(char *cmdstr)
{
	int beg = 0;
	Command *command;

	strstrip(cmdstr);
	command = malloc(sizeof(Command));
	if (strlen(cmdstr) == 1) {
		strcpy(command->cmd, cmdstr);
		command->range.beg = 0;
		command->range.end = 0;
		return command;
	}
}

function find_function(Command *command)
{
	function func;
	switch (command->cmd[0]) {
	case 'a':
		func = append;
		break;
	case 'q':
		func = quit;
		break;
	default:
		func = unknown;
		break;
	}
	return func;
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

void quit(Command *command)
{
	exit(EXIT_SUCCESS);
}

void unknown(Command *command)
{
	puts("?");
	return;
}
