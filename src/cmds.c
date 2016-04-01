#include "ed.h"
#include <string.h>
#include <regex.h>

static void renumber_buffer(Buffer *buffer);
static Line *find_line(long line_no);

function find_function(Command *command)
{
	function func;
	switch (command->cmd) {
	case 'a':
		func = append;
		break;
	case 'd':
		func = delete;
		break;
	case 'e':
		func = edit;
		break;
	case 'f':
		func = find;
		break;
	case 'p':
		func = print;
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

Command *new_command(const char *cmdstr)
{
	Command *command;

	command = malloc(sizeof(Command));
	command->range.beg = -1;
	command->range.end = -1;
	command->cmd = '\0';
	command->arg = charalloc(PATH_MAX);

	return command;
}

void delete_command(Command *command)
{
	free(command->arg);
	free(command);
}

void append(Command *command)
{
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

void delete(Command *command)
{
	long beg_no, end_no;

	beg_no = command->range.beg;
	end_no = command->range.end;
	if (beg_no < 1 || end_no < 1) {
		unknown(command);
		return;
	}

	Line *beg, *end;
	beg = find_line(beg_no);
	end = find_line(end_no);

	Line *p;
	Line *tmp;
	for (p = beg; p != NULL && p->line_no <= end->line_no;) {
		tmp = p->next;
		remove_line(curbuf, p);
		p = tmp;
	}
	renumber_buffer(curbuf);
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

void find(Command *command)
{
	if (strlen(curbuf->path) != 0) {
		puts(curbuf->path);
	}
	else {
		unknown(command);
	}
}

void print(Command *command)
{
	long beg_no, end_no;

	beg_no = command->range.beg;
	end_no = command->range.end;
	if (beg_no < 1 || end_no < 1) {
		unknown(command);
		return;
	}

	Line *beg, *end;
	beg = find_line(beg_no);
	end = find_line(end_no);

	Line *p;
	for (p = beg; p != NULL && p->line_no <= end->line_no; p = p->next) {
		printf("%s", p->text);
	}
}

void read_in(Command *command)
{
	ssize_t retval;

	if (strlen(curbuf->path) == 0 && strlen(command->arg) == 0) {
		unknown(command);
		return;
	}

	if (strlen(curbuf->path) == 0) {
		strcpy(curbuf->path, command->arg);
	}
	retval = read_file(curbuf,
			(strlen(command->arg) != 0) ? command->arg : curbuf->path);
	if (retval < 0) {
		unknown(command);
		return;
	}

	renumber_buffer(curbuf);
	printf("%ld\n", retval);
}

void write_out(Command *command)
{
	if (strlen(curbuf->path) == 0 && strlen(command->arg) == 0) {
		unknown(command);
		return;
	}

	if (strlen(curbuf->path) == 0) {
		strcpy(curbuf->path, command->arg);
	}
	ssize_t retval;
	retval = write_buffer(
			(strlen(command->arg) != 0) ? command->arg : curbuf->path);
	if (retval < 0) {
		unknown(command);
		return;
	}
	curbuf->modified = false;
	printf("%ld\n", retval);
}

void quit(Command *command)
{
	if (curbuf->modified) {
		curbuf->modified = false;
		unknown(command);
		return;
	}
	exit(EXIT_SUCCESS);
}

void unknown(Command *command)
{
	puts("?");
}

static void renumber_buffer(Buffer *buffer)
{
	int i;
	Line *p;

	for (i = 1, p = curbuf->first_line; p != NULL; p = p->next, i++) {
		p->line_no = i;
	}
}

static Line *find_line(long line_no)
{
	Line *line;

	for (line = curbuf->first_line; line->line_no != line_no;
			line = line->next);
	return line;
}
