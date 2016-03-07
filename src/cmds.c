#include "ed.h"
#include <string.h>

function find_function(Command *command)
{
	function func;
	switch (command->cmd) {
	case 'a':
		func = append;
		break;
	case 'e':
		func = edit;
		break;
	case 'f':
		func = find;
		break;
	case 'r':
		func = read_in;
		break;
	case 'w':
		func = write_out;
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

void delete_command(Command *command)
{
	free(command->arg);
	free(command);
}

Command *parse_command(Command *command, char *cmdstr)
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
	command->cmd = (cmd != NULL) ? cmd[0] : '\0';
	arg = strtok(NULL, " ");
	if (arg != NULL && strlen(arg) != 0) {
		strcpy(command->arg, arg);
	}
	return command;
}

void append(Command *command)
{
	int ch;
	int line_no;
	char *charbuf;

	line_no = (curbuf->last_line != NULL) ? curbuf->last_line->line_no : 0;
	while ((charbuf = read_line(stdin)) != NULL) {
		Line *line;

		if (strcmp(charbuf, ".\n") == 0) {
			return;
		}
		line = new_line(charbuf, ++line_no);
		push_back_line(curbuf, line);
	}
}

void find(Command *command)
{
	if (strlen(curbuf->path) != 0) {
		puts(curbuf->path);
	}
	else {
		unknown(command);
	}
}

void edit(Command *command)
{
	Buffer *buffer;
	ssize_t retval;
	char path[PATH_MAX];

	if (curbuf->modified) {
		curbuf->modified = false;
		unknown(command);
		return;
	}
	if (strlen(curbuf->path) == 0 && strlen(command->arg) == 0) {
		unknown(command);
		return;
	}

	strcpy(path, (strlen(command->arg) != 0) ? command->arg : curbuf->path);
	if (strlen(curbuf->path) == 0) {
		strcpy(curbuf->path, path);
	}
	buffer = curbuf;
	if (curbuf->first_line != NULL) {
		delete_buffer(curbuf);
		buffer = new_buffer(path);
		curbuf = buffer;
	}
	retval = read_file(buffer, path);
	if (retval < 0) {
		delete_buffer(buffer);
		unknown(command);
		return;
	}
	buffer->modified = false;
	printf("%ld\n", retval);
}

void read_in(Command *command)
{
	ssize_t retval;
	char path[PATH_MAX];

	if (curbuf->modified) {
		curbuf->modified = false;
		unknown(command);
		return;
	}
	if (strlen(curbuf->path) == 0 && strlen(command->arg) == 0) {
		unknown(command);
		return;
	}

	strcpy(path, (strlen(command->arg) != 0) ? command->arg : curbuf->path);
	if (strlen(curbuf->path) == 0) {
		strcpy(curbuf->path, path);
	}
	retval = read_file(curbuf, path);
	if (retval < 0) {
		unknown(command);
		return;
	}
	printf("%ld\n", retval);
}

void write_out(Command *command)
{
	char *path;
	ssize_t retval;

	if (strlen(curbuf->path) == 0) {
		if (strlen(command->arg) != 0) {
			strcpy(curbuf->path, command->arg);
		}
		else {
			unknown(command);
			return;
		}
	}

	path = (strlen(command->arg) != 0) ? command->arg : curbuf->path;
	retval = write_buffer(path);
	if (retval < 0) {
		unknown(command);
		return;
	}
	printf("%ld\n", retval);
}

void quit(Command *command)
{
	exit(EXIT_SUCCESS);
}

void unknown(Command *command)
{
	puts("?");
}
