#ifndef ED_H
#define ED_H
#include <stdlib.h>
#include <stdio.h>
#include <limits.h>
#include <assert.h>

#define BUFFSIZE 128

typedef struct Range {
	int beg;
	int end;
} Range;

typedef struct Command {
	Range range;
	char cmd;
	char *arg;
} Command;

typedef struct Line {
	char *text;
	int line_no;
	struct Line *next;
	struct Line *prev;
} Line;

typedef struct Buffer {
	char path[PATH_MAX];
	Line *first_line;
	Line *last_line;
	Line *cur_line;
} Buffer;

extern Buffer *curbuf;

typedef void (*function)(Command*);

void strstrip(char *str);
char *charalloc(size_t len);
char *charrealloc(char *buffer, size_t len);
Buffer *new_buffer(const char *path);
Line *new_line(const char *text, int line_no);
void push_back_line(Buffer *buffer, Line *line);
void push_front_line(Buffer *buffer, Line *line);
void insert_line(Buffer *buffer, Line *line);
Command *parse_cmd(char *cmdstr);
function find_function(Command *command);
void append(Command *command);
void quit(Command *command);
void unknown(Command *command);
char *read_line(FILE *fs);
void write_buffer(const char *path);
void write_out(Command *command);
void delete_cmd(Command *command);
void clrstr(char *str);
Command *new_cmd();
#endif
