#include "ed.h"
#include <string.h>
#include <regex.h>

static regex_t regex;
static char *pattern =
	"^(([$0-9]+)(,([$0-9]+))?)?([a-z])[[:blank:]]*([[:print:]]*)$";
static const size_t nmatch = 6 + 1;

#define CMD_RANGE_BEG 2
#define CMD_RANGE_END 4
#define CMD_CMD       5
#define CMD_ARG       6

void init_regex()
{
	int retval;

	memset(&regex, 0, sizeof(regex_t));
	retval = regcomp(&regex, pattern, REG_EXTENDED);
	if (retval != 0) {
		exit(EXIT_FAILURE);
	}
}

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

	parse_command(command, cmdstr);
	return command;
}

void delete_command(Command *command)
{
	free(command->arg);
	free(command);
}

Command *parse_command(Command *command, const char *cmdstr)
{
	if (cmdstr == NULL || strlen(cmdstr) == 0) {
		return command;
	}

	int retval;
	char *cmd_copy;
	regmatch_t match[nmatch];

	cmd_copy = charalloc(strlen(cmdstr));
	strcpy(cmd_copy, cmdstr);
	/* NOTE: newline must be removed */
	strstrip(cmd_copy);
	retval = regexec(&regex, cmd_copy, nmatch, match, 0);
	if (retval == REG_NOMATCH) {
		return command;
	}

	int len = strlen(cmd_copy);
	char *tmpstr = charalloc(len);
	if (match[CMD_RANGE_BEG].rm_so != -1) {
		int beg = match[CMD_RANGE_BEG].rm_so;
		int end = match[CMD_RANGE_BEG].rm_eo;

		if (beg - end < len) {
			long converted;

			strncpy(tmpstr, cmd_copy + beg, end - beg);
			tmpstr[end] = '\0';
			if (strcmp(tmpstr, "$") == 0) {
				command->range.beg = curbuf->last_line->line_no;
			}
			else {
				converted = strtol(tmpstr, NULL, 10);
				if (converted != LONG_MAX) {
					command->range.beg = converted;
				}
				clrstr(tmpstr);
			}
		}
	}
	if (match[CMD_RANGE_END].rm_so != -1) {
		int beg = match[CMD_RANGE_END].rm_so;
		int end = match[CMD_RANGE_END].rm_eo;

		if (beg - end < len) {
			long converted;

			strncpy(tmpstr, cmd_copy + beg, end - beg);
			tmpstr[end] = '\0';
			if (strcmp(tmpstr, "$") == 0) {
				command->range.end = curbuf->last_line->line_no;
			}
			else {
				converted = strtol(tmpstr, NULL, 10);
				if (converted != LONG_MAX) {
					command->range.end = converted;
				}
				clrstr(tmpstr);
			}
		}
	}
	if (match[CMD_CMD].rm_so != -1) {
		command->cmd = cmd_copy[match[CMD_CMD].rm_so];
	}
	if (match[CMD_ARG].rm_so != -1) {
		int beg = match[CMD_ARG].rm_so;
		int end = match[CMD_ARG].rm_eo;

		if (beg - end < BUFFSIZE) {
			strcpy(command->arg, cmd_copy + beg);
		}
	}
	free(tmpstr);
	free(cmd_copy);

	return command;
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

void print(Command *command)
{
	long beg_no, end_no;

	beg_no = command->range.beg;
	end_no = command->range.end;

	if (curbuf->first_line == NULL) {
		unknown(command);
		return;
	}
	if (beg_no > curbuf->last_line->line_no ||
			end_no > curbuf->last_line->line_no) {
		unknown(command);
		return;
	}

	if (beg_no == -1 && end_no == -1) {
		beg_no = curbuf->cur_line->line_no;
		end_no = curbuf->cur_line->line_no;
	}
	else if (beg_no > 0 && end_no == -1) {
		end_no = beg_no;
	}
	else if (beg_no > 0 && end_no > 0 && beg_no <= end_no) {
		/* legal */
	}
	else {
		unknown(command);
		return;
	}

	Line *beg;
	for (beg = curbuf->first_line; beg->line_no != beg_no; beg = beg->next);
	Line *end;
	for (end = curbuf->first_line; end->line_no != end_no; end = end->next);

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

	/* Renumber the buffer */
	int i;
	Line *p;
	for (i = 1, p = curbuf->first_line; p != NULL; p = p->next, i++) {
		p->line_no = i;
	}
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
