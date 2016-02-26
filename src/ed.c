#include "ed.h"
#include <stdio.h>
#include <stdlib.h>
#include <getopt.h>
#include <string.h>

void print_help()
{
	puts("\
Ed - The line editor.\n\
Usage: ed [options] [file]\n\
\n\
Options:\n\
  -h, --help                 display this help and exit\n\
  -V, --version              output version information and exit\n\
  -p, --prompt=STRING        use STRING as an interactive prompt\n\
  -r, --restricted           run in restricted mode\n\
  -s, --quiet, --silent      suppress diagnostics\n\
  -v, --verbose              be verbose\n\
Start edit by reading in 'file' if given.\n\
If 'file' begins with a '!', read output of shell command.");
}

void print_version()
{
	printf("Ed %s\n", "0.0.1");
}

Command *parse_cmd(char *cmdstr)
{
	int beg = 0;
	Command *command;

	command = malloc(sizeof(Command));
	if (strlen(cmdstr) == 1) {
		strcpy(command->cmd, cmdstr);
		command->range.beg = 0;
		command->range.end = 0;
		return command;
	}
}

char *read_line(FILE *fs)
{
	int ch;
	char *buf;
	int i = 0;
	int buffsize = BUFFSIZE;

	buf = charalloc(BUFFSIZE);
	while ((ch = fgetc(fs)) != EOF) {
		/* check for overflow */
		if (i == BUFFSIZE) {
			buffsize += BUFFSIZE;
			buf = charrealloc(buf, buffsize);
			if (buf == NULL) {
				/* Err */
			}
		}

		buf[i++] = (char)ch;
		buf[i] = '\0';

		if (buf[i - 1] == '\n') {
			return buf;
		}
	}
	if (strlen(buf) != 0) {
		return buf;
	}

	return NULL;
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

		line = new_line(charbuf, ++line_no);
		push_back_line(curbuf, line);
	}
}

function find_function(Command *command)
{
	function func;
	switch (command->cmd[0]) {
	case 'a':
		func = append;
		break;
	}
	return func;
}

Buffer *read_file(const char *path)
{
	int ch;
	FILE *fs;
	int i = 0;
	int line_no = 0;
	char *buf;
	Line *line;
	Buffer *buffer;
	size_t buffsize = BUFFSIZE;

	fs = fopen(path, "rw");
	if (fs == NULL) {
		/* Err */
	}

	buffer = new_buffer(path);
	while ((buf = read_line(fs)) != NULL) {
		line_no++;
		line = new_line(buf, line_no);
		push_back_line(buffer, line);
	}
	fclose(fs);

	return buffer;
}

int ed()
{
	char *cmdbuf;
	Command *command;
	function func;

	cmdbuf = malloc(BUFFSIZE * sizeof(char));
	while (fgets(cmdbuf, BUFFSIZE, stdin) != NULL) {
		strstrip(cmdbuf);	
		command = parse_cmd(cmdbuf);
		func = find_function(command);
		func(command);
	}
	free(cmdbuf);
	/* Get a command
	 * Parse the command
	 * Execute the command */
	return 0;
}

int main(int argc, char **argv)
{
	int opt;
	int retval;
	int longindex;
	char *optstring = "hlp:rsvV";
	struct option longopts[] = {
		{"help", 0, NULL, 'h'},
		{"loose-exit-status", 0, NULL, 'l'},
		{"prompt", 1, NULL, 'p'},
		{"restricted", 0, NULL, 'r'},
		{"verbose", 0, NULL, 'v'},
		{"version", 0, NULL, 'V'},
		{NULL, 0, NULL, 0},
	};

	while ((opt = getopt_long(argc, argv, 
				optstring, longopts, &longindex)) != -1) {
		switch (opt) {
		case 'l':
			break;
		case 'p':
			break;
		case 'r':
			break;
		case 'v':
			break;
		case 'V':
			print_version();
			exit(EXIT_SUCCESS);
		case 'h':
			print_help();
			exit(EXIT_SUCCESS);
		default: /* ? */
			exit(EXIT_FAILURE);
		}
	}

	retval = ed();

	return retval;
}