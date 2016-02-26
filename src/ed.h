#ifndef ED_H
#define ED_H
#include <stdlib.h>

#define BUFFSIZE 128

typedef struct Range {
	int beg;
	int end;
} Range;

typedef struct Command {
	Range range;
	char cmd[3];
} Command;

typedef struct Line {
	char *text;
	int line_no;
	struct Line *next;
	struct Line *prev;
} Line;

typedef struct Buffer {
	char *path;
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
#endif
