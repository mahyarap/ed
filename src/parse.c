#include "ed.h"
#include <string.h>
#include <regex.h>

static regex_t regex;
static char *pattern =
	"^(([.$0-9]+)(,([.$0-9]+))?)?(([a-z])[[:blank:]]*([[:print:]]*))?$";
static const size_t nmatch = 7 + 1;

#define REGEX_RANGE_BEG 2
#define REGEX_RANGE_END 4
#define REGEX_CMD       6
#define REGEX_ARG       7

#define RANGE_INVALID 0
#define RANGE_NOTSET -1
#define RANGE_DOT    -2
#define RANGE_DOLLAR -3

typedef struct CommandPhase1 {
	/* The range of lines the functions act on */
	struct {
		char beg[32];
		char end[32];
	} range;
	/* A one-character command */
	char cmd;
	/* Contains the arguments passed to the functions */
	char arg[PATH_MAX];
} CommandPhase1;

static void init_command_phase_1(CommandPhase1 *cmdphase1);
static int parse_phase_1(CommandPhase1 *cmdphase1, const char *cmdstr);
static int parse_phase_2(Command *command, const CommandPhase1 *cmdphase1);
static int parse_phase_3(Command *command);

void init_regex()
{
	int retval;

	memset(&regex, 0, sizeof(regex_t));
	retval = regcomp(&regex, pattern, REG_EXTENDED);
	if (retval != 0) {
		exit(EXIT_FAILURE);
	}
}

int parse_command(Command *command, const char *cmdstr)
{
	int retval;
	CommandPhase1 cmdphase1;

	init_command_phase_1(&cmdphase1);

	retval = parse_phase_1(&cmdphase1, cmdstr);
	if (retval != 0) {
		return retval;
	}
	retval = parse_phase_2(command, &cmdphase1);
	if (retval != 0) {
		return retval;
	}
	retval = parse_phase_3(command);
	if (retval != 0) {
		return retval;
	}

	return 0;
}

static void init_command_phase_1(CommandPhase1 *cmdphase1)
{
	cmdphase1->range.beg[0] = '\0';
	cmdphase1->range.end[0] = '\0';
	cmdphase1->cmd = '\0';
	cmdphase1->arg[0] = '\0';
}

static int parse_phase_1(CommandPhase1 *cmdphase1, const char *cmdstr)
{
	if (cmdstr == NULL || strlen(cmdstr) == 0) {
		return 1;
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
		return REG_NOMATCH;
	}

	int len = strlen(cmd_copy);
	if (match[REGEX_RANGE_BEG].rm_so != -1) {
		int beg = match[REGEX_RANGE_BEG].rm_so;
		int end = match[REGEX_RANGE_BEG].rm_eo;

		if (beg - end < len) {
			strncpy(cmdphase1->range.beg, cmd_copy + beg, end - beg);
			cmdphase1->range.beg[end] = '\0';
		}
		else {
			return 1;
		}
	}
	if (match[REGEX_RANGE_END].rm_so != -1) {
		int beg = match[REGEX_RANGE_END].rm_so;
		int end = match[REGEX_RANGE_END].rm_eo;

		if (beg - end < len) {
			strncpy(cmdphase1->range.end, cmd_copy + beg, end - beg);
			cmdphase1->range.end[end] = '\0';
		}
		else {
			return 1;
		}
	}

	if (match[REGEX_CMD].rm_so != -1) {
		cmdphase1->cmd = cmd_copy[match[REGEX_CMD].rm_so];
	}

	if (match[REGEX_ARG].rm_so != -1) {
		int beg = match[REGEX_ARG].rm_so;
		int end = match[REGEX_ARG].rm_eo;

		if (beg - end < PATH_MAX) {
			strcpy(cmdphase1->arg, cmd_copy + beg);
		}
		else {
			return 1;
		}
	}

	return 0;
}

static int parse_phase_2(Command *command, const CommandPhase1 *cmdphase1)
{
	/* Range */
	if (strlen(cmdphase1->range.beg) > 0) {
		if (strcmp(cmdphase1->range.beg, "$") == 0) {
			command->range.beg = RANGE_DOLLAR;
		}
		else if (strcmp(cmdphase1->range.beg, ".") == 0) {
			command->range.beg = RANGE_DOT;
		}
		else {
			long converted;

			converted = strtol(cmdphase1->range.beg, NULL, 10);
			if (converted > 0 && converted != LONG_MAX) {
				command->range.beg = converted;
			}
			else {
				return 1;
			}
		}
	}
	else {
		command->range.beg = RANGE_NOTSET;
	}
	if (strlen(cmdphase1->range.end) > 0) {
		if (strcmp(cmdphase1->range.end, "$") == 0) {
			command->range.end = RANGE_DOLLAR;
		}
		else if (strcmp(cmdphase1->range.end, ".") == 0) {
			command->range.end = RANGE_DOT;
		}
		else {
			long converted;

			converted = strtol(cmdphase1->range.end, NULL, 10);
			if (converted > 0 && converted != LONG_MAX) {
				command->range.end = converted;
			}
			else {
				return 1;
			}
		}
	}
	else {
		command->range.end = RANGE_NOTSET;
	}

	/* Cmd */
	command->cmd = cmdphase1->cmd;

	/* Arg */
	if (strlen(cmdphase1->arg) > 0) {
		strcpy(command->arg, cmdphase1->arg);
	}

	return 0;
}

static int parse_phase_3(Command *command)
{
	long beg, end;

	beg = command->range.beg;
	end = command->range.end;
	if (beg == RANGE_NOTSET && end > 0) {
		return 1;
	}

	switch (beg) {
	case RANGE_DOT:
		beg = (curbuf->cur_line != NULL) ?
			curbuf->cur_line->line_no : RANGE_INVALID;
		break;
	case RANGE_DOLLAR:
			beg = (curbuf->last_line != NULL) ?
			curbuf->last_line->line_no : RANGE_INVALID;
	case RANGE_NOTSET:
		beg = (curbuf->cur_line != NULL) ?
			curbuf->cur_line->line_no : RANGE_INVALID;
		break;
	default:
		/* beg > 0 */
		if (curbuf->last_line == NULL || beg > curbuf->last_line->line_no) {
			beg = RANGE_INVALID;
		}
	}
	switch (end) {
	case RANGE_DOT:
		end = (curbuf->cur_line != NULL) ?
			curbuf->cur_line->line_no : RANGE_INVALID;
		break;
	case RANGE_DOLLAR:
		end = (curbuf->last_line != NULL) ?
			curbuf->last_line->line_no : RANGE_INVALID;
		break;
	case RANGE_NOTSET:
		end = beg;
		break;
	default:
		/* end > 0 */
		if (curbuf->last_line == NULL || end > curbuf->last_line->line_no) {
			end = RANGE_INVALID;
		}
	}
	command->range.beg = beg;
	command->range.end = end;

	if (command->cmd == '\0' && beg > 0 && end > 0) {
		command->cmd = 'p';
	}

	return 0;
}
