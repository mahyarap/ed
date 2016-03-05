#ifndef ED_H
#define ED_H
#include <stdlib.h>
#include <stdio.h>
#include <limits.h>
#include <assert.h>

/* Constants */
#define BUFFSIZE 128

/* Data structures */

typedef struct Range {
	/* Beginning line */
	int beg;
	/* Ending line */
	int end;
} Range;

typedef struct Command {
	/* The range of lines the functions act on */
	Range range;
	/* A one-character command */
	char cmd;
	/* Contains the arguments passed to the functions */
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
void strstrip(char *str);
char *charalloc(size_t len);
char *charrealloc(char *buffer, size_t len);
void clrstr(char *str);

/* file.c */
Buffer *new_buffer(const char *path);
Line *new_line(const char *text, int line_no);
void push_back_line(Buffer *buffer, Line *line);
void push_front_line(Buffer *buffer, Line *line);
void insert_line(Buffer *buffer, Line *line);
char *read_line(FILE *fs);
int read_file(const char *path);
int write_buffer(const char *path);

/* cmds.c */
Command *new_command(char *cmdstr);
void delete_cmd(Command *command);
void *parse_command(Command *command, char *cmdstr);
function find_function(Command *command);
void append(Command *command);
void quit(Command *command);
void read_in(Command *command);
void write_out(Command *command);
void unknown(Command *command);
#endif
