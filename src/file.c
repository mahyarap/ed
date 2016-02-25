#include "ed.h"
#include <stdlib.h>
#include <string.h>
#include <assert.h>

Buffer *new_buffer(const char *path)
{
	Buffer *buffer;

	assert(path != NULL);

	buffer = malloc(sizeof(Buffer));
	buffer->path = charalloc(strlen(path) + 1);
	strcpy(buffer->path, path);
	buffer->first_line = NULL;
	buffer->last_line = NULL;
	buffer->cur_line = NULL;

	return buffer;
}

Line *new_line(const char *text, int line_no)
{
	Line *line;

	line = malloc(sizeof(Line));
	line->text = charalloc(strlen(text) + 1);
	strcpy(line->text, text);
	line->line_no = line_no;
	line->next = NULL;
	line->prev = NULL;

	return line;
}

void push_back_line(Buffer *buffer, Line *line)
{
	assert(buffer != NULL);

	if (buffer->last_line == NULL) {
		buffer->first_line = line;
		buffer->last_line = line;
		buffer->cur_line = line;
	}
	else {
		line->prev = buffer->last_line;
		line->next = NULL;
		buffer->last_line->next = line;
		buffer->last_line = line;
	}
}

void push_front_line(Buffer *buffer, Line *line)
{
	assert(buffer != NULL);

	if (buffer->first_line == NULL) {
		buffer->first_line = line;
		buffer->last_line = line;
		buffer->cur_line = line;
	}
	else {
		line->prev = NULL;
		line->next = buffer->first_line;
		buffer->first_line->prev = line;
		buffer->first_line = line;
	}
}

void insert_line(Buffer *buffer, Line *line)
{
	assert(buffer != NULL);

	if (buffer->first_line == NULL) {
		buffer->first_line = line;
		buffer->last_line = line;
		buffer->cur_line = line;
	}
	else if (buffer->cur_line == buffer->last_line) {
		push_back_line(buffer, line);
	}
	else {
		line->prev = buffer->cur_line->next;
		line->next = buffer->cur_line->next->prev;
		buffer->cur_line->next = line;
		buffer->cur_line->next->prev = line;
		buffer->cur_line = line;
	}
}
