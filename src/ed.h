#ifndef ED_H
#define ED_H

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif
#include <stdlib.h>
#include <stdio.h>
#include <limits.h>
#include <stdbool.h>
#include <assert.h>

/* Constants */
#define BUFFSIZE 128

/* Data structures */

typedef struct Range {
	/* Starting line */
	long beg;
	/* Ending line */
	long end;
} Range;

typedef struct Command {
	/* The range of lines on which functions operate */
	Range range;
	/* A one-character command */
	char cmd;
	/* Contains the arguments passed to functions */
	char *arg;
} Command;

typedef struct Line {
	/* Holds the actual text */
	char *text;
	/* The line number */
	int line_no;
	struct Line *next;
	struct Line *prev;
} Line;

typedef struct Buffer {
	/* The path of the associated file */
	char path[PATH_MAX];
	bool modified;
	Line *first_line;
	Line *last_line;
	Line *cur_line;
} Buffer;

/* Declaration of globals */

/* Points to the single global buffer */
extern Buffer *curbuf;

/* Typedefs */
typedef void (*function)(Command*);

/* Function prototypes */

/* Utils.c */
char *charalloc(size_t len);
char *charrealloc(char *buffer, size_t len);
void clrstr(char *str);
void strstrip(char *str);

/* file.c */
Buffer *new_buffer(const char *path);
Line *new_line(const char *text, int line_no);
bool is_empty_buffer(Buffer *buffer);
char *read_line(FILE *fs);
ssize_t read_file(Buffer *buffer, const char *path);
ssize_t write_buffer(const char *path);
void delete_buffer(Buffer *buffer);
void delete_line(Line *line);
void insert_line(Buffer *buffer, Line *line);
void pop_back_line(Buffer *buffer);
void pop_front_line(Buffer *buffer);
void push_back_line(Buffer *buffer, Line *line);
void push_front_line(Buffer *buffer, Line *line);
void remove_line(Buffer *buffer, Line *line);

/* cmds.c */
Command *new_command(const char *cmdstr);
function find_function(Command *command);
int parse_command(Command *command, const char *cmdstr);
void append(Command *command);
void delete(Command *command);
void delete_command(Command *command);
void edit(Command *command);
void find(Command *command);
void init_regex();
void print(Command *command);
void quit(Command *command);
void read_in(Command *command);
void unknown(Command *command);
void write_out(Command *command);

#endif
