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
	printf("Ed %s\n", VERSION);
}

int ed()
{
	char *cmdstr;
	Command *command;
	function func;

	cmdstr = charalloc(BUFFSIZE);
	/* Main Loop
	 *
	 * Get a command
	 * Parse the command
	 * Execute the command */
	curbuf = new_buffer(NULL);
	while (fgets(cmdstr, BUFFSIZE, stdin) != NULL) {
		command = new_command(cmdstr);
		func = find_function(command);
		func(command);

		clrstr(cmdstr);
		delete_command(command);
	}
	free(cmdstr);
	
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
