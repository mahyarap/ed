#include "ed.h"
#include <stdlib.h>
#include <string.h>

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

void write_buffer(const char *path)
{
	Line *p;
	FILE *fs;

	fs = fopen(path, "w+");
	if (fs == NULL) {
		/* Err */
	}

	for (p = curbuf->first_line; p != NULL; p = p->next) {
		fputs(p->text, fs);
	}
	fclose(fs);
}

Buffer *new_buffer(const char *path)
{
	Buffer *buffer;

	buffer = malloc(sizeof(Buffer));
	if (path != NULL && strlen(path) != 0) {
		strcpy(buffer->path, path);
	}
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
